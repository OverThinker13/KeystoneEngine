#include "Test.h"
#include "keystone/core/Log.h"
#include <cstring>

void RunLogTests() {
    KS_TEST(1 + 1 == 2);
    KS_TEST_EQ(1 + 1, 2);
    KS_TEST(std::strcmp(ks::LogBaseName("a/b/c.txt"), "c.txt") == 0);
    KS_TEST(std::strcmp(ks::LogBaseName("D:\\a\\b.txt"), "b.txt") == 0);
}
