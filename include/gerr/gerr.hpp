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

#include <fmt/format.h>

#include <memory>
#include <string>
#include <type_traits>

/**
 * 定义一个自定义的 gerr::Error 类型，并附加一个错误码和错误信息。
 *
 * Example:
 *   // 不带错误码的 error 类型
 *   DEFINE_ERROR(MyError1, "my error message 1");
 *   // 带错误码的 error 类型
 *   DEFINE_CODE_ERROR(MyError2, 123456, "my error message 2");
 *
 *   struct MyCtxType { int i; int j; };
 *   // 带具体环境信息的 error 类型
 *   DEFINE_CONTEXT_ERROR(MyError3, MyCtxType,
 *     "my error message 3, {}, {}", context.i, context.j);
 *   // 带错误码和具体环境信息的 error 类型
 *   DEFINE_CODE_CONTEXT_ERROR(MyError4, 234567, MyCtxType,
 *     "my error message 4, {}, {}", context.i, context.j);
 *
 * 在使用的地方，需要使用其静态函数 E：
 *   gerr::Error SomeFunction(int i, int j) {
 *       if (i < 0) {
 *            return MyError1::E();
 *       } else if (i == 0) {
 *            return MyError2::E();
 *       }
 *       auto err = SomeFunction2();
 *       if (err != nullptr) {
 *           return MyError1::E(err); // 包装一层向上传递
 *       }
 *       int ret1 = FooAPI();
 *       int ret2 = BarAPI();
 *       if (ret1 != 0) {
 *           return MyError3::E({ret1, ret2});
 *       }
 *       if (ret2 != 0) {
 *           return MyError4::E({ret1, ret2});
 *       }
 *       return nullptr; // 没有错误
 *   }
 */
#define DEFINE_ERROR(__ErrTypE__, __ErrMessagE__)                             \
  class __ErrTypE__ : public ::gerr::details::IError {                        \
   protected:                                                                 \
    struct __PrivateStruct__ {};                                              \
                                                                              \
   public:                                                                    \
    __ErrTypE__(__PrivateStruct__ const&) {}                                  \
    __ErrTypE__(::gerr::Error __c__, __PrivateStruct__ const&)                \
        : __causE__{::std::move(__c__)} {}                                    \
                                                                              \
    static ::gerr::Error E() {                                                \
      static auto __valuE__ = ::gerr::Make<__ErrTypE__>(__PrivateStruct__{}); \
      return __valuE__;                                                       \
    }                                                                         \
                                                                              \
    template <class ErrType,                                                  \
              class = typename ::std::enable_if<                              \
                  ::std::is_base_of<IError, ErrType>::value>::type>           \
    static ::gerr::Error E(::std::shared_ptr<ErrType>&& __p__) {              \
      return ::gerr::Make<__ErrTypE__>(::std::move(__p__),                    \
                                       __PrivateStruct__{});                  \
    }                                                                         \
                                                                              \
    template <class ErrType,                                                  \
              class = typename ::std::enable_if<                              \
                  ::std::is_base_of<IError, ErrType>::value>::type>           \
    static ::gerr::Error E(::std::shared_ptr<ErrType> const& __p__) {         \
      return ::gerr::Make<__ErrTypE__>(__p__, __PrivateStruct__{});           \
    }                                                                         \
                                                                              \
    char const* Message() override { return __ErrMessagE__; }                 \
    ::gerr::Error Cause() override { return __causE__; }                      \
                                                                              \
   private:                                                                   \
    ::gerr::Error __causE__{};                                                \
  }

