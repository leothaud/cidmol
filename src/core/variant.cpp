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
export module core:variant;

import :start;
import :traits;
import :types;

namespace core {

template <typename, typename...> struct IndexOfBase;

template <typename Elt, typename... Ts> struct IndexOfBase<Elt, Elt, Ts...> {
  static constexpr u64 value = 0;
};

template <typename Elt, typename T, typename... Ts>
struct IndexOfBase<Elt, T, Ts...> {
  static constexpr u64 value = 1 + IndexOfBase<Elt, Ts...>::value;
};

template <typename Elt, typename... Ts>
static constexpr u64 IndexOf = IndexOfBase<Elt, Ts...>::value;

template <unsigned, typename = void, typename...> struct GetTypeBase;
template <typename H, typename... T> struct GetTypeBase<0, void, H, T...> {
  using type = H;
};

template <unsigned N, typename H, typename... T>
struct GetTypeBase<N, enableIf<N != 0>, H, T...> {
  using type = typename GetTypeBase<N - 1, void, T...>::type;
};

template <unsigned N, typename... T>
using GetType = typename GetTypeBase<N, T...>::type;

export template <typename... Ts> class Variant;

export template <typename T> class Variant<T> {
protected:
  T elt;

public:
  ~Variant() = default;
  Variant() = default;
  Variant(const Variant &other) : elt(other.elt) {}
  Variant(Variant &&other) : elt(core::move(other.elt)) {}
  Variant &operator=(const Variant &other) {
    elt = other.elt;
    return *this;
  }
  Variant &operator=(Variant &&other) {
    elt = core::move(other.elt);
    return *this;
  }

  Variant(const T &elt) : elt(elt) {}
  Variant(T &&elt) : elt(core::move(elt)) {}

  template <typename T2>
  void set(const T2 &elt, enableIf<isSame<T, T2>, int> = 0) {
    this->elt = elt;
  }
  template <typename T2> void set(T &&elt, enableIf<isSame<T, T2>, int> = 0) {
    this->elt = core::move(elt);
  }

  template <typename T2> bool isA(enableIf<isSame<T, T2>, int> = 0) {
    return true;
  }

  template <typename T2> T2 &get(enableIf<isSame<T, T2>, int> = 0) {
    return elt;
  }

  template <typename T2> T2 get(enableIf<isSame<T, T2>, int> = 0) const {
    return elt;
  }

  template <typename F> auto visit(F &&f) { return f(elt); }
};

export template <typename... Ts> class Variant {
protected:
  static constexpr u64 maxSize = TraitMax<sizeof(Ts)...>;
  static constexpr u64 maxAlign = TraitMax<alignof(Ts)...>;
  alignas(maxAlign) char value[maxSize];
  int index;

public:
  ~Variant() { finalize(); }

  Variant() : index(-1) {}
  Variant(const Variant &other) : index(other.index) {
    if (index != -1) {
      using t = GetType<index, void, Ts...>;
      initialize<t>();
      *reinterpret_cast<t *>(value) = *reinterpret_cast<t *>(other.value);
    }
  }
  Variant &operator=(const Variant &other) {
    finalize();
    index = other.index;
    if (index != -1) {
      using t = GetType<index, void, Ts...>;
      initialize<t>();
      *reinterpret_cast<t *>(value) = *reinterpret_cast<t *>(other.value);
    }
    return *this;
  }

  Variant(Variant &&other) : index(other.index) {
    if (index != -1) {
      using t = GetType<index, void, Ts...>;
      initialize<t>();
      *reinterpret_cast<t *>(value) =
          core::move(*reinterpret_cast<t *>(other.value));
    }
  }

  template <int idx> void initValue(int index, Variant &other) {
    if (idx == index) {
      using t = GetType<idx, void, Ts...>;
      initialize<t>();
      *reinterpret_cast<t *>(value) =
          core::move(*reinterpret_cast<t *>(other.value));
    } else if constexpr (idx < sizeof...(Ts) - 1) {
      initValue<idx + 1>(index, other);
    }
  }

  Variant &operator=(Variant &&other) {
    finalize();
    index = other.index;
    if (index != -1) {
      initValue<0>(index, other);
    }
    return *this;
  }

  template <typename T>
  Variant(T &&elt, enableIf<(isSame<T, Ts> || ...), int> = 0) : Variant() {
    set<T>(core::move(elt));
  }
  template <typename T>
  Variant(const T &elt, enableIf<(isSame<T, Ts> || ...), int> = 0) : Variant() {
    set<T>(elt);
  }

  template <typename T>
  void set(T &&elt, enableIf<(isSame<T, Ts> || ...), int> = 0) {
    finalize();
    constexpr u64 index = IndexOf<T, Ts...>;
    if (index != -1) {
      using t = GetType<index, void, Ts...>;
      if (index != this->index)
        initialize<t>();
      *reinterpret_cast<t *>(value) = move(elt);
      this->index = index;
    }
  }

  template <typename T>
  void set(const T &elt, enableIf<(isSame<T, Ts> || ...), int> = 0) {
    finalize();
    constexpr u64 index = IndexOf<T, Ts...>;
    if (index != -1) {
      using t = GetType<index, void, Ts...>;
      if (index != this->index)
        initialize<t>();
      if (this->index != index) {
      }
      *reinterpret_cast<t *>(value) = elt;
      this->index = index;
    }
  }

  template <typename T>
  bool isA(enableIf<(isSame<LoseConst<T>, Ts> || ...), int> = 0) {
    return index == IndexOf<T, Ts...>;
  }

  template <typename T>
  T &get(enableIf<(isSame<LoseConst<T>, Ts> || ...), int> = 0) {
    constexpr u64 index = IndexOf<LoseConst<T>, Ts...>;
    if (index == this->index) {
      return *reinterpret_cast<T *>(value);
    }
    __builtin_trap();
  }

  template <typename T>
  T &get(enableIf<(isSame<LoseConst<T>, Ts> || ...), int> = 0) const {
    constexpr u64 index = IndexOf<LoseConst<T>, Ts...>;
    if (index == this->index) {
      return *reinterpret_cast<T *>(value);
    }
    __builtin_trap();
  }

  template <unsigned N, typename F>
  decltype(auto) visitImpl(F &&f, enableIf<(N < sizeof...(Ts)), int> = 0) {
    if (N == index) {
      using t = GetType<N, void, Ts...>;
      return f(*reinterpret_cast<t *>(value));
    }
    if constexpr (N == 0) {
      __builtin_trap();
    } else {
      return visitImpl<N - 1, F>(f);
    }
  }

  template <unsigned N, typename F>
  decltype(auto) visitImpl(F &&f,
                           enableIf<(N < sizeof...(Ts)), int> = 0) const {
    if (N == index) {
      using t = GetType<N, void, Ts...>;
      return f(*reinterpret_cast<const t *>(value));
    }
    if constexpr (N == 0) {
      __builtin_trap();
    } else {
      return visitImpl<N - 1, F>(f);
    }
  }

  template <typename F> decltype(auto) visit(F &&f) {
    return visitImpl<sizeof...(Ts) - 1>(f);
  }

  template <typename F> decltype(auto) visit(F &&f) const {
    return visitImpl<sizeof...(Ts) - 1>(f);
  }

  template <unsigned N>
  void finalizeImpl(enableIf<(N < sizeof...(Ts)), int> = 0) {
    if (N == index) {
      using t = GetType<N, void, Ts...>;
      (*reinterpret_cast<t *>(value)).~t();
    } else {
      if constexpr (N != 0) {
        finalizeImpl<N - 1>();
      }
    }
  }

  void finalize() { finalizeImpl<sizeof...(Ts) - 1>(); }

  template <typename T, typename = enableIf<
                            (isSame<LoseConst<RemoveRef<T>>, Ts> || ...), void>>
  void initialize() {
    new (value) T();
  }
};

} // namespace core