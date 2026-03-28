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
export module core:ioUtils;

import :builtins;
import :traits;

namespace core {

//! Wrapper to print numbers in hexadecimal format
export struct Hex {
  u64 value;

public:
  constexpr Hex(u64 value) : value(value) {}
  operator u64() { return value; }
};

} // namespace core

export constexpr core::Hex operator""_x(unsigned long long value) {
  return core::Hex(value);
}

namespace core {

namespace color {
export enum class text : int {
  RED = 31,
  GREEN = 32,
  YELLOW = 33,
  BLUE = 34,
  PURPLE = 35,
  CYAN = 36,
  WHITE = 37,

  DEFAULT = 0,
};

export enum class background : int {
  RED = 41,
  GREEN = 42,
  YELLOW = 43,
  BLUE = 44,
  PURPLE = 45,
  CYAN = 46,
  WHITE = 47,
  LGRAY = 100,
  LRED = 101,
  LGREEN = 102,
  LYELLOW = 103,
  LBLUE = 105,
  LCYAN = 106,
  LWHITE = 107,

  DEFAULT = 0,
};
} // namespace color

export class FdStream;

//! @cond INTERNAL
template <typename... T> struct ColorTuple;

template <typename H, typename... T>
  requires(sizeof...(T) > 0)
struct ColorTuple<H, T...> {
  H head;
  ColorTuple<T...> tail;

  ColorTuple(H head, T... tail) : head(head), tail(tail...) {}
  FdStream *printElements(FdStream *stream);
};

template <typename T> struct ColorTuple<T> {
  T elt;
  ColorTuple(T elt) : elt(elt) {}
  FdStream *printElements(FdStream *stream);
};

template <typename... T> struct Color {
  struct ColorOption {
    color::background backgroundColor;
    color::text textColor;
  } options;
  ColorTuple<T...> values;

  constexpr Color(color::background backgroundColor, color::text textColor,
                  T... values)
      : options{backgroundColor, textColor}, values(values...) {}
};

//! @endcond

//! Wrapper used to print values with a background color and a text color.
export template <color::background background, color::text text, typename... T>
auto getColor(T... values) {
  return Color<T...>(background, text, values...);
}

//! Wrapper used to print values with a background color and a text color.
export template <color::text text, color::background background, typename... T>
auto getColor(T... values) {
  return Color<T...>(background, text, values...);
}

//! Wrapper used to print values with a text color.
export template <color::text text, typename... T> auto getColor(T... values) {
  return Color<T...>(color::background::DEFAULT, text, values...);
}

//! Wrapper used to print values with a background color.
export template <color::background background, typename... T>
auto getColor(T... values) {
  return Color<T...>(background, color::text::DEFAULT, values...);
}

} // namespace core