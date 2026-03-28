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
export module core:initializerList;

import :builtins;

//! @cond INTERNAL

namespace std {
export template <typename T> class initializer_list {
private:
  const T *array_;
  u64 length_;

public:
  const T *begin() { return array_; }
  const T *end() { return array_ + length_; }
};
} // namespace std

//! @endcond

namespace core {
//! Wrapper to use standard C++ initializee_list in constructors.
export template <typename T> using InitializerList = ::std::initializer_list<T>;
} // namespace core