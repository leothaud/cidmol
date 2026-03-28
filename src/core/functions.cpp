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
export module core:functions;

import :sharedPtr;
import :traits;

namespace core {

//! @cond INTERNAL

template <typename R, typename... Args> struct CallableBase {
  virtual ~CallableBase() = default;
  virtual R call(Args... args) = 0;
};

template <typename C, typename R, typename... Args>
class Callable : public CallableBase<R, Args...> {
  C callable;

public:
  Callable(C &&callable) : callable(callable) {}
  virtual R call(Args... args) override {
    return callable(forward<Args>(args)...);
  }
};

//! @endcond

//! Function-type wrapper to use it as arguments.
//! For example Function<int, char, char> to encode a function with return type
//! int and two char as arguments.
export template <typename R, typename... Args> class Function {
  SharedPtr<CallableBase<R, Args...>> fun;

public:
  ~Function() = default;
  Function() = default;

  template <typename F>
  Function(F &&callable)
      : fun(SharedPtr<CallableBase<R, Args...>>(
            static_cast<CallableBase<R, Args...> *>(
                new Callable<F, R, Args...>(forward<F>(callable))))) {}

  R operator()(Args... args) { return fun->call(forward<Args>(args)...); }

private:
  template <typename F, typename... ARGS> class InvokeResultBase {
    template <typename U>
    static auto test(U *p)
        -> decltype((*p)(declval<ARGS>()...), void(),
                    RemoveRef<decltype((*p)(declval<ARGS>()...))>{});

    template <typename> static void test(...);

  public:
    using type = decltype(test<F>(nullptr));
  };

public:
  template <typename F, typename... ARGS>
  using InvokeResult = typename InvokeResultBase<F, ARGS...>::type;
  template <typename F> Function<InvokeResult<F, R>, R> compose(F &&g) {
    return Function<InvokeResult<F, R>, R>(
        [&](Args... args) { return g(fun->call(forward<Args>(args)...)); });
  }
};

} // namespace core