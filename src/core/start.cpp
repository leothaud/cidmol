/* Copyright 2026 Dylan Leothaud
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
export module core:start;

import :cstring;
import :builtins;
import :syscalls;
import :allocs;
import :mutex;
import :elf64;
import :logger;

namespace core {

extern "C" export [[gnu::no_stack_protector]] void
__cxa_pure_virtual() asm("__cxa_pure_virtual");
extern "C" export [[gnu::no_stack_protector]] void __cxa_pure_virtual() {
  getThreadLogger()->error("Calling a pure virtual method.");
  trap();
}

extern "C" export u64 __stack_chk_guard asm("__stack_chk_guard") = 167;
extern "C" export void __stack_chk_fail(void) asm("__stack_chk_fail");
extern "C" export void __stack_chk_fail(void) {
  getThreadLogger()->error("Stack check failed.");
  exit(1);
}

export enum : int {
  AT_NULL = 0,
  AT_IGNORE = 1,
  AT_EXECFD = 2,
  AT_PHDR = 3,
  AT_PHENT = 4,
  AT_PHNUM = 5,
  AT_PAGESZ = 6,
  AT_BASE = 7,
  AT_FLAGS = 8,
  AT_ENTRY = 9,
  AT_NOTELF = 10,
  AT_UID = 11,
  AT_EUID = 12,
  AT_GID = 13,
  AT_EGID = 14,
  AT_PLATFORM = 15,
  AT_HWCAP = 16,
  AT_CLKTCK = 17,
  AT_SECURE = 23,
  AT_BASE_PLATFORM = 24,
  AT_RANDOM = 25,
  AT_HWCAP2 = 26,
  AT_RSEQ_FEATURE_SIZE = 27,
  AT_RSEQ_ALIGN = 28,
  AT_HWCAP3 = 29,
  AT_HWCAP4 = 30,
  AT_EXECFN = 31,
  AT_MINSIGSTKSZ = 51,
  AT_SYSINFO = 32,
  AT_SYSINFO_EHDR = 33,
};

export union Elf32AuxvValue {
  long aVal;
  void *aPtr;
  void (*aFcn)(void);
};

export struct Elf32Auxv {
  int aType;
  Elf32AuxvValue aUn;
};

export Elf32AuxvValue auxvVals[33]{};

export struct TlsInfo {
  u64 memsz;
  u64 filesz;
  u64 vaddr;
  u64 align;
};
export struct TlsHeader {
  TlsHeader *self;
  u8 *tlsInfo;
  u64 dummy[3];
  u64 canary;
};

//! Tls for the main process
export TlsInfo tlsInfo{};

} // namespace core

extern "C++" {
int main();
int main(int);
int main(int, char **);
int main(int, char **, char **);
int main(int, char **, char **, core::Elf32Auxv *);
};

namespace core {

struct CxaAtExitElement {
  void (*func)(void *), *arg, *dso;
};
static inline constexpr u64 numCxaAtExitElements = 128;
CxaAtExitElement cxaAtExitElements[numCxaAtExitElements];
u32 cxaAtExitIndex = 0;

struct CxaThreadAtExitWrapper {
  void (*func[numCxaAtExitElements])(void *), *arg[numCxaAtExitElements];
  u32 index = 0;
};

export CxaThreadAtExitWrapper &getCxaThreadAtExitWrapper() {
  static thread_local CxaThreadAtExitWrapper wrapper{};
  return wrapper;
}

extern "C" void *__dso_handle = &__dso_handle;

extern "C" export [[gnu::no_stack_protector]] int
__cxa_atexit(void (*func)(void *), void *arg, void *dso) {
  if (cxaAtExitIndex == numCxaAtExitElements) {
    core::serr << "cxa_atexit stack too small.\n";
    core::exit(1);
  }
  cxaAtExitElements[cxaAtExitIndex].func = func;
  cxaAtExitElements[cxaAtExitIndex].arg = arg;
  cxaAtExitElements[cxaAtExitIndex].dso = dso;
  return 0;
}

extern "C" export [[gnu::no_stack_protector]] int
__cxa_thread_atexit(void(func)(void *), void *arg, void *) {
  auto &wrapper = getCxaThreadAtExitWrapper();
  if (wrapper.index == numCxaAtExitElements) {
    core::serr << "cxa_thread_atexit stack too small.\n";
    core::exit(1);
  }
  wrapper.func[wrapper.index] = func;
  wrapper.arg[wrapper.index] = arg;
  return 0;
}

extern "C" export [[gnu::no_stack_protector]] void __cxa_finalize(void *dso) {
  for (u64 i = cxaAtExitIndex; i > 0; --i) {

    if (cxaAtExitElements[i - 1].func &&
        (!dso || dso == cxaAtExitElements[i - 1].dso)) {
      cxaAtExitElements[i - 1].func(cxaAtExitElements[i - 1].arg);
      cxaAtExitElements[i - 1].func = nullptr; // prevent double call
    }
  }
}

//! Allocate a thread-local storage
export TlsHeader *allocateTls() {
  u64 align = tlsInfo.align > 16 ? tlsInfo.align : 16;
  u64 size = tlsInfo.memsz + sizeof(TlsHeader);
  u8 *memory = new u8[size + align];

  u8 *aligned = reinterpret_cast<u8 *>(
      (reinterpret_cast<u64>(memory) + align - 1) & ~(align - 1));

  if (tlsInfo.memsz > 0) {
    memcpy(reinterpret_cast<char *>(aligned),
           reinterpret_cast<const char *>(tlsInfo.vaddr), tlsInfo.filesz);
    memset(reinterpret_cast<char *>(aligned + tlsInfo.filesz), 0,
           tlsInfo.memsz - tlsInfo.filesz);
  }

  TlsHeader *header = reinterpret_cast<TlsHeader *>(aligned + tlsInfo.memsz);
  header->self = header;
  header->canary = *reinterpret_cast<u64 *>(auxvVals[AT_RANDOM].aPtr);
  header->tlsInfo = memory;
  return header;
}

extern "C" export [[gnu::no_stack_protector]] int init(int, char **, char **,
                                                       Elf32Auxv *) asm("init");

extern "C" export [[gnu::no_stack_protector]] int
init(int argc, char **argv, char **envp, Elf32Auxv *auxv) {

  // Initialize the auxilliary vector
  while (auxv->aType != AT_NULL) {
    if (auxv->aType < 33) {
      auxvVals[auxv->aType] = auxv->aUn;
    }
    ++auxv;
  }
  __stack_chk_guard = *reinterpret_cast<u64 *>(auxvVals[AT_RANDOM].aPtr);

  Elf64Phdr *phdrs = static_cast<Elf64Phdr *>(auxvVals[AT_PHDR].aPtr);
  u64 phnum = auxvVals[AT_PHNUM].aVal;
  u64 phent = auxvVals[AT_PHENT].aVal;
  for (u64 i = 0; i < phnum; ++i) {
    auto *ph = reinterpret_cast<Elf64Phdr *>(reinterpret_cast<u8 *>(phdrs) +
                                             i * phent);
    if (ph->p_type == PT_TLS) {
      tlsInfo = TlsInfo{
          .memsz = ph->p_memsz,
          .filesz = ph->p_filesz,
          .vaddr = ph->p_vaddr,
          .align = ph->p_align,
      };
    }
  }

  auto *tls = allocateTls();

  i64 archPrctlRes = archPrctl(reinterpret_cast<u64>(tls));
  if (archPrctlRes != 0) {
    write(stderr, "Error initializing tls.\n");
    exit(1);
  }

  initializeLoggers();
  int fd = open(argv[0], O_RDONLY, 0666);
  u64 size = getFileSize(fd);
  char *buffer = reinterpret_cast<char *>(
      mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
  close(fd);
  initializeSymbolNames(buffer);
  munmap(buffer, size);

  int res = main();
  if (res) {
    __cxa_finalize(nullptr);
    return res;
  }
  res = main(argc);
  if (res) {
    __cxa_finalize(nullptr);
    return res;
  }
  res = main(argc, argv);
  if (res) {
    __cxa_finalize(nullptr);
    return res;
  }
  res = main(argc, argv, envp);
  if (res) {
    __cxa_finalize(nullptr);
    return res;
  }
  res = main(argc, argv, envp, auxv);
  __cxa_finalize(nullptr);
  return res;
}
}; // namespace core

extern "C++" {

static inline u64 doAlign(u64 n, u64 a) { return (n + a - 1) & ~(a - 1); }

export enum class AlignVal : u64 {};

export void *operator new(u64 size) {
  return core::DefaultAllocator::get().allocate(size);
}

export void operator delete(void *ptr) noexcept {
  if (!ptr)
    return;
  core::DefaultAllocator::get().free(ptr);
}

export void operator delete(void *ptr, u64) noexcept { operator delete(ptr); }

export void *operator new[](u64 size) { return operator new(size); }

export void operator delete[](void *ptr) noexcept { operator delete(ptr); }

export void operator delete[](void *ptr, u64) noexcept { operator delete(ptr); }

export void *operator new(u64 size, AlignVal al) {
  u64 total = size + static_cast<u64>(al) + sizeof(void *);
  void *raw = core::DefaultAllocator::get().allocate(total);
  u64 *ptr = reinterpret_cast<u64 *>(raw);
  void *alignedPtr = reinterpret_cast<u64 *>(
      doAlign(reinterpret_cast<u64>(ptr), static_cast<u64>(al)));
  reinterpret_cast<void **>(alignedPtr)[-1] = raw;
  return alignedPtr;
}

export void operator delete(void *ptr, AlignVal) {
  if (!ptr)
    return;
  void *raw = reinterpret_cast<void **>(ptr)[-1];
  core::DefaultAllocator::get().free(raw);
}

export void operator delete(void *ptr, u64, AlignVal al) {
  operator delete(ptr, al);
}

export void *operator new[](u64 size, AlignVal al) {
  return operator new(size, al);
}

export void operator delete[](void *ptr, AlignVal al) {
  return operator delete(ptr, al);
}

export void operator delete[](void *ptr, u64, AlignVal al) {
  return operator delete(ptr, al);
}

export void *operator new(u64, void *ptr) { return ptr; }

export void *operator new[](u64, void *ptr) { return ptr; }

export [[gnu::no_stack_protector, gnu::no_instrument_function]] void
__cxa_guard_release(u64 *guard) asm("__cxa_guard_release");

export [[gnu::no_stack_protector, gnu::no_instrument_function]] void
__cxa_guard_release(u64 *guard) {
#ifdef CORE_THREAD
#if __has_builtin(__atomic_store_n) and defined(__ATOMIC_RELEASE)
  __atomic_store_n(reinterpret_cast<u8 *>(guard), (u8)1, __ATOMIC_RELEASE);
#else
#error No alternative for cxa_guard_release
#endif
#else
  *guard |= 1;
#endif
}

export [[gnu::no_stack_protector, gnu::no_instrument_function]] void
__cxa_guard_abort(u64 *guard) asm("__cxa_guard_abort");

export [[gnu::no_stack_protector, gnu::no_instrument_function]] void
__cxa_guard_abort(u64 *guard) {
#ifdef CORE_THREAD
#if __has_builtin(__atomic_store_n) and defined(__ATOMIC_RELEASE)
  __atomic_store_n(reinterpret_cast<u8 *>(guard), (u8)0, __ATOMIC_RELEASE);
#else
#error No alternative for cxa_guard_release
#endif
#else
  *guard |= 0;
#endif
}

export [[gnu::no_stack_protector, gnu::no_instrument_function]] int
__cxa_guard_acquire(u64 *guard) asm("__cxa_guard_acquire");

export [[gnu::no_stack_protector, gnu::no_instrument_function]] int
__cxa_guard_acquire(u64 *guard) {
#ifdef CORE_THREAD
#if __has_builtin(__atomic_load_n) and defined(__ATOMIC_ACQUIRE) and           \
    __has_builtin(__atomic_compare_exchange_n) and defined(__ATOMIC_RELAXED)
  u8 *g = reinterpret_cast<u8 *>(guard);
  if (__atomic_load_n(g, __ATOMIC_ACQUIRE) == 1)
    return 0;
  u8 expected = 0;
  while (!__atomic_compare_exchange_n(g, &expected, 0xFF, false,
                                      __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
    if (expected == 1)
      return 0;
    expected = 0;
  }
  return 1;
#else
#error No alternative for __cxa_guard_acquire
#endif
#else
  return !(*guard & 1);
#endif
}
}