#define DEFINE_CODE_ERROR(__ErrTypE__, __ErrCodE__, __ErrMessagE__)            \
  class __ErrTypE__ : public ::gerr::details::IError {                         \
   protected:                                                                  \
    struct __PrivateStruct__ {};                                               \
                                                                               \
   public:                                                                     \
    __ErrTypE__(__PrivateStruct__ const&) {}                                   \
    __ErrTypE__(::gerr::Error&& __c__, __PrivateStruct__ const&)               \
        : __causE__{::std::move(__c__)} {}                                     \
    __ErrTypE__(::gerr::Error const& __c__, __PrivateStruct__ const&)          \
        : __causE__{__c__} {}                                                  \
                                                                               \
    static ::gerr::Error E() {                                                 \
      static auto __valuE__ = ::gerr::Make<__ErrTypE__>(__PrivateStruct__{});  \
      return __valuE__;                                                        \
    }                                                                          \
                                                                               \
    template <class ErrType,                                                   \
              class = typename ::std::enable_if<                               \
                  ::std::is_base_of<IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType>&& __p__) {               \
      return ::gerr::Make<__ErrTypE__>(std::move(__p__), __PrivateStruct__{}); \
    }                                                                          \
                                                                               \
    template <class ErrType,                                                   \
              class = typename ::std::enable_if<                               \
                  ::std::is_base_of<IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType> const& __p__) {          \
      return ::gerr::Make<__ErrTypE__>(__p__, __PrivateStruct__{});            \
    }                                                                          \
                                                                               \
    int Code() override { return __ErrCodE__; }                                \
    char const* Message() override { return __ErrMessagE__; }                  \
    ::gerr::Error Cause() override { return __causE__; }                       \
                                                                               \
   private:                                                                    \
    ::gerr::Error __causE__{};                                                 \
  }

#define DEFINE_CONTEXT_ERROR(__ErrTypE__, __ContextTypE__, __ErrFormaT__, ...) \
  class __ErrTypE__ : public ::gerr::details::IError {                         \
   protected:                                                                  \
    struct __PrivateStruct__ {};                                               \
                                                                               \
   public:                                                                     \
    using ContextType = __ContextTypE__;                                       \
                                                                               \
    __ErrTypE__(ContextType const& __ctX__, std::string&& __msG__,             \
                __PrivateStruct__ const&)                                      \
        : __contexT__{__ctX__}, __messagE__{std::move(__msG__)} {}             \
                                                                               \
    __ErrTypE__(ContextType&& __ctX__, std::string&& __msG__,                  \
                __PrivateStruct__ const&)                                      \
        : __contexT__{std::move(__ctX__)}, __messagE__{std::move(__msG__)} {}  \
                                                                               \
    __ErrTypE__(::gerr::Error&& __c__, ContextType const& __ctX__,             \
                std::string&& __msG__, __PrivateStruct__ const&)               \
        : __causE__{::std::move(__c__)},                                       \
          __contexT__{__ctX__},                                                \
          __messagE__{std::move(__msG__)} {}                                   \
                                                                               \
    __ErrTypE__(::gerr::Error const& __c__, ContextType const& __ctX__,        \
                std::string&& __msG__, __PrivateStruct__ const&)               \
        : __causE__{__c__},                                                    \
          __contexT__{__ctX__},                                                \
          __messagE__{std::move(__msG__)} {}                                   \
                                                                               \
    __ErrTypE__(::gerr::Error&& __c__, ContextType&& __ctX__,                  \
                std::string&& __msG__, __PrivateStruct__ const&)               \
        : __causE__{::std::move(__c__)},                                       \
          __contexT__{std::move(__ctX__)},                                     \
          __messagE__{std::move(__msG__)} {}                                   \
                                                                               \
    __ErrTypE__(::gerr::Error const& __c__, ContextType&& __ctX__,             \
                std::string&& __msG__, __PrivateStruct__ const&)               \
        : __causE__{__c__},                                                    \
          __contexT__{std::move(__ctX__)},                                     \
          __messagE__{std::move(__msG__)} {}                                   \
                                                                               \
    static ::gerr::Error E(ContextType const& context) {                       \
      return ::gerr::Make<__ErrTypE__>(                                        \
          context, fmt::format(__ErrFormaT__, __VA_ARGS__),                    \
          __PrivateStruct__{});                                                \
    }                                                                          \
                                                                               \
    static ::gerr::Error E(ContextType&& context) {                            \
      auto __tempMsG__ = fmt::format(__ErrFormaT__, __VA_ARGS__);              \
      return ::gerr::Make<__ErrTypE__>(context, std::move(__tempMsG__),        \
                                       __PrivateStruct__{});                   \
    }                                                                          \
                                                                               \
    template <class ErrType,                                                   \
              class = typename ::std::enable_if<                               \
                  ::std::is_base_of<IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType>&& __p__,                 \
                           ContextType const& context) {                       \
      return ::gerr::Make<__ErrTypE__>(                                        \
          ::std::move(__p__), fmt::format(__ErrFormaT__, __VA_ARGS__),         \
          __PrivateStruct__{});                                                \
    }                                                                          \
                                                                               \
    template <class ErrType,                                                   \
              class = typename ::std::enable_if<                               \
                  ::std::is_base_of<IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType> const& __p__,            \
                           ContextType const& context) {                       \
      return ::gerr::Make<__ErrTypE__>(                                        \
          __p__, fmt::format(__ErrFormaT__, __VA_ARGS__),                      \
          __PrivateStruct__{});                                                \
    }                                                                          \
                                                                               \
    template <class ErrType,                                                   \
              class = typename ::std::enable_if<                               \
                  ::std::is_base_of<IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType>&& __p__,                 \
                           ContextType&& context) {                            \
      auto __tempMsG__ = fmt::format(__ErrFormaT__, __VA_ARGS__);              \
      return ::gerr::Make<__ErrTypE__>(                                        \
          ::std::move(__p__), std::move(__tempMsG__), __PrivateStruct__{});    \
    }                                                                          \
                                                                               \
    template <class ErrType,                                                   \
              class = typename ::std::enable_if<                               \
                  ::std::is_base_of<IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType> const& __p__,            \
                           ContextType&& context) {                            \
      auto __tempMsG__ = fmt::format(__ErrFormaT__, __VA_ARGS__);              \
      return ::gerr::Make<__ErrTypE__>(__p__, std::move(__tempMsG__),          \
                                       __PrivateStruct__{});                   \
    }                                                                          \
                                                                               \
    char const* Message() override { return __messagE__.c_str(); }             \
    ::gerr::Error Cause() override { return __causE__; }                       \
    ContextType& Context() { return __contexT__; }                             \
                                                                               \
   private:                                                                    \
    ::gerr::Error __causE__{};                                                 \
    ContextType __contexT__{};                                                 \
    ::std::string __messagE__{};                                               \
  }

