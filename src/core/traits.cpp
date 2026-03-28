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

import :builtins;

export template <typename T> T &&declval() noexcept;

namespace core {

//! @cond INTERNAL
template <bool B, typename T = void> struct EnableIfBase {};

template <typename T> struct EnableIfBase<true, T> {
  using type = T;
};
//! @endcond

//! Use in SFINAE. The type exists if `B` is true.
export template <bool B, typename T = void>
using enableIf = typename EnableIfBase<B, T>::type;

//! @cond INTERNAL
template <typename T1, typename T2> struct IsSameBase {
  static constexpr bool value = false;
};

template <typename T> struct IsSameBase<T, T> {
  static constexpr bool value = true;
};
//! @endcond

//! Is true iff `T1` and `T2` represents the same type.
export template <typename T1, typename T2>
constexpr bool isSame = IsSameBase<T1, T2>::value;

//! @cond INTERNAL
template <typename T> struct RValueBase {
  using type = T &&;
};

template <typename T> struct RValueBase<T &> {
  using type = T &&;
};

template <typename T> struct RValueBase<T &&> {
  using type = T &&;
};
//! @endcond

//! Type representing an rvalue of type `T`.
export template <typename T> using RValue = typename RValueBase<T>::type;

//! @cond INTERNAL
template <typename T> struct RemoveRefBase {
  using type = T;
};

template <typename T> struct RemoveRefBase<T &> {
  using type = T;
};

template <typename T> struct RemoveRefBase<T &&> {
  using type = T;
};
//! @endcond

//! Type representing `T` not as reference.
export template <typename T> using RemoveRef = typename RemoveRefBase<T>::type;

//! @cond INTERNAL
template <typename T> struct RefBase {
  using type = T &;
};

template <typename T> struct RefBase<T &> {
  using type = T &;
};

template <typename T> struct RefBase<T &&> {
  using type = T &;
};
//! @endcond

//! Type representing a reference of type `T`.
export template <typename T> using Ref = typename RefBase<T>::type;

//! Forward `arg` as is.
export template <typename T> constexpr T &&forward(RemoveRef<T> &&arg) {
  return static_cast<T &&>(arg);
}
//! Forward `arg` as is.
export template <typename T> constexpr T &&forward(RemoveRef<T> &arg) {
  return static_cast<T &&>(arg);
}

//! Move `elt` by rvalue-reference to give its ownership.
export template <typename T> constexpr RValue<T> move(T &&elt) {
  return static_cast<RValue<T>>(elt);
}

//! @cond INTERNAL
template <u64... sizes> struct TraitMaxBase;

template <u64 size> struct TraitMaxBase<size> {
  static constexpr u64 value = size;
};

template <u64 size, u64... sizes> struct TraitMaxBase<size, sizes...> {
  static constexpr u64 nextValue = TraitMaxBase<sizes...>::value;
  static constexpr u64 value = (size > nextValue) ? size : nextValue;
};
//! @endcond

//! Compute the max of `sizes...` at compile time.
export template <u64... sizes>
constexpr u64 TraitMax = TraitMaxBase<sizes...>::value;

//! @cond INTERNAL
template <typename T> struct LoseConstBase {
  using type = T;
};

template <typename T> struct LoseConstBase<const T> {
  using type = T;
};
//! @endcond

//! Represent type `T` but is never const.
export template <typename T> using LoseConst = typename LoseConstBase<T>::type;

//! @cond INTERNAL
template <typename T> struct IsVoidBase {
  static constexpr bool value = false;
};

template <> struct IsVoidBase<void> {
  static constexpr bool value = true;
};
//! @endcond

//! Is true iff `T` is the void type.
export template <typename T> constexpr bool IsVoid = IsVoidBase<T>::value;

//! Contains a sequence of index as template parameter.
export template <u64... Index> struct IndicesSequence {};

//! @cond INTERNAL
template <u64 N, u64... Indices>
struct IndicesSequenceBuilderBase
    : IndicesSequenceBuilderBase<N - 1, N - 1, Indices...> {};
template <u64... I> struct IndicesSequenceBuilderBase<0, I...> {
  using type = IndicesSequence<I...>;
};
//! @endcond

//! Construct an `IndicesSequence` with index `0..I`.
export template <u64... I>
using IndicesSequenceBuilder = IndicesSequenceBuilderBase<I...>::type;

//! @cond INTERNAL
template <typename F, typename... ARGS> struct IsCallableBase {
  template <typename U>
  static char test(decltype((*((U *)0))(declval<ARGS>()...)) *);

  template <typename> static int test(...);

  static constexpr bool value = sizeof(decltype(test<F>(0))) == sizeof(char);
};
//! @endcond

//! Is true iff a function of type `F` can be called with
// arguments of types `ARGS...`.
export template <typename F, typename... ARGS>
constexpr bool IsCallable = IsCallableBase<F, ARGS...>::value;

template <typename T, typename... Args> struct IsConstructibleBase {

  template <typename U, typename... A>
  static char test(decltype(U(declval<A>()...)) *);

  template <typename> static int test(...);

  static constexpr bool value =
      sizeof(decltype(test<T, Args...>)) == sizeof(char);
};

export template <typename T, typename... Args>
using IsConstructible = IsConstructibleBase<T, Args...>::value;

export template <auto T, auto... Ts> struct FirstTemplateBase {
  static constexpr auto value = T;
};

export template <auto T, auto... Ts>
inline constexpr auto FirstTemplate = FirstTemplateBase<T, Ts...>::value;

} // namespace core