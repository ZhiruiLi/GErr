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
#include <gerr/gerr.hpp>
#include <iostream>

#include "myapi.hpp"

int main() {
  for (int i = -1; i < 5; i++) {
    std::cout << "Handling " << i << ":\n";
    auto const err = fake::MyFakeAPI::Call(i);
    if (err != nullptr) {
      auto const rerr = gerr::As<fake::ErrLERandNum1>(err);
      if (rerr != nullptr) {
        // 将通用错误转换为特定错误
        auto const &ctx = rerr->Context();
        std::cout << "ErrLERandNum, rand val1:" << ctx.randNum1 << ";"
                  << ctx.randNum2 << "\n";
      } else if (gerr::Is<fake::ErrArgumentZero>(err)) {
        // 检查一个错误是否是特定的错误，by type
        std::cout << "I don't care arg zero error\n";
      } else if (gerr::IsCode(123, err)) {
        // 检查一个错误是否是特定的错误，by error code
        std::cout << "I don't care a dummy error";
      } else {
        // 简单格式化错误链条并打印错误
        std::cerr << *err << "\n";
      }
    } else {
      std::cout << "Nothing happened\n";
    }
  }
  return 0;
}
