// MIT License
//
// Copyright (c) 2020 ZhiruiLi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#pragma once

#include <gerr/gerr.hpp>
#include <type_traits>
#include <utility>

namespace mylib {

/*
Try 类型是对 gerr::Error 类型的简单封装，用于处理一个可能出错的返回值。
目前的实现仅仅是一个 std::pair<ValueType, gerr::Error> 而已，在 C++ 17
中，我们可以通过 structure binding 的特性获得更好的编程体验，这个 Try
就没有什么意义了，但是在 C++ 11 中，我们没有这个能力，
因此这里提供这样的一个封装。

例如一个函数可能返回一个 int
类型的值，也有可能出错，那么这个返回类型可以表达为：Try<int>：

```c++
Try<double> SafeDiv(int i, int j) {
    if (j == 0) {
        return gerr::New("can't div 0"); // Error 类型可以被隐式转换为 Try
    }
    return (double)i / (double)j; // 合法值也可以被隐式转换为 Try
}
```

对于使用者，则需要在调用后判定是否出错：

```c++
Try<double> SafeReciprocal(int i) {
    auto tryDiv = SafeDiv(1, i);
    // 使用 IsSuccess 或 IsFailure 判定是否出错，
    // 如果 IsSuccess，则 Try 可被隐式转换为 true，
    // 否则，Try 可以被隐式转化为 false。
    if (!tryDiv) {
        return gerr::Wrap(tryDiv.Error(), "1/{}", i); // 包装 error 上抛
    }
    return tryDiv; // 如果正确就直接返回
}

void EchoReciprocal(int i) {
    auto tryRec = SafeReciprocal(i);
    if (!tryRec) {
        // 出错的时候，通过 Error 函数获得内部的错误
        std::cerr << gerr::String(tryRec.Error());
    } else {
        // 正确的时候，通过 Value 函数获得内部的值
        std::cout << tryRec.Value();
    }
}
```
*/
template <class ValueType>
class Try : public std::pair<ValueType, ::gerr::Error> {
 public:
  static_assert(!std::is_reference<ValueType>::value,
                "Try may not be used with reference types");
  static_assert(!std::is_abstract<ValueType>::value,
                "Try may not be used with abstract types");

  Try(::gerr::Error&& err)
      : std::pair<ValueType, ::gerr::Error>{{}, std::move(err)} {}

  Try(::gerr::Error const& err)
      : std::pair<ValueType, ::gerr::Error>{{}, err} {}

  template <class ErrType, class = typename std::enable_if<std::is_base_of<
                               ::gerr::details::IError, ErrType>::value>::type>
  Try(std::shared_ptr<ErrType>&& err)
      : std::pair<ValueType, ::gerr::Error>{
            {},
            std::move(
                std::static_pointer_cast<::gerr::details::IError>(err))} {}

  template <class ErrType, class = typename std::enable_if<std::is_base_of<
                               ::gerr::details::IError, ErrType>::value>::type>
  Try(std::shared_ptr<ErrType> const& err)
      : std::pair<ValueType, ::gerr::Error>{
            {}, std::static_pointer_cast<::gerr::details::IError>(err)} {}

  Try(ValueType const& val) noexcept(
      std::is_nothrow_copy_constructible<ValueType>::value)
      : std::pair<ValueType, ::gerr::Error>{val, nullptr} {}

  Try(ValueType&& val) noexcept(
      std::is_nothrow_move_constructible<ValueType>::value)
      : std::pair<ValueType, ::gerr::Error>{std::move(val), nullptr} {}

  Try(Try&& src) noexcept(std::is_nothrow_move_constructible<ValueType>::value)
      : std::pair<ValueType, ::gerr::Error>{std::move(src.first),
                                            std::move(src.second)} {}

  Try(Try& src) noexcept(std::is_nothrow_copy_constructible<ValueType>::value)
      : std::pair<ValueType, ::gerr::Error>{src.first, src.second} {}

  explicit operator bool() const noexcept { return IsSuccess(); }

  bool IsSuccess() const { return this->second == nullptr; }
  bool IsFailure() const { return this->second != nullptr; }

  ValueType const& Value() const { return this->first; }
  ValueType& Value() { return this->first; }

  ::gerr::Error const& Error() const { return this->second; }
  ::gerr::Error& Error() { return this->second; }

  void ClearValue() { this->first = ValueType{}; }
  void ClearError() { this->second = nullptr; }

  template <class ErrType, class = typename std::enable_if<std::is_base_of<
                               ::gerr::details::IError, ErrType>::value>::type>
  void Assign(std::shared_ptr<ErrType>&& err) {
    if (IsSuccess()) {
      ClearValue();
    }
    this->second = std::move(err);
  }

  template <class ErrType, class = typename std::enable_if<std::is_base_of<
                               ::gerr::details::IError, ErrType>::value>::type>
  void Assign(std::shared_ptr<ErrType> const& err) {
    if (IsSuccess()) {
      ClearValue();
    }
    this->second = err;
  }

  void Assign(::gerr::Error&& err) {
    if (IsSuccess()) {
      ClearValue();
    }
    this->second = std::move(err);
  }

  void Assign(::gerr::Error const& err) {
    if (IsSuccess()) {
      ClearValue();
    }
    this->second = err;
  }

  void Assign(Try&& src) {
    this->first = std::move(src.first);
    this->second = std::move(src.second);
  }

  void Assign(Try const& src) {
    this->first = src.first;
    this->second = src.second;
  }

  void Assign(ValueType&& newValue) {
    if (IsFailure()) {
      ClearError();
    }
    this->first = std::move(newValue);
  }

  void Assign(ValueType const& newValue) {
    if (IsFailure()) {
      ClearError();
    }
    this->first = newValue;
  }

  template <class ErrType, class = typename std::enable_if<std::is_base_of<
                               ::gerr::details::IError, ErrType>::value>::type>
  Try& operator=(std::shared_ptr<ErrType> const& err) noexcept {
    Assign(err);
    return *this;
  }

  template <class ErrType, class = typename std::enable_if<std::is_base_of<
                               ::gerr::details::IError, ErrType>::value>::type>
  Try& operator=(std::shared_ptr<ErrType>&& err) noexcept {
    Assign(std::move(err));
    return *this;
  }

  Try& operator=(::gerr::Error const& err) noexcept {
    Assign(err);
    return *this;
  }

  Try& operator=(::gerr::Error&& err) noexcept {
    Assign(std::move(err));
    return *this;
  }

  template <class Arg>
  Try& operator=(Arg&& arg) {
    Assign(std::forward<Arg>(arg));
    return *this;
  }

  Try& operator=(Try&& other) noexcept(
      std::is_nothrow_move_assignable<ValueType>::value) {
    Assign(std::move(other));
    return *this;
  }

  Try& operator=(Try const& other) noexcept(
      std::is_nothrow_copy_assignable<ValueType>::value) {
    Assign(other);
    return *this;
  }
};

template <class T>
void swap(Try<T>& a, Try<T>& b) noexcept(noexcept(a.swap(b))) {
  a.swap(b);
}

/** 辅助函数，类似 make_unique 之类的 std 函数 */
template <class T>
constexpr Try<typename std::decay<T>::type> MakeTry(T&& v) {
  return {std::forward<T>(v)};
}

template <class T, class... Args>
constexpr Try<T> MakeTry(Args&&... args) {
  return {std::forward<Args>(args)...};
}

template <class T, class U, class... Args>
constexpr Try<T> MakeTry(std::initializer_list<U> il, Args&&... args) {
  return {il, std::forward<Args>(args)...};
}

}  // namespace mylib
