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
export module core:sharedPtr;

import :traits;
import :types;

namespace core {

export template <typename T> class SharedPtr {
  T *ptr;
  u64 *count;

  void finalize() {
    if (ptr && (--*count == 0)) {
      delete ptr;
      delete count;
    }
  }

public:
  ~SharedPtr() { finalize(); }

  SharedPtr() : ptr(nullptr), count(nullptr) {}
  explicit SharedPtr(T *ptr, u64 *count) : ptr(ptr), count(count) {}
  explicit SharedPtr(T *ptr) : ptr(ptr), count(ptr ? new u64(1) : nullptr) {}

  template <typename... ARGS>
  [[nodiscard]] static SharedPtr create(ARGS &&...args) {
    return SharedPtr(new T{core::forward<ARGS &&>(args)...});
  }

  SharedPtr(decltype(nullptr)) : ptr(nullptr), count(nullptr) {}

  SharedPtr(const SharedPtr &other) : ptr(other.ptr), count(other.count) {
    if (ptr)
      ++*count;
  }
  SharedPtr &operator=(const SharedPtr &other) {
    finalize();
    ptr = other.ptr;
    count = other.count;
    if (ptr)
      ++*count;
    return *this;
  }

  SharedPtr(SharedPtr &&other) : ptr(other.ptr), count(other.count) {
    other.ptr = nullptr;
    other.count = nullptr;
  }
  SharedPtr &operator=(SharedPtr &&other) {
    finalize();
    ptr = other.ptr;
    count = other.count;
    other.ptr = nullptr;
    other.count = nullptr;
    return *this;
  }

  T &operator*() const { return *ptr; }
  T *operator->() const { return ptr; }
  T *getRaw() const { return ptr; }

  bool operator==(const SharedPtr &other) const { return (ptr == other.ptr); }
  bool operator!=(const SharedPtr &other) const { return !(*this == other); }

  bool operator==(decltype(nullptr)) const { return ptr == nullptr; }
  bool operator!=(decltype(nullptr)) const { return !(*this == nullptr); }

  operator bool() const { return ptr; }

  template <typename T2> SharedPtr<T2> cast() const {
    ++*count;
    T2 *castedRaw = static_cast<T2 *>(ptr);
    return SharedPtr<T2>(castedRaw, count);
  }
};

} // namespace core