#define DEFINE_CODE_CONTEXT_ERROR(__ErrTypE__, __ErrCodE__, __ContextTypE__,  \
                                  __ErrFormaT__, ...)                         \
  class __ErrTypE__ : public ::gerr::details::IError {                        \
   protected:                                                                 \
    struct __PrivateStruct__ {};                                              \
                                                                              \
   public:                                                                    \
    using ContextType = __ContextTypE__;                                      \
                                                                              \
    __ErrTypE__(ContextType const& __ctX__, std::string&& __msG__,            \
                __PrivateStruct__ const&)                                     \
        : __contexT__{__ctX__}, __messagE__{std::move(__msG__)} {}            \
                                                                              \
    __ErrTypE__(ContextType&& __ctX__, std::string&& __msG__,                 \
                __PrivateStruct__ const&)                                     \
        : __contexT__{std::move(__ctX__)}, __messagE__{std::move(__msG__)} {} \
                                                                              \
    __ErrTypE__(::gerr::Error&& __c__, ContextType const& __ctX__,            \
                std::string&& __msG__, __PrivateStruct__ const&)              \
        : __causE__{::std::move(__c__)},                                      \
          __contexT__{__ctX__},                                               \
          __messagE__{std::move(__msG__)} {}                                  \
                                                                              \
    __ErrTypE__(::gerr::Error const& __c__, ContextType const& __ctX__,       \
                std::string&& __msG__, __PrivateStruct__ const&)              \
        : __causE__{__c__},                                                   \
          __contexT__{__ctX__},                                               \
          __messagE__{std::move(__msG__)} {}                                  \
                                                                              \
    __ErrTypE__(::gerr::Error&& __c__, ContextType&& __ctX__,                 \
                std::string&& __msG__, __PrivateStruct__ const&)              \
        : __causE__{::std::move(__c__)},                                      \
          __contexT__{std::move(__ctX__)},                                    \
          __messagE__{std::move(__msG__)} {}                                  \
                                                                              \
    __ErrTypE__(::gerr::Error const& __c__, ContextType&& __ctX__,            \
                std::string&& __msG__, __PrivateStruct__ const&)              \
        : __causE__{__c__},                                                   \
          __contexT__{std::move(__ctX__)},                                    \
          __messagE__{std::move(__msG__)} {}                                  \
                                                                              \
    static ::gerr::Error E(ContextType const& context) {                      \
      return ::gerr::Make<__ErrTypE__>(                                       \
          context, fmt::format(__ErrFormaT__, __VA_ARGS__),                   \
          __PrivateStruct__{});                                               \
    }                                                                         \
                                                                              \
    static ::gerr::Error E(ContextType&& context) {                           \
      auto __tempMsG__ = fmt::format(__ErrFormaT__, __VA_ARGS__);             \
      return ::gerr::Make<__ErrTypE__>(context, std::move(__tempMsG__),       \
                                       __PrivateStruct__{});                  \
    }                                                                         \
                                                                              \
    template <class ErrType,                                                  \
              class = typename ::std::enable_if<::std::is_base_of<            \
                  ::gerr::details::IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType>&& __p__,                \
                           ContextType const& context) {                      \
      return ::gerr::Make<__ErrTypE__>(                                       \
          ::std::move(__p__), fmt::format(__ErrFormaT__, __VA_ARGS__),        \
          __PrivateStruct__{});                                               \
    }                                                                         \
                                                                              \
    template <class ErrType,                                                  \
              class = typename ::std::enable_if<::std::is_base_of<            \
                  ::gerr::details::IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType> const& __p__,           \
                           ContextType const& context) {                      \
      return ::gerr::Make<__ErrTypE__>(                                       \
          __p__, fmt::format(__ErrFormaT__, __VA_ARGS__),                     \
          __PrivateStruct__{});                                               \
    }                                                                         \
                                                                              \
    template <class ErrType,                                                  \
              class = typename ::std::enable_if<::std::is_base_of<            \
                  ::gerr::details::IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType>&& __p__,                \
                           ContextType&& context) {                           \
      auto __tempMsG__ = fmt::format(__ErrFormaT__, __VA_ARGS__);             \
      return ::gerr::Make<__ErrTypE__>(                                       \
          ::std::move(__p__), std::move(__tempMsG__), __PrivateStruct__{});   \
    }                                                                         \
                                                                              \
    template <class ErrType,                                                  \
              class = typename ::std::enable_if<::std::is_base_of<            \
                  ::gerr::details::IError, ErrType>::value>::type>            \
    static ::gerr::Error E(::std::shared_ptr<ErrType> const& __p__,           \
                           ContextType&& context) {                           \
      auto __tempMsG__ = fmt::format(__ErrFormaT__, __VA_ARGS__);             \
      return ::gerr::Make<__ErrTypE__>(__p__, std::move(__tempMsG__),         \
                                       __PrivateStruct__{});                  \
    }                                                                         \
                                                                              \
    int Code() override { return __ErrCodE__; }                               \
    char const* Message() override { return __messagE__.c_str(); }            \
    ::gerr::Error Cause() override { return __causE__; }                      \
    ContextType& Context() { return __contexT__; }                            \
                                                                              \
   private:                                                                   \
    ::gerr::Error __causE__{};                                                \
    ContextType __contexT__{};                                                \
    ::std::string __messagE__{};                                              \
  }

