#include "keystone/core/Log.h" // 引入头文件：拿到 LogLevel 枚举、LogMessage函数声明、KS_LOG_*宏
#include <chrono>              // C++时间库，获取系统当前高精度时间（时:分:秒.毫秒）
#include <cstdarg>             // 可变参数va_list、va_start、va_end，处理 ... 可变参数
#include <cstdio>              // std::fprintf / std::fflush 控制台输出
#include <cstdlib>             // std::abort()，Fatal时终止程序
#include <cstring>             // std::strrchr 查找路径里最后一个斜杠，提取文件名
#include <ctime>               // time_t、tm 本地时间结构体
#include <mutex>               // std::mutex、std::lock_guard，多线程锁，保证日志打印不会乱
namespace ks {

// 裁剪完整文件路径，只保留最后的文件名
// 输入：D:\KeystoneEngine\sandbox\main.cpp  → 输出 main.cpp
const char* LogBaseName(const char* path) {
    // 找最后一个 '/'（unix路径分隔符）
    const char* slash = std::strrchr(path, '/');
    // 找最后一个 '\'（windows路径分隔符）
    const char* backslash = std::strrchr(path, '\\');
    // 取两个里面靠后的那一个
    const char* last = slash > backslash ? slash : backslash;
    // 如果找到了斜杠，斜杠下一个字符就是文件名；找不到直接返回原始路径
    return last != nullptr ? last + 1 : path;
}

// 匿名命名空间 namespace {}：里面的东西仅在当前这个 .cpp 文件内可见，别的文件访问不到
namespace {
std::mutex g_logMutex; // 全局静态互斥锁，所有线程打印日志共用这一把锁

// 根据日志等级，返回对应的字符串文本 "TRACE"/"INFO"/"WARN"/"ERROR"/"FATAL"
const char* LevelName(LogLevel level) {
    switch (level) {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Info:
        return "INFO ";
    case LogLevel::Warn:
        return "WARN ";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Fatal:
        return "FATAL";
    }
    return "?????"; // 兜底，非法等级
}

} // namespace

// LogMessage 函数【定义/实现】，对应 Log.h 里的函数声明
void LogMessage(LogLevel level, const char* file, i32 line, const char* fmt, ...) {

    // ==========把模板 + 零散参数，合并成**一段完整的字符串**，存进 message 缓冲区，后面交给
    // fprintf 打印==========
    char message[2048];  // 缓冲区，存放格式化后的日志文本，最多2047个字符
    va_list args;        // 可变参数容器
    va_start(args, fmt); // 初始化可变参数，从fmt后面开始拿参数
    // vsnprintf：把可变参数格式化，写入message缓冲区，防止缓冲区溢出
    std::vsnprintf(message, sizeof(message), fmt, args);
    va_end(args); // 清理可变参数资源

    // ========== 获取时间戳（时:分:秒.毫秒）==========
    const auto now = std::chrono::system_clock::now();                     // 获取当前系统时间点
    const std::time_t seconds = std::chrono::system_clock::to_time_t(now); // 转成秒数
    // 计算毫秒部分
    const auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::tm local{}; // tm结构体，存放时分秒年月日

    // 跨平台时间转换：Windows用localtime_s，Linux/macOS用localtime_r
#if defined(_WIN32)
    localtime_s(&local, &seconds);
#else
    localtime_r(&seconds, &local);
#endif

    {
        // lock_guard：自动加锁，离开大括号作用域自动解锁（RAII）
        // <std::mutex>表示模板lock_guard的参数，告诉编译器要用哪一种锁
        std::lock_guard<std::mutex> lock(g_logMutex);
        // 打印最终日志：[时:分:秒.毫秒] [等级] 日志内容 (文件名:行号)
        std::fprintf(stdout, "[%02d:%02d:%02d.%03d] [%s] %s  (%s:%d)\n", local.tm_hour,
                     local.tm_min, local.tm_sec, static_cast<int>(millis.count()), LevelName(level),
                     message, LogBaseName(file), line);
        std::fflush(stdout); // 强制刷新控制台缓冲区，立刻输出，不缓存
    } // 大括号结束，lock_guard销毁，自动释放锁

    // 如果是Fatal等级，打印完成直接终止程序
    if (level == LogLevel::Fatal) {
        std::abort();
    }
}

} // namespace ks
