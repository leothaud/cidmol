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

#ifdef CORE_THREAD

namespace core {

struct [[gnu::aligned(16)]] stackHead {
  void (*entry)(stackHead *);
  int futex;
  void (*fn)(u8 *);
  u8 *arg;
  u8 *stack;
};

[[clang::noinline, clang::no_stack_protector]] void
threadEntry(stackHead *head) {
  head->fn(head->arg);
  __atomic_store_n(&head->futex, 0, __ATOMIC_SEQ_CST);
  futexWakeAll(&head->futex);
  delete[] head->arg;
  delete[] head->stack;
  exit(0);
}

struct Thread {
  int tid;
};

[[gnu::naked]] long newthread(stackHead *stack) {
#if defined(linux) and defined(__x86_64__) and __has_extension(gnu_asm)
  __asm__ volatile("mov %%rdi, %%rsi\n"
                   "mov $0x50f00, %%edi\n"
                   "mov $56, %%eax\n"
                   "syscall\n"
                   "mov %%rsp, %%rdi\n"
                   "ret\n" ::
                       : "rax", "rcx", "rsi", "rdi", "r11", "memory");
#else
#error No alternative for newthread function.
#endif
}

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

template <typename F, typename... ARGS>
  requires(IsCallable<F, ARGS...>)
void doCall(u8 *entry) {
  auto *funEntry = reinterpret_cast<FunEntry<F, ARGS...> *>(entry);
  doCallImpl(funEntry->fun, funEntry->args,
             IndicesSequenceBuilder<sizeof...(ARGS)>{});
  funEntry->~FunEntry();
}

export template <auto fn, typename... ARGS>
  requires(IsCallable<decltype(fn), ARGS...>)
[[clang::always_inline, clang::no_stack_protector]] int *
startThread(ARGS... args) {
  static constexpr u64 stackSize = 8 * 1024 * 1024;
  u8 *stack = new u8[stackSize];
  stackHead *head = reinterpret_cast<stackHead *>(
      reinterpret_cast<u64>((stack + stackSize - sizeof(stackHead))) & ~0xFllu);
  u8 *entryPtr = new u8[sizeof(FunEntry<decltype(fn), ARGS...>)];
  FunEntry<decltype(fn), ARGS...> *entry =
      new (entryPtr) FunEntry<decltype(fn), ARGS...>(fn, args...);
  head->entry = threadEntry;
  head->futex = 1;
  head->fn = doCall<decltype(fn), ARGS...>;
  head->arg = entryPtr;
  head->stack = stack;
  newthread(head);
  return &head->futex;
}

export template <auto fn, typename... ARGS>
  requires(!IsCallable<decltype(fn), ARGS...>)
[[clang::always_inline, clang::no_stack_protector]] int *startThread(ARGS...) {
  static_assert(
      false,
      "startThread arguments are not compatible with the target function.");
}

}; // namespace core

#endif