namespace gerr {

namespace details {
struct IError;
}

/**
 * 错误类型，本身是一个共享指针，因此可以通过 err == nullptr
 * 这样的语法来判定是否有错误出现。 例如：
 *   auto err = SomeFunction(uin);
 *   if (err != nullptr) {
 *       return err;
 *   }
 *
 * 对于函数的定义者，可以这样写：
 *   gerr::Error SomeFunction(unsigned uin) {
 *       if (uin == 0) {
 *           return gerr::New("UIN can not be 0");
 *       }
 *       return nullptr;
 *   }
 *
 * 更多可用的构建错误的方法，可以参考 gerr::Make / gerr::New / gerr::Wrap
 */
using Error = std::shared_ptr<details::IError>;

namespace details {

/**
 * 错误的基础类型，所有的错误都应该继承自这个类型。
 * 自己定义一个 gerr::Error
 * 类型主要是用于希望错误本身带有一些能够被直接获取到的信息的场合。
 * 一般简单的自定义类型可以使用上面的 DEFINE_ERROR 宏来实现。
 *
 * Example:
 *   // 定义一个自定义错误类型，带上出错的 UIN。
 *   struct MyErrType : ::gerr::details::IError {
 *       MyErrType(unsigned u): uin(u) {}
 *       char const *Message() override { return "my error occurs"; } \
 *       unsigned uin{};
 *   };
 *
 *   // 构建的地方需要用 gerr::Make 函数来处理：
 *   gerr::Error MyFunction() {
 *       auto const ok = SomeFunction(uin);
 *       if (!ok) {
 *           return gerr::Make<MyErrType>(uin);
 *       }
 *       return nullptr;
 *   }
 */
struct IError : std::enable_shared_from_this<IError> {
  virtual ~IError() = 0;
  // override 此函数来返回错误码
  virtual int Code() { return 0; }
  // override 此函数来返回错误信息
  virtual char const* Message() { return nullptr; }
  // override 此函数来返回父错误
  virtual Error Cause() { return nullptr; }

