#include "keystone/platform/Window.h"

#include <windows.h>
#include <dwmapi.h>

#include "keystone/core/Log.h"

namespace ks {
namespace {

// 窗口消息处理回调函数
// 每个窗口必须绑定一个“消息处理函数”（WndProc）。用户点鼠标、按键盘、拖边框、点 ×，Windows
// 都会调用这个函数把事件递进来。
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0); // 发出 WM_QUIT，PollEvents 会收到
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

constexpr const wchar_t* kClassName = L"KeystoneWindowClass";

} // namespace

// RAII 全称：Resource Acquisition Is Initialization  中文：资源获取即初始化
// Window 类的析构函数会自动释放窗口资源，避免内存泄漏。
Window::~Window() {
    Destroy();
}

bool Window::Create(int width, int height, const char* title) {
    HINSTANCE instance = GetModuleHandleW(nullptr);

    // ---- 注册窗口类（进程内只做一次）----
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName = kClassName;

        if (RegisterClassExW(&wc) == 0) {
            KS_LOG_ERROR("RegisterClassExW failed: %lu", GetLastError());
            return false;
        }
        classRegistered = true;
    }

    // ---- UTF-8 标题转 UTF-16 ----
    wchar_t wtitle[256] = {};
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wtitle, 256);

    // ---- 创建窗口 ----
    RECT rc = {0, 0, width, height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowExW(0, kClassName, wtitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr,
                                nullptr, instance, nullptr);

    if (hwnd == nullptr) {
        KS_LOG_ERROR("CreateWindowExW failed: %lu", GetLastError());
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    m_hwnd = hwnd;
    // CS_OWNDC 意味着这个 DC 终身专属于本窗口，可以一直持有，不必每帧 GetDC/ReleaseDC
    m_dc = GetDC(hwnd);
    m_width = width;
    m_height = height;
    KS_LOG_INFO("window created: %dx%d", width, height);
    return true;
}

bool Window::PollEvents() {
    if (m_hwnd == nullptr) {
        return false;
    }

    HWND hwnd = static_cast<HWND>(m_hwnd);

    MSG msg = {};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // 每帧查一次真实绘图区尺寸：用户可能刚拖过窗口边框。
    // 注意顺序——先算出新值放进临时变量，和旧值比较，确认变了再覆盖成员。
    // 如果先赋值再比较，比的就是"新值 vs 新值"，永远相等。
    RECT rc = {};
    if (GetClientRect(hwnd, &rc)) {
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        if (w != m_width || h != m_height) {
            m_width = w;
            m_height = h;
            m_resized = true;
            KS_LOG_INFO("window resized: %dx%d", w, h);
        }
    }

    return true;
}

void Window::WaitForNextFrame() {
    // 等桌面合成器完成一次刷新，效果接近 VSync：
    // 把主循环从"空转占满一个核"降到"跟着屏幕刷新率走"。
    // 真正的 VSync 要等 S19 接上 OpenGL 后用 wglSwapIntervalEXT(1)。
    DwmFlush();
}

void Window::Destroy() {
    if (m_hwnd == nullptr) {
        return;
    }

    HWND hwnd = static_cast<HWND>(m_hwnd);

    if (m_dc != nullptr) {
        ReleaseDC(hwnd, static_cast<HDC>(m_dc));
        m_dc = nullptr;
    }

    // 用户点 × 时窗口已被系统销毁，这里再调 DestroyWindow 会失败。
    // IsWindow 挡一道，避免对失效句柄（可能已被系统复用）重复销毁。
    if (IsWindow(hwnd)) {
        DestroyWindow(hwnd);
    }

    m_hwnd = nullptr;
}

} // namespace ks