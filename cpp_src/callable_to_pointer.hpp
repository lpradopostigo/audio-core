/*
 * Credit to https://gist.github.com/isc30/fab67e5956fe8f2097bed84ebc42c1e8
 * */
#pragma once

#include <type_traits>
#include <functional>

template<typename TCallable, typename TSignature>
constexpr bool is_callable_as_v = std::is_constructible<std::function<TSignature>, TCallable>::value;

template<
    typename TCallable,
    typename TSignature,
    typename = std::enable_if_t<is_callable_as_v<TCallable, TSignature>>>
using callable_t [[maybe_unused]] = TCallable;

template<typename TCallable>
struct callable_metadata
    : callable_metadata<decltype(&TCallable::operator())> {
};

template<class TClass, typename TReturn, typename... TArgs>
struct callable_metadata<TReturn(TClass::*)(TArgs...)> {
  using class_t [[maybe_unused]] = TClass;
  using return_t [[maybe_unused]] = TReturn;
  using args_tuple_t [[maybe_unused]] = std::tuple<TArgs...>;
  using ptr_t = TReturn(*)(TArgs...);

  [[maybe_unused]] static ptr_t generate_pointer(const TClass &closure) {
    static TClass closure_copy = closure;

    return [](TArgs... args) {
      return closure_copy(args...);
    };
  }
};

template<class TClass, typename TReturn, typename... TArgs>
struct callable_metadata<TReturn(TClass::*)(TArgs...) const> {
  using class_t [[maybe_unused]] = TClass;
  using return_t [[maybe_unused]] = TReturn;
  using args_tuple_t [[maybe_unused]] = std::tuple<TArgs...>;
  using ptr_t = TReturn(*)(TArgs...);

  [[maybe_unused]] static ptr_t generate_pointer(const TClass &closure) {
    static TClass closure_copy = closure;

    return [](TArgs... args) {
      return closure_copy(args...);
    };
  }
};

template<typename TCallable>
[[maybe_unused]] auto *callable_to_pointer(const TCallable &callable) {
  return callable_metadata<TCallable>::generate_pointer(callable);
}