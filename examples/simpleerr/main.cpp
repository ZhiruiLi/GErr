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
#include <string>

using namespace std;

gerr::Error CheckArgumentValue(char const* arg) {
  try {
    std::stoi(arg);
  } catch (std::exception const& e) {
    return gerr::New("conv exception:{}", arg, e.what());
  }
  return nullptr;
}

gerr::Error CheckArguments(int argc, char const* argv[]) {
  if (argc != 2) {
    return gerr::New("argc({}) != 2", argc);  // 简单构建一个错误
  }
  auto const err = CheckArgumentValue(argv[1]);
  if (err != nullptr) {
    // 包装上层返回的错误
    return gerr::Wrap(err, "CheckArgumentValue(argv[1]({}))", argv[1]);
  }
  return nullptr;
}

int main(int argc, char const* argv[]) {
  auto const err = CheckArguments(argc, argv);
  if (err != nullptr) {
    // 直接通过 iostream 打印错误信息
    std::cerr << "Check arguments fail! " << *err
              << "\nExample: " << argv[0] << " <number>\n";
    // 使用 Code 函数获取错误链条上的第一个错误码
    // 如果整个错误链条上都没有错误码，就会返回默认的错误码 -1
    // 可以通过 Code 函数的第二个参数替换该错误码
    return gerr::Code(err);
  }
  std::cout << "Got argument: " << argv[1] << "\n";
  return 0;
}
