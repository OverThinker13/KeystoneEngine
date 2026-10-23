#pragma once
#include "keystone/core/Log.h"
#include <cmath>

// extern声明，告诉编译器这个变量在别的源文件里定义过了，避免重复定义编译错误
// 一般在h文件里extern声明，在cpp文件里真正定义一次，保证全局只有一个变量实例
extern int g_failedTests;
// 加一层do
// while(false)是为了让宏像一条语句一样工作，避免在展开的时候把else和外层的if混在一起，导致语法错误
#define KS_TEST(...)                                                                               \
    do {                                                                                           \
        if (!(__VA_ARGS__)) {                                                                      \
            KS_LOG_WARN("Test failed: %s", #__VA_ARGS__);                                          \
            g_failedTests++;                                                                       \
        }                                                                                          \
    } while (false)

#define KS_TEST_EQ(actual, expected)                                                               \
    do {                                                                                           \
        if ((actual) != (expected)) {                                                              \
            KS_LOG_WARN("Test failed: %s != %s", #actual, #expected);                              \
            g_failedTests++;                                                                       \
        }                                                                                          \
    } while (false)

// 浮点近似相等断言：abs(actual - expected) <= eps
#define KS_TEST_NEAR(actual, expected, eps)                                                        \
    do {                                                                                           \
        if (std::fabs((actual) - (expected)) > (eps)) {                                            \
            KS_LOG_WARN("Test failed: %s (%f) != %s (%f)", #actual, (double)(actual), #expected,   \
                        (double)(expected));                                                       \
            g_failedTests++;                                                                       \
        }                                                                                          \
    } while (false)

// 声明测试函数
int RunAllTests();
void RunLogTests();
void RunMathTests();