  Error AsError() { return shared_from_this(); }
};

inline IError::~IError() {}

}  // namespace details

/**
 * 将通用错误转换为特定类型的错误，如果不成功，返回 nullptr。
 * 允许传入 nullptr，此时返回的就是 nullptr。
 */
template <class ExpectErr, class OriginErr,
          class = typename std::enable_if<
              std::is_base_of<details::IError, ExpectErr>::value &&
              std::is_base_of<details::IError, OriginErr>::value>::type>
std::shared_ptr<ExpectErr> As(std::shared_ptr<OriginErr> const& err) {
  if (err == nullptr) {
    return nullptr;
  }

  for (auto p = err; p != nullptr; p = p->Cause()) {
    auto p1 = std::dynamic_pointer_cast<ExpectErr>(p);
    if (p1 != nullptr) {
      return p1;
    }
  }
  return nullptr;
}

/**
 * 判定一个错误是否是指定类型的错误。
 * 在错误链条上没有找到任何指定类型的错误时，返回 false 否则返回 true。
 * 允许传入 nullptr，此时返回 false。
 */
template <class ExpectErr, class OriginErr,
          class = typename std::enable_if<
              std::is_base_of<details::IError, ExpectErr>::value &&
              std::is_base_of<details::IError, OriginErr>::value>::type>
bool Is(std::shared_ptr<OriginErr> const& err) {
  return As<ExpectErr>(err) != nullptr;
}

/**
 * 查找错误链条上是否存在带有指定错误码的错误，
 * 在错误链条上没有找到任何相同错误码时，返回 nullptr。
 * 否则返回第一个查找到的错误码匹配的错误。
 * 允许传入 nullptr，此时返回 nullptr。
 */
template <class ErrType, class = typename std::enable_if<std::is_base_of<
                             details::IError, ErrType>::value>::type>
Error AsCode(int code, std::shared_ptr<ErrType> const& err) {
  if (err == nullptr) {
    return 0;
  }

  for (auto p = std::static_pointer_cast<details::IError>(err); p != nullptr;
       p = p->Cause()) {
    if (p->Code() == code) {
      return p;
    }
  }
  return nullptr;
}

/**
 * 查找错误链条上是否存在带有指定错误码的错误，
 * 在错误链条上没有找到任何相同错误码时，返回 false 否则返回 true。
 * 允许传入 nullptr，此时返回 false。
 */
template <class ErrType, class = typename std::enable_if<std::is_base_of<
                             details::IError, ErrType>::value>::type>
bool IsCode(int code, std::shared_ptr<ErrType> const& err) {
  return AsCode(code, err) != nullptr;
}

/**
 * 将错误格式化为字符串
 * 格式为：
 *   顶层错误码: 顶层错误信息: 下层错误码: 下层错误信息 ... 底层错误码:
 * 底层错误信息 如果某一层的错误中，错误码为 0，则该层的错误码不会打印
 * 如果某一层的错误中，错误信息为空字符串或者空指针，则该层的错误信息不会打印
 * 如果某一层错误码为 0
 * 错误信息为空字符串或空指针，这个属于不正常情况，会打印一个 <EMPTY> 予以提示
 * 如果传递进来的是空指针，那么会打印 <NIL> 予以提示
 */
template <class ErrType, class = typename std::enable_if<std::is_base_of<
                             details::IError, ErrType>::value>::type>
std::string String(std::shared_ptr<ErrType> const& err) {
  if (err == nullptr) {
    return "<NIL>";
  }

  std::string s{};
  for (auto p = std::static_pointer_cast<details::IError>(err);;) {
    auto const c = p->Code();
    auto const msg = p->Message();
    auto const hasMsg = msg != nullptr && msg[0] != '\0';
    if (c != 0 && hasMsg) {
      s += std::to_string(c);
      s += ":";
      s += msg;
    } else if (c == 0 && !hasMsg) {
      // 完全空的 error，这个不应该出现，这里打印一点信息提示一下
      s += "<EMPTY>";
    } else if (c == 0) {
      // 如果 code == 0，就只打印 message
      s += msg;
    } else {  // c != 0 && !hasMsg
      // 如果 message 是空，就只打印 code
      s += std::to_string(c);
    }
    p = p->Cause();
    if (p != nullptr) {
      s += ":";
    } else {
      break;
    }
  }
  return s;
}

/**
 * 获取一个错误链条上的第一个错误码，当传入的 err == nullptr 时，返回 0。
 * 当 err != nullptr 且在错误链条上没有找到任何错误码时，返回 defaultErrCode。
 */
template <class ErrType, class = typename std::enable_if<std::is_base_of<
                             details::IError, ErrType>::value>::type>
int Code(std::shared_ptr<ErrType> const& err, int defaultErrCode = -1) {
  if (err == nullptr) {
    return 0;
  }

  for (auto p = std::static_pointer_cast<details::IError>(err); p != nullptr;
       p = p->Cause()) {
    auto const c = p->Code();
    if (c != 0) {
      return c;
    }
  }
  return defaultErrCode;
}

/**
 * 这里定义了一堆错误类型，主要是为了实现上的高效，尽可能让错误类型占用的内存减少。
 * 一般使用的时候不需要关心。
 */
namespace details {

/** 只包含一个 C 风格字符串的错误 */
class RawStrMessageError : public IError {
 public:
  RawStrMessageError(char const* message) : errorMessage_{message} {}
  int Code() override { return 0; }
  char const* Message() override { return errorMessage_; }
  Error Cause() override { return nullptr; }

 private:
  char const* errorMessage_{};
};

/** 只包含一个 std::string 字符串的错误 */
class MessageError : public IError {
 public:
  MessageError(std::string message) : errorMessage_{std::move(message)} {}
  int Code() override { return 0; }
  char const* Message() override { return errorMessage_.c_str(); }
  Error Cause() override { return nullptr; }

 private:
  std::string errorMessage_{};
};

/** 包含一个错误码和一个 C 风格字符串的错误 */
class CodeRawStrMessageError : public IError {
 public:
  CodeRawStrMessageError(int code, char const* message)
      : errorCode_{code}, errorMessage_{message} {}
  int Code() override { return errorCode_; }
  char const* Message() override { return errorMessage_; }
  Error Cause() override { return nullptr; }

 private:
  int errorCode_{};
  char const* errorMessage_{};
};

/** 包含一个错误码和一个 std::string 字符串的错误 */
class CodeMessageError : public IError {
 public:
  CodeMessageError(int code, std::string message)
      : errorCode_{code}, errorMessage_{std::move(message)} {}
  int Code() override { return errorCode_; }
  char const* Message() override { return errorMessage_.c_str(); }
  Error Cause() override { return nullptr; }

 private:
  int errorCode_{};
  std::string errorMessage_{};
};

/** 只包含一个错误码和一个父错误的错误 */
class CodeSubError : public IError {
 public:
  CodeSubError(int code, Error cause)
      : errorCode_{code}, causeError_{std::move(cause)} {}
  int Code() override { return errorCode_; }
  char const* Message() override { return nullptr; }
  Error Cause() override { return causeError_; }

