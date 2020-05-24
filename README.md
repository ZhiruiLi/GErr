# GErr - 一个实现 golang 风格错误封装的 C++ 库

GErr 是一个简单易用的错误类型封装，用于取代原来的仅仅只使用错误码来传递错误的方式。
过去我们会给服务定义一些错误码，便于向调用方表达某种错误，但在实际使用中，经常为了避免错误路径上的
信息丢失，我们会每个出错的地方都打印日志，结果一个错误的出现可能会导致每一层调用都打印日志，仅为了
附加一些额外的错误信息。

GErr 参考了 Go 语言中的错误信息处理方式，提供了一种简单高效的错误表达方法，可以方便地定义错误，
包装错误，传递错误，将错误构筑成一个错误链条，确保环境信息的保留。

## Quick Start

需要先安装 CMake，并支持 C++11 的编译。

```bash
git clone --recurse-submodules https://github.com/ZhiruiLi/GErr.git && cd GErr && \
  mkdir build && cd build && \
  cmake .. && make
```

上述命令可以生成 3 个二进制文件，分别对应 examples 目录下的三个简单例子，可以简单执行一下：

```bash
./simpleerr 1 2
./simpleerr a
./simpleerr 1
./defineerr
./simpletry
```

## 返回一个最简单的错误

参考 [SimpleErr](https://www.github.com/zhiruili/GErr/tree/master/examples/simpleerr)，通过 gerr::New 来创建一个匿名的错误对象，可以携带错误信息和错误码。

```c++
gerr::Error MyFunction1(int i) {
    if (i == 0) {
        return gerr::New("i can't be 0"); // 非常轻量的形式，仅持有一个 C 风格字符串
    }
    if (i < 0) {
        return gerr::New("i = {} < 0 is illegal", i); // 允许使用 fmt 库的形式格式化字符串
    }
    int ret = api.Call(i);
    if (ret != 0) {
        return gerr::New(ret, "call api fail, i = {}", i); // 携带错误信息和错误码
    }
    return nullptr; // 返回 nullptr 代表没有错误发生
}
```

## 封装底层放回的错误

参考 [SimpleErr](https://www.github.com/zhiruili/GErr/tree/master/examples/simpleerr)，通过 gerr::Wrap 来创建一个匿名的错误对象，封装底层返回的错误，可以附带额外的错误信息和错误码。

```c++
gerr::Error MyFunction2(int i) {
    auto err = MyFunction1(i);
    if (err != nullptr) {
        return gerr::Wrap(err, "fail to call 1, i = {}", i); // 仅附加错误信息
    }
    err = MyFunction1(i + 1);
    if (err != nullptr) {
        return gerr::Wrap(err, 123456, "call fail, i = {}", i + 1); // 附加错误码和信息
    }
    err = MyFunction1(i + 1);
    if (err != nullptr) {
        return gerr::Wrap(err, 123456); // 仅附加一个错误码
    }
    return nullptr;
}
```

## 定义简单的错误类型

参考 [DefineErr](https://www.github.com/zhiruili/GErr/tree/master/examples/defineerr)，通过 `DEFINE_ERROR` 和 `DEFINE_CODE_ERROR` 宏来定义需要的错误类型，并可以附加错误码和错误信息。一个简单的错误类型非常轻量，全局只会存在一个该错误类型的对象，供所有地方使用。

```c++
// 定义一个错误类型 MyError1，附带错误信息
DEFINE_ERROR(MyError1, "my error 1");
// 定义一个错误类型 MyError2，附带错误码和错误信息
DEFINE_CODE_ERROR(MyError2, 1000001, "my error 2"); 

// 使用的时候需要用静态函数 E 来获取错误值
gerr::Error MyFunction3(int i, int j) {
    if (i < 0) { return MyError1::E(); }
    if (j < 0) { return MyError2::E(); }
    auto err = MyFunction1(i);
    if (err != nullptr) {
        // 自定义的错误类型也可以 wrap 父错误，同样是使用静态函数 E，
        // 但是此时就不是全局共享一个 MyError1 的错误对象了。
        return MyError1::E(err);
    }
    return nullptr;
}
```

## 定义携带上下文的错误类型

有时，我们除了需要错误携带环境信息，此时我们可以参考 [DefineErr](https://www.github.com/zhiruili/GErr/tree/master/examples/defineerr)，通过 `DEFINE_CONTEXT_ERROR` 和 `DEFINE_CODE_CONTEXT_ERROR` 宏来定义需要的错误类型，除了可以附加错误码和错误信息之外，还可以额外附加一个自定义的环境类型。此时，

```c++
struct MyContext { int i; int j; };
// 定义一个错误类型 MyError3，附带错误信息，携带一个 MyContext 环境
// 在格式化错误信息时，可以使用 `context` 变量获取到环境对象，并获取其中的成员
DEFINE_CONTEXT_ERROR(MyError3, MyContext, 
                     "my error 3, i {}, j {}", context.i, context.j);
// 定义一个错误类型 MyError4，附带错误码和错误信息，携带一个 MyContext 环境
// 在格式化错误信息时，可以使用 `context` 变量获取到环境对象，并获取其中的成员
DEFINE_CODE_CONTEXT_ERROR(MyError4, 1000002, MyContext,
                          "my error 4, i {}, j {}", context.i, context.j);

// 使用的时候，带环境的错误类型也需要用静态函数 E 来创建错误值，传入对应的环境变量
gerr::Error MyFunction4(int i, int j) {
    if (i < 0) { return MyError3::E({i, j}); }
    if (j < 0) { return MyError4::E({i, j}); }
    auto err = MyFunction1(i);
    if (err != nullptr) {
        // 带环境的自定义的错误类型也可以 wrap 父错误，同样是使用静态函数 E
        return MyError3::E(err, {i, j});
    }
    return nullptr;
}
```

## 定义复杂的自定义错误

大多数情况下，上述的四个宏都足以满足需求，如果有特殊的自定义需求（例如想给错误类型添加自己的成员函数），可以通过自定义的错误来进行实现。一个 gerr::Error 本质上是一个继承自 gerr::details::IError 的类型的共享指针，因此我们仅需要自定义一个继承自 gerr::details::IError 的类型：

```c++
struct MyComplexError : gerr::details::IError {
   MyComplexError(unsigned u)
     : uin{u}, message{fmt::format("fail with uin {}", uin)} {}
   MyComplexError(unsigned u, gerr::Error const &c)
     : uin{u}, message{fmt::format("fail with uin {}", uin)}, cause{c} {}

   // override Code 函数来提供错误码
   int Code() override { return 12345; };
   // override Message 函数来提供错误信息
   char const *Message() override { return message.c_str(); };
   // override Cause 函数来提供父错误
   gerr::Error Cause() override { return cause; };

   unsigned uin{};
   std::string message{};
   gerr::Error cause{};
}

// 使用的时候需要使用 gerr::Make 来构建错误
gerr::Error MyFunctionX(unsigned uin, int i) {
    auto err = MyFunction1(i);
    if (err != nullptr) {
        return gerr::Make<MyComplexError>(uin, err);
    }
    return nullptr;
}
```

## 判定错误类型和获取错误具体内容

参考 [DefineErr](https://www.github.com/zhiruili/GErr/tree/master/examples/defineerr)，由于上层可能需要判断底层返回错误的具体内容，并进行不同的处理，因此 GErr 提供如下几个函数：

* `gerr::Is` 用于判断一个错误链条上是否包含指定类型的错误
* `gerr::As` 尝试将错误转换为指定类型
* `gerr::IsCode` 用于判断一个错误链条上是否包含带有指定错误码的错误
* `gerr::AsCode` 尝试将错误转换为带有指定错误码的错误
* `gerr::Code` 尝试获取错误链条上的第一个错误码，如果获取不到，就返回默认的错误码

对于一般情况，我们可能只是需要简单地将错误信息输出一下，我们可以直接将 Error 打印到 `std::ostream` 中，
或者使用 `gerr::String` 将整个错误链条上的所有错误信息格式化成一个字符串。

```c++
int MyFunction5(unsigned uin, int i) {

    auto err = SomeOtherFunction(uin, i)

    // 判定一个错误链条上是不是包含 MyError1 类型的错误，
    // 这里不需要先判定 err 是否为空，Is 函数是 nullptr 安全的，
    // 如果一个 err 是 nullptr，那么 Is 函数总会返回 false。
    if (gerr::Is<MyError1>(err)) {
        std::cerr << "error 1";
        return 0;
    }

    // 尝试找到错误链条上的第一个 MyError3 类型的错误并返回，如果找不到，就返回 nullptr，
    // 这里不需要先判定 err 是否为空，As 函数是 nullptr 安全的，
    // 如果一个 err 是 nullptr，那么 As 函数总会返回 nullptr。
    auto err3 = gerr::As<MyError3>(err);
    if (cpxErr != nullptr) {
        // 通过 Context 函数可以获取到 error 里面的环境信息。
        auto const &ctx = err3->Context();
        std::cerr << "error complex, i: " << ctx->i << ", j: " << ctx->j; 
        return 0;
    }

    // 其他未知错误，直接打印一下
    if (err != nullptr) {
        // String 函数也是 nullptr 安全的，nullptr 会被格式化为 <nil>
        // std::cerr << gerr::String(err);
        std::cerr << *err;
    }
    // Code 函数也是 nullptr 安全的，传入 nullptr 会返回 0
    // 对于非 nullptr 的 err 而整个错误链条上都没有非 0 错误码的场合
    // 会返回默认的错误码 -1，这个行为可以通过 Code 的第二个参数指定
    return gerr::Code(err);
}
```

## 表达一个可能成功可能出错的返回值

在 C++ 中，我们的函数经常返回一个错误码，然后其他需要返回的值通过指针参数来向外传递，或是会返回一个 `std::tuple` 来表述多返回值。这样的情况 GErr 能够很好地替代，但是，有时候我们也会希望实现类似 Rust 的 [`Result`](https://doc.rust-lang.org/std/result/enum.Result.html) 类型或是 Scala 的 [`Try`](https://www.scala-lang.org/api/current/scala/util/Try.html) 类型，用来表示一个可能成功可能失败的返回值。基于 GErr 可以很容易实现类似的效果，在 examples 中简单实现了一个非常简易的 `Try` 模板，参考 [SimpleTry](https://www.github.com/zhiruili/GErr/tree/master/examples/simpletry)。
