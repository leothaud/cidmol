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
export module core:logicalResult;

import :errors;
import :stream;
import :string;
import :traits;

namespace core {

//! @cond INTERNAL

export template <typename T> class LogicalResult;

export template <> class LogicalResult<void> {
  bool result;
  String message;

  LogicalResult(bool result, String message)
      : result(result), message(message) {}

public:
  [[nodiscard]] static LogicalResult<void> success() {
    return LogicalResult<void>(true, "");
  }
  [[nodiscard]] static LogicalResult<void> failure(core::String message) {
    return LogicalResult<void>(false, message);
  }

  bool succeed() const { return result; }
  bool failed() const { return !result; }
  String getMessage() const {
    assert(!result, "getMessage on not failed LogicalResult.\n");
    return message;
  }

  template <typename T> operator LogicalResult<T>() {
    assert(!result, "LogicalResult conversion are only available on failure.");
    return LogicalResult<T>::failure(message);
  }

  friend core::FdStream &operator<<(FdStream &stream, LogicalResult result);
};

//! @endcond

//! Class used as function result values for function that can fail.
//! Either contains the real result value or an error message
export template <typename T> class LogicalResult {
  bool result;
  String message;
  T value;

  LogicalResult(bool result, String message, const T &value)
      : result(result), message(message), value(value) {}

  LogicalResult(bool result, String message, T &&value)
      : result(result), message(message), value(move(value)) {}

public:
  [[nodiscard]] static LogicalResult<T> success(const T &value) {
    return LogicalResult<T>(true, "", value);
  }
  [[nodiscard]] static LogicalResult<T> success(T &&value) {
    return LogicalResult<T>(true, "", move(value));
  }
  [[nodiscard]] static LogicalResult<T> failure(core::String message) {
    return LogicalResult<T>(false, message, T{});
  }

  template <typename U>
    requires(isConstructible<T, U>)
  LogicalResult(U &&value)
      : result(true), message(""), value{forward<U>(value)} {}

  LogicalResult(const T &value) : result(true), message(""), value(value) {}
  LogicalResult(T &&value) : result(true), message(""), value(move(value)) {}

  bool succeed() const { return result; }
  bool failed() const { return !result; }
  String getMessage() const {
    assert(!result, "getMessage on not failed LogicalResult.\n");
    return message;
  }
  T &getValue() const {
    assert(result, "getValue on failed LogicalResult.\n");
    return value;
  }
  T &getValue() {
    assert(result, "getValue on failed LogicalResult.\n");
    return value;
  }

  T &operator*() const { return getValue(); }

  T &operator*() { return getValue(); }
  T *operator->() const { return &value; }
  T *operator->() { return &value; }

  template <typename T2> operator LogicalResult<T2>() {
    assert(!result, "LogicalResult conversion are only available on failure.");
    return LogicalResult<T2>::failure(message);
  }

  friend core::FdStream &operator<<(FdStream &stream, LogicalResult result);
};

export template <typename T>
core::FdStream &operator<<(FdStream &stream, LogicalResult<T> result) {
  core::assert(result.failed());
  return (stream << result.message);
}

} // namespace core