 private:
  int errorCode_{};
  Error causeError_{};
};

/** 只包含一个 C 风格字符串和一个父错误的错误 */
class RawStrMessageSubError : public IError {
 public:
  RawStrMessageSubError(char const* message, Error cause)
      : errorMessage_{message}, causeError_{std::move(cause)} {}
  int Code() override { return 0; }
  char const* Message() override { return errorMessage_; }
  Error Cause() override { return causeError_; }

 private:
  char const* errorMessage_{};
  Error causeError_{};
};

/** 只包含一个 std::string 字符串和一个父错误的错误 */
class MessageSubError : public IError {
 public:
  MessageSubError(std::string message, Error cause)
      : errorMessage_{std::move(message)}, causeError_{std::move(cause)} {}
  int Code() override { return 0; }
  char const* Message() override { return errorMessage_.c_str(); }
  Error Cause() override { return causeError_; }

 private:
  std::string errorMessage_{};
  Error causeError_{};
};

/** 包含一个错误码和一个 C 风格字符串和一个父错误的错误 */
class CodeRawStrMessageSubError : public IError {
 public:
  CodeRawStrMessageSubError(int code, char const* message, Error cause)
      : errorCode_{code},
        errorMessage_{message},
        causeError_{std::move(cause)} {}
  int Code() override { return errorCode_; }
  char const* Message() override { return errorMessage_; }
  Error Cause() override { return causeError_; }

 private:
  int errorCode_{};
  char const* errorMessage_{};
  Error causeError_{};
};

/** 包含一个错误码和一个 std::string 字符串和一个父错误的错误 */
class CodeMessageSubError : public IError {
 public:
  CodeMessageSubError(int code, std::string message, Error cause)
      : errorCode_{code},
        errorMessage_{std::move(message)},
        causeError_{std::move(cause)} {}
  int Code() override { return errorCode_; }
  char const* Message() override { return errorMessage_.c_str(); }
  Error Cause() override { return causeError_; }

