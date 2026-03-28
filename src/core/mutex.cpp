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
export module core:mutex;

import :builtins;
import :syscalls;

#ifdef CORE_THREAD

namespace core {

export void acquireMutex(volatile int *mut) {
#if __has_builtin(__atomic_exchange_n) and defined(__ATOMIC_ACQUIRE)
  while (__atomic_exchange_n(mut, 1, __ATOMIC_ACQUIRE)) {
    futexWait(mut, 1);
  }
#else
#error No alternative for acquireMutex.
#endif
}

export bool tryAcquireMutex(volatile int *mut) {
#if __has_builtin(__atomic_exchange_n) and defined(__ATOMIC_ACQUIRE)
  return __atomic_exchange_n(mut, 1, __ATOMIC_ACQUIRE) == 0;
#else
#error No alternative for tryAcquireMutex.
#endif
}

export void releaseMutex(volatile int *mut) {
#if __has_builtin(__atomic_clear) and defined(__ATOMIC_RELEASE)
  __atomic_clear(mut, __ATOMIC_RELEASE);
  futexWake(mut);
#else
#error No alternative for releaseMutex.
#endif
}

export struct Mutex {
  volatile int lock = 0;

  void acquire() { acquireMutex(&lock); }

  bool tryAcquire() { return tryAcquireMutex(&lock); }

  void release() { releaseMutex(&lock); }
};

} // namespace core

#endif