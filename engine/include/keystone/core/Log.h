#pragma once                     // 头文件保护指令：防止该头文件被多次#include，避免重复定义编译错误
#include "keystone/core/Types.h" // 引入引擎基础类型定义头文件，提供u8、i32等自定义固定宽度整数类型

// 引擎里不允许到处 printf。原因不是"优雅"，而是：
//   1. 没有级别，发布时关不掉；
//   2. 没有时间戳和文件行号，出问题不知道在哪；
//   3. 多线程同时 printf 会交错；
//   4. 将来要重定向到控制台面板/文件，printf 做不到。
namespace ks { // 命名空间ks，隔离引擎所有代码，防止和第三方库、系统库的名字冲突
// 强类型枚举：日志等级，底层存储类型指定为u8（1字节无符号整数，节省内存）
enum class LogLevel : u8 {
    Trace, // 最细粒度追踪日志，用于调试流程细节
    Info,  // 普通提示信息，告知正常运行状态
    Warn,  // 警告：非致命异常，程序仍可继续执行
    Error, // 错误：功能执行失败，但程序不会直接终止
    Fatal, // 致命错误：严重故障，打印日志后终止程序
};

// 函数声明（只描述函数签名，实现写在Log.cpp源文件中）
// printf风格可变参数函数，内部自带互斥锁，多线程调用打印不会出现文字错乱（线程安全）
// 参数：
// level：日志等级；file：源码文件名；line：代码行号；fmt：格式化字符串；...可变参数列表
void LogMessage(LogLevel level, const char* file, i32 line, const char* fmt, ...);

const char* LogBaseName(const char* path);

} // namespace ks

// 宏的作用：把 __FILE__ / __LINE__ 自动带上，调用处不用手写。
// do { ... } while (false) 是宏的标准写法，保证它像一条语句一样工作。
// 【宏说明】：下面是可变参数宏，预处理器在编译前做纯文本替换
// __FILE__：预处理器内置宏，替换为当前源码文件名字符串
// __LINE__：预处理器内置宏，替换为当前代码所在行号数字
// __VA_ARGS__：可变参数宏语法，接收传入的全部参数，原样转发给LogMessage函数
// ::ks:: 前面双冒号代表从全局命名空间开始查找ks，避免局部命名空间覆盖带来的查找错误
#define KS_LOG_TRACE(...) ::ks::LogMessage(::ks::LogLevel::Trace, __FILE__, __LINE__, __VA_ARGS__)
#define KS_LOG_INFO(...) ::ks::LogMessage(::ks::LogLevel::Info, __FILE__, __LINE__, __VA_ARGS__)
#define KS_LOG_WARN(...) ::ks::LogMessage(::ks::LogLevel::Warn, __FILE__, __LINE__, __VA_ARGS__)
#define KS_LOG_ERROR(...) ::ks::LogMessage(::ks::LogLevel::Error, __FILE__, __LINE__, __VA_ARGS__)
#define KS_LOG_FATAL(...) ::ks::LogMessage(::ks::LogLevel::Fatal, __FILE__, __LINE__, __VA_ARGS__)
