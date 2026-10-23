#include "Test.h"

int g_failedTests = 0;

int RunAllTests() {
    RunLogTests();
    RunMathTests();
    return g_failedTests;
}

int main() {
    return RunAllTests();
}