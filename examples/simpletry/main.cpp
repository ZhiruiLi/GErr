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
#include <vector>

#include "try.hpp"

// 安全的除法函数，返回一个 Try<int> 表示可能成功也可能失败的计算
mylib::Try<int> SafeDiv(int a, int b) {
  if (b == 0) {
    return gerr::New("div 0");
  }
  return a / b;
}

int main() {
  std::vector<std::pair<int, int>> cases{
      {0, 10},
      {10, 0},
      {10, 5},
  };
  for (auto const& c : cases) {
    std::cout << "Call SafeDiv on " << c.first << ", " << c.second << " ...\n";
    auto const t = SafeDiv(c.first, c.second);
    if (t) {
      std::cout << "No error, result = " << t.Value() << "\n";
    } else {
      std::cout << "Error occurs: " << gerr::String(t.Error()) << "\n";
    }
  }
}
