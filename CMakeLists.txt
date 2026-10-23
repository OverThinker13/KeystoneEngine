# ============================================================================
# 1. 基础设置
# ============================================================================

# 最低 CMake 版本，低于 3.20 直接报错
cmake_minimum_required(VERSION 3.20)

# 项目名、版本号；仅启用 C++ 编译器
project(KeystoneEngine VERSION 0.3.0 LANGUAGES CXX)

# C++20，强制要求，不允许降级
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 关闭编译器私有扩展，严格遵循标准
set(CMAKE_CXX_EXTENSIONS OFF)

# 生成 compile_commands.json：
# VSCode 的 C/C++ 扩展靠它拿到真实的编译参数（include 路径、宏、标准），消除假红波浪线
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# 2. 工具函数
# ============================================================================

# 给目标开启高等级警告。
# 警告绑定到 target 而不是全局，互不污染。
function(ks_enable_warnings target)
    if(MSVC)
        # MSVC：/W4 最高等级，/permissive- 严格标准，/utf-8 源码按 UTF-8 解析
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
    else()
        # GCC / Clang
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# ============================================================================
# 3. 引擎静态库（无 main，不能直接运行）
# ============================================================================

add_library(keystone STATIC
    engine/src/core/Log.cpp
    engine/src/platform/WindowWin32.cpp
)

# PUBLIC：keystone 自身编译需要该头文件目录；
# 所有链接 keystone 的目标自动继承这个 include 路径
target_include_directories(keystone PUBLIC ${PROJECT_SOURCE_DIR}/engine/include)

# Win32 编译定义。
# keystone 的 platform/ 层已经用到 windows.h，所以这些宏必须挂在库上；
# PUBLIC 让所有链接 keystone 的目标（sandbox、ks_tests）自动继承。
target_compile_definitions(keystone PUBLIC
    UNICODE              # Windows API 宏映射到 W 版本（WNDCLASSEX → WNDCLASSEXW）
    _UNICODE             # C 运行库同上（<tchar.h> 的 _T 系列）
    WIN32_LEAN_AND_MEAN  # 排除 winsock 等用不到的头，编译更快、冲突更少
    NOMINMAX             # 关掉 windows.h 的 min/max 宏，避免撞 std::min/std::max
)

# PUBLIC：user32 随 keystone 一起传给下游
target_link_libraries(keystone PUBLIC user32 dwmapi)

ks_enable_warnings(keystone)

# ============================================================================
# 4. sandbox 示例程序（可执行文件，演示与手动测试用）
# ============================================================================

add_executable(sandbox
    sandbox/src/main.cpp
)

# 只需要链接 keystone：编译宏、include 路径、user32 都由 keystone 的 PUBLIC 接口继承过来
target_link_libraries(sandbox PRIVATE keystone)

ks_enable_warnings(sandbox)

# ============================================================================
# 5. 单元测试（独立 exe）
# ============================================================================

add_executable(ks_tests
    tests/Test.cpp
    tests/LogTests.cpp
    tests/MathTests.cpp
)

target_link_libraries(ks_tests PRIVATE keystone)

ks_enable_warnings(ks_tests)

# ============================================================================
# 6. CTest 注册
# ============================================================================

enable_testing()
add_test(NAME ks_tests COMMAND ks_tests)
