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
export module core:uniquePtr;

import :traits;

namespace core {

//! Smart pointer own by one owner at a time.
export template <typename T>
  requires(!core::IsVoid<T>)
class UniquePtr {

  T *ptr;

public:
  ~UniquePtr() { delete ptr; }

  UniquePtr(decltype(nullptr)) : ptr(nullptr) {}

  UniquePtr() : ptr(nullptr) {}
  UniquePtr(T *ptr) : ptr(ptr) {}

  UniquePtr(const UniquePtr &) = delete;
  UniquePtr &operator=(const UniquePtr &) = delete;

  UniquePtr(UniquePtr &&other) : ptr(other.ptr) { other.ptr = nullptr; }
  UniquePtr &operator=(UniquePtr &&other) {
    delete ptr;
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  //! allocate the underlying pointer,
  //! and initialized it with the type constructors using `args...`.
  template <typename... ARGS>
  [[nodiscard]] static UniquePtr create(ARGS &&...args) {
    return UniquePtr(new T{core::forward<ARGS &&>(args)...});
  }

  T &operator*() const { return *ptr; }

  T *operator->() const { return ptr; }

  //! Returns the underlying pointer
  //! @warning use with caution: memory leak, etc.
  T *getRaw() const { return ptr; }

  bool operator==(decltype(nullptr)) const { return ptr == nullptr; }
  bool operator!=(decltype(nullptr)) const { return !(*this == nullptr); }

  operator bool() const { return (ptr != nullptr); }
};

} // namespace core