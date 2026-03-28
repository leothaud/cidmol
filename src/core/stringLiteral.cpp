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
export module core:stringLiteral;

import :string;
import :builtins;

namespace core {

//! char * wrapper to use it as litteral in templates.\n
//! @code
//! template <StringLiteral STR> class MyClass { ... };
//! class MyOtherClass: public MyClass<"other"> { ... };
//! @endcode
export template <u64 N> struct StringLiteral {
  char value[N];
  constexpr StringLiteral(const char (&str)[N]) {
    for (u64 i = 0; i < N; ++i)
      value[i] = str[i];
  }

  constexpr operator core::String() const { return core::String(value); }
};

} // namespace core