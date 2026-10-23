<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="assets/logo-lockup-dark.svg">
    <source media="(prefers-color-scheme: light)" srcset="assets/logo-lockup.svg">
    <img src="assets/logo-lockup.svg" alt="KeystoneEngine" width="520">
  </picture>
</p>

# KeystoneEngine

**KeystoneEngine 是一个用 C++20 与 OpenGL 从零构建的轻量级实时 3D 引擎。**

它面向一个明确的目标：让引擎的每一层都能被单独解释、单独测试、单独替换。平台层、渲染后端、场景组织与资源生命周期各自独立，依赖只向下——上层的改动不会渗进下层。

引擎以静态库形式提供，示例程序、编辑器与测试共用同一份实现。项目按里程碑推进，当前处于早期开发阶段。

| | |
|---|---|
| **技术栈** | C++20 · OpenGL 3.3 Core · CMake |
| **平台** | Windows 10/11 |
| **状态** | 早期开发 |
| **协议** | MIT |

> 名字取自拱心石：拱顶合拢时最后落下的那块石头，整个结构的重量最终压在它身上。
> 引擎照这个位置来设计——不做最显眼的一层，只做让上面所有东西站得住的那一层。

## 构建

Windows 10/11 + CMake ≥ 3.20 + MinGW-w64（GCC 13+）或 Visual Studio 2022。

```powershell
# MinGW + Ninja
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build
.\build\sandbox.exe

# 运行测试
ctest --test-dir build --output-on-failure
```

## 目录

```
engine/   引擎本体（静态库）
sandbox/  示例程序（可执行文件）
tests/    单元测试
assets/   品牌资源
```

## License

MIT，见 [LICENSE](LICENSE)。
