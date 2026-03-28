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
export module core:threads;

import :builtins;
import :syscalls;
import :functions;
import :stream;
import :mutex;
import :vector;
import :start;
import :logger;

#ifdef CORE_THREAD

namespace core {

//! Wrapper for the futex allowing the parent thread to check or wait for the
//! thread execution.
export class ExecutionFutex {
  int futex;

  void set(int value) { __atomic_store_n(&futex, value, __ATOMIC_SEQ_CST); }
  void wakeAll() { futexWakeAll(&futex); }

  friend void terminate(ExecutionFutex *);

public:
  ExecutionFutex() : futex(1) {}

  //! Non blocking check.
  //! @return `true` iff the execution is terminated
  bool finished() { return __atomic_load_n(&futex, __ATOMIC_SEQ_CST) == 0; }
  //! Blocking method. Returns only when the execution is terminated.
  void wait() {
    while (__atomic_load_n(&futex, __ATOMIC_SEQ_CST) != 0) {
      futexWait(&futex, 0);
    }
  }
};

//! Tells that the thread execution is terminated. Should only be called by
//! the child thread.
void terminate(ExecutionFutex *futex) {
  futex->set(0);
  futex->wakeAll();
}

//! Represent the data given to the new thread.
//! @warning No dot modify the first two elements,
//! the first one is necessary as first for thread starting
//! The tlsHeader is accessed by offset
struct [[gnu::aligned(16), gnu::packed]] stackHead {
  //! void threadEntry(stackHead *head) function pointer.
  void (*entry)(stackHead *);
  //! Pointer to the tlsHeader
  TlsHeader *tlsHeader;
  //! Futex used to give information on execution termination
  ExecutionFutex *futex;
  //! Wrapper for the target function
  void (*fn)(u8 *);
  //! wrapper for the target function arguments
  u8 *arg;
  //! Top of the stack for the new thread
  u8 *stack;
  //! logger to initialize the threadLogger with
  Logger *logger;
};

//! Entry functino for new threads.
[[clang::noinline, clang::no_stack_protector]] void
threadEntry(stackHead *head) {
  // Initialize the thread logger.
  getThreadLogger() = head->logger;
  // Call the target function wrapper
  head->fn(head->arg);
  // Broadcast termination
  if (head->futex) {
    terminate(head->futex);
  }

  // Call the thread_local variable destructors.
  auto &cxaAtExitWrapper = getCxaThreadAtExitWrapper();
  for (u64 i = cxaAtExitWrapper.index; i > 0; --i) {
    cxaAtExitWrapper.func[i](cxaAtExitWrapper.arg[i]);
    cxaAtExitWrapper.func[i] = nullptr;
  }

  // Free the thread data and stack
  // Warning: this will work with the current allocator
  // and exit implementation. But may break if allocator change
  delete[] head->tlsHeader->tlsInfo;
  delete[] head->arg;
  delete[] head->stack;
  exit(0);
}

//! Create a new thread with `stack` as stack
[[gnu::naked]] long newthread(stackHead *stack) {
#if defined(linux) and defined(__x86_64__) and __has_extension(gnu_asm)
  __asm__ volatile("mov %%rdi, %%rsi\n"
                   "mov 8(%%rsi), %%r8\n"
                   "mov $0xd0f00, %%edi\n"
                   "mov $56, %%eax\n"
                   "xor %%edx, %%edx\n"
                   "xor %%r10d, %%r10d\n"
                   "syscall\n"
                   "mov %%rsp, %%rdi\n"
                   "ret\n" ::
                       : "rax", "rcx", "rsi", "rdi", "r8", "r10", "r11",
                         "memory");
#else
#error No alternative for newthread function.
#endif
}

//! @cond INTERNAL
template <typename... ARGS> struct ParameterPack;
template <> struct ParameterPack<> {
  ParameterPack() {}
};
template <typename H, typename... T> struct ParameterPack<H, T...> {
  H head;
  ParameterPack<T...> tail;
  ParameterPack(H head, T... tail) : head(head), tail(forward<T>(tail)...) {}
};
template <typename F, typename... ARGS> struct FunEntry {
  F fun;
  ParameterPack<ARGS...> args;

  FunEntry(F fun, ARGS... args) : fun(fun), args(forward<ARGS>(args)...) {}
};

template <u64 I, typename T> struct ArgsBuilder;

template <typename H, typename... T>
struct ArgsBuilder<0, ParameterPack<H, T...>> {
  static H &get(ParameterPack<H, T...> &a) { return a.head; }
};

template <u64 I, typename H, typename... T>
struct ArgsBuilder<I, ParameterPack<H, T...>> {
  static auto &get(ParameterPack<H, T...> &a) {
    return ArgsBuilder<I - 1, ParameterPack<T...>>::get(a.tail);
  }
};

template <typename F, typename... ARGS, u64... I>
void doCallImpl(F &fun, ParameterPack<ARGS...> args, IndicesSequence<I...>) {
  fun(ArgsBuilder<I, ParameterPack<ARGS...>>::get(args)...);
}
//! @endcond

//! Wrapper to call the target function
template <typename F, typename... ARGS>
  requires(IsCallable<F, ARGS...>)
void doCall(u8 *entry) {
  auto *funEntry = reinterpret_cast<FunEntry<F, ARGS...> *>(entry);
  doCallImpl(funEntry->fun, funEntry->args,
             IndicesSequenceBuilder<sizeof...(ARGS)>{});
  funEntry->~FunEntry();
}

//! Options given to the startThread function.
export class StartThreadOptions {
  //! Initial threadLogger value
  Logger *const logger;
  //! Pointer to the termination futex
  ExecutionFutex *const futex;

