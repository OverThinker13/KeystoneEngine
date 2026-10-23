#pragma once

#include "keystone/core/Log.h"

#if defined(_MSC_VER)
#define KS_DEBUGBREAK() __debugbreak()
#else
#define KS_DEBUGBREAK() __builtin_trap()
#endif

// 断言在 Release 里也保留：这是学习项目，一个静默的内存错误
// 比一次崩溃贵得多。断言只用于"程序写错了"，不用于处理用户输入。
#define KS_ASSERT(condition, message)                                                        \
    do {                                                                                    \
        if (!(condition)) {                                                                 \
            KS_LOG_FATAL("assertion failed: %s | %s", #condition, (message));               \
            KS_DEBUGBREAK();                                                                \
        }                                                                                   \
    } while (false)
