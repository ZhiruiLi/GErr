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
#include <random>
#include <utility>

namespace fake {

// 定义 error 类型，不带错误码
DEFINE_ERROR(ErrArgumentZero, "Argument is zero");
// 定义 error 类型，附带错误码
DEFINE_CODE_ERROR(ErrArgumentNeg, 1000001, "Argument is negative");

struct LERandErrorContext {
  int randNum1;
  int randNum2;
};
// 定义 error 类型，附带错误环境信息
DEFINE_CONTEXT_ERROR(ErrLERandNum1, LERandErrorContext,
                     "Random num is illegal, rand val1: {}, rand val2: {}",
                     context.randNum1, context.randNum2);

// 定义 error 类型，附带错误码和错误环境信息
DEFINE_CODE_CONTEXT_ERROR(ErrLERandNum2, 1000002, LERandErrorContext,
                          "random num is illegal, rand val1: {}, rand val2: {}",
                          context.randNum1, context.randNum2);

// 另一种定义复杂 error 类型的方法，通过继承 IError 来实现完全的自定义
// struct ErrLERandNumComplex : gerr::IError {
//   ErrLERandNum(int r, int r2) : ranNum1{r1}, randNum2{r2} {}
//   char const* Message() override { return "not greater than rand num"; }
//   int ranNum1{};
//   int ranNum2{};
// };

struct MyFakeAPI {
  static gerr::Error Call(int x) {
    if (x < 0) {
      return ErrArgumentNeg::E();
    }
    if (x == 0) {
      return ErrArgumentZero::E();
    }
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 3);
    auto const r1 = dist(rd);
    auto const r2 = dist(rd);
    if (x <= r1) {
      return ErrLERandNum1::E({r1, r2});
      // 如果使用 ErrLERandNumComplex 来进行定义，就需要使用 Make 函数
      // return gerr::Make<ErrLERandNumComplex>(r1, r2);
    }
    if (x <= r2) {
      return ErrLERandNum2::E({r1, r2});
    }
    return nullptr;
  }
};

}  // namespace fake