  template <auto fn, typename... ARGS>
    requires(IsCallable<decltype(fn), ARGS...>)
  friend void startThread(StartThreadOptions options, ARGS &&...args);

public:
  constexpr StartThreadOptions() : logger(&stdLogger), futex(nullptr) {}
  constexpr StartThreadOptions(Logger *logger)
      : logger(logger), futex(nullptr) {}
  constexpr StartThreadOptions(ExecutionFutex *futex)
      : logger(&stdLogger), futex(futex) {}
  constexpr StartThreadOptions(Logger *logger, ExecutionFutex *futex)
      : logger(logger), futex(futex) {}
};

//! Start a thread that call function `fn` with arguments `args...` and
//! initialized with loggers and futex in `options`.
export template <auto fn, typename... ARGS>
  requires(IsCallable<decltype(fn), ARGS...>)
[[clang::always_inline, clang::no_stack_protector]] void
startThread(StartThreadOptions options, ARGS &&...args) {
  static constexpr u64 stackSize = 8 * 1024 * 1024;
  u8 *stack = new u8[stackSize];
  stackHead *head = reinterpret_cast<stackHead *>(
      reinterpret_cast<u64>((stack + stackSize - sizeof(stackHead))) & ~0xFllu);
  u8 *entryPtr = new u8[sizeof(FunEntry<decltype(fn), ARGS...>)];
  FunEntry<decltype(fn), ARGS...> *entry = new (entryPtr)
      FunEntry<decltype(fn), ARGS...>(fn, forward<ARGS>(args)...);
  head->logger = options.logger;
  head->entry = threadEntry;
  head->futex = options.futex;
  head->fn = doCall<decltype(fn), ARGS...>;
  head->arg = entryPtr;
  head->stack = stack;
  head->tlsHeader = allocateTls();

  newthread(head);
}

//! Start a thread that call function `fn` with arguments `args...` and default
//! StartThreadOptions.
export template <auto fn, typename... ARGS>
  requires(IsCallable<decltype(fn), ARGS...>)
[[clang::always_inline, clang::no_stack_protector]] void
startThread(ARGS &&...args) {
  startThread<fn>(StartThreadOptions{}, forward<ARGS>(args)...);
}

}; // namespace core

#endif
