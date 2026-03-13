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
export module core:traits;

import :types;

export template <typename T> T &&declval() noexcept;

namespace core {

template <bool B, typename T = void> struct EnableIfBase {};

template <typename T> struct EnableIfBase<true, T> {
  using type = T;
};
export template <bool B, typename T = void>
using enableIf = typename EnableIfBase<B, T>::type;

template <typename T1, typename T2> struct IsSameBase {
  static constexpr bool value = false;
};

template <typename T> struct IsSameBase<T, T> {
  static constexpr bool value = true;
};

export template <typename T1, typename T2>
constexpr bool isSame = IsSameBase<T1, T2>::value;

template <typename T> struct RValueBase {
  using type = T &&;
};

template <typename T> struct RValueBase<T &> {
  using type = T &&;
};

template <typename T> struct RValueBase<T &&> {
  using type = T &&;
};

export template <typename T> using RValue = typename RValueBase<T>::type;

template <typename T> struct RemoveRefBase {
  using type = T;
};

template <typename T> struct RemoveRefBase<T &> {
  using type = T;
};

template <typename T> struct RemoveRefBase<T &&> {
  using type = T;
};

export template <typename T> using RemoveRef = typename RemoveRefBase<T>::type;

template <typename T> struct RefBase {
  using type = T &;
};

template <typename T> struct RefBase<T &> {
  using type = T &;
};

template <typename T> struct RefBase<T &&> {
  using type = T &;
};

export template <typename T> using Ref = typename RefBase<T>::type;

export template <typename T> constexpr T &&forward(RemoveRef<T> &&arg) {
  return static_cast<T &&>(arg);
}
export template <typename T> constexpr T &&forward(RemoveRef<T> &arg) {
  return static_cast<T &&>(arg);
}

export template <typename T> constexpr RValue<T> move(T &&elt) {
  return static_cast<RValue<T>>(elt);
}

template <u64... sizes> struct TraitMaxBase;

template <u64 size> struct TraitMaxBase<size> {
  static constexpr u64 value = size;
};

template <u64 size, u64... sizes> struct TraitMaxBase<size, sizes...> {
  static constexpr u64 nextValue = TraitMaxBase<sizes...>::value;
  static constexpr u64 value = (size > nextValue) ? size : nextValue;
};

export template <u64... sizes>
constexpr u64 TraitMax = TraitMaxBase<sizes...>::value;

template <typename T> struct LoseConstBase {
  using type = T;
};

template <typename T> struct LoseConstBase<const T> {
  using type = T;
};

export template <typename T> using LoseConst = typename LoseConstBase<T>::type;

template <typename T> struct IsVoidBase {
  static constexpr bool value = false;
};

template <> struct IsVoidBase<void> {
  static constexpr bool value = true;
};

export template <typename T> constexpr bool IsVoid = IsVoidBase<T>::value;

} // namespace core