 private:
  int errorCode_{};
  std::string errorMessage_{};
  Error causeError_{};
};

}  // namespace details

/**
 * 构建指定类型的 error
 * Example:
 *   struct MyErrType: gerr::details::IError {
 *     MyErrorType(int r, int c, std::string m): callRet(r), apiCode(c),
 *     apiMessage(std::move(m)) {} int callRet; int apiCode{}; std::string
 *     apiMessage{};
 *   }
 *
 *   ApiRsp rsp{};
 *   int ret = api.Call(&rsp);
 *   if (ret != 0 || rsp.ret != 0) {
 *     return gerr::Make<MyErrType>(ret, rsp.ret, rsp.msg);
 *   }
 */
template <class ErrType, class... Args>
inline Error Make(Args&&... args) {
  auto p = std::make_shared<ErrType>(std::forward<Args>(args)...);
  return std::static_pointer_cast<details::IError>(p);
}

/**
 * 新建一个 err 对象，附加额外的错误信息
 * Example:
 *   auto const ok = CallSomeFunction();
 *   if (!ok) {
 *       return gerr::New("error occurs !");
 *   }
 */
inline Error New(char const* msg) {
  return Make<details::RawStrMessageError>(msg);
}

/**
 * 新建一个 err 对象，附加额外的错误信息
 * Example:
 *   auto const ok = CallSomeFunction();
 *   if (!ok) {
 *       return gerr::New("fail to call: uin={}", uin);
 *   }
 */
template <class... Args>
inline Error New(char const* formatStr, Args&&... args) {
  return Make<details::MessageError>(
      fmt::format(formatStr, std::forward<Args>(args)...));
}

template <class... Args>
inline Error New(std::string const& formatStr, Args&&... args) {
  return Make<details::MessageError>(
      fmt::format(formatStr, std::forward<Args>(args)...));
}

/**
 * 新建一个 err 对象，附加额外的错误码和错误信息
 * Example:
 *   auto const ok = CallSomeFunction();
 *   if (!ok) {
 *       return gerr::New(kMyErrorCode, "error occurs!");
 *   }
 */
inline Error New(int code, char const* msg) {
  return Make<details::CodeRawStrMessageError>(code, msg);
}

/**
 * 新建一个 err 对象，附加额外的错误码和错误信息
 * Example:
 *   auto const ok = CallSomeFunction();
 *   if (!ok) {
 *       return gerr::New(kMyErrorCode, "fail to call: uin={}", uin);
 *   }
 */
template <class... Args>
inline Error New(int code, std::string const& formatStr, Args&&... args) {
  return Make<details::CodeMessageError>(
      code, fmt::format(formatStr, std::forward<Args>(args)...));
}

template <class... Args>
inline Error New(int code, char const* formatStr, Args&&... args) {
  return Make<details::CodeMessageError>(
      code, fmt::format(formatStr, std::forward<Args>(args)...));
}

/**
 * 包装一个 err 对象，附加额外的错误信息
 * Example:
 *   auto const err = CallSomeFunction();
 *   if (err != nullptr) {
 *       return gerr::Wrap(err, "call some function fail");
 *   }
 */
template <class... Args>
inline Error Wrap(Error err, char const* msg) {
  auto ptr =
      std::make_shared<details::RawStrMessageSubError>(msg, std::move(err));
  return std::static_pointer_cast<details::IError>(ptr);
}

/**
 * 包装一个 err 对象，附加额外的错误信息
 * Example:
 *   auto const err = CallSomeFunction();
 *   if (err != nullptr) {
 *       return gerr::Wrap(err, "addition info: uin={}, clienttype={}", uin,
 *       clienttype);
 *   }
 */
template <class... Args>
inline Error Wrap(Error err, std::string const& formatStr, Args&&... args) {
  return Make<details::MessageSubError>(
      fmt::format(formatStr, std::forward<Args>(args)...), std::move(err));
}

template <class... Args>
inline Error Wrap(Error err, char const* formatStr, Args&&... args) {
  return Make<details::MessageSubError>(
      fmt::format(formatStr, std::forward<Args>(args)...), std::move(err));
}

/**
 * 包装一个 err 对象，附加一个错误码
 * Example:
 *   auto const err = CallSomeFunction();
 *   if (err != nullptr) {
 *       return gerr::Wrap(err, kMyErrorCode);
 *   }
 */
template <class... Args>
inline Error Wrap(Error err, int code) {
  return Make<details::CodeSubError>(code, std::move(err));
}

/**
 * 包装一个 err 对象，附加错误码和额外的错误信息
 * Example:
 *   auto const err = CallSomeFunction();
 *   if (err != nullptr) {
 *       return gerr::Wrap(err, kMyErrorCode, "call some function fail");
 *   }
 */
template <class... Args>
inline Error Wrap(Error err, int code, char const* msg) {
  return Make<details::CodeRawStrMessageSubError>(code, msg, std::move(err));
}

/**
 * 包装一个 err 对象，附加错误码和额外的错误信息
 * Example:
 *   auto const err = CallSomeFunction();
 *   if (err != nullptr) {
 *       return gerr::Wrap(err, kMyErrorCode, "addition info: uin={}", uin);
 *   }
 */
template <class... Args>
inline Error Wrap(Error err, int code, std::string const& formatStr,
                  Args&&... args) {
  return Make<details::CodeMessageSubError>(
      code, fmt::format(formatStr, std::forward<Args>(args)...),
      std::move(err));
}

template <class... Args>
inline Error Wrap(Error err, int code, char const* formatStr, Args&&... args) {
  return Make<details::CodeMessageSubError>(
      code, fmt::format(formatStr, std::forward<Args>(args)...),
      std::move(err));
}

}  // namespace gerr
