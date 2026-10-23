#pragma once

// 平台无关的窗口封装。只管三件事：创建、收消息、报告尺寸。
// 注意：本头文件里不能出现 HWND / HDC / windows.h —— 平台细节全在 WindowWin32.cpp。

namespace ks {

class Window {
public:
    Window() = default;
    ~Window();

    // 不允许拷贝：一个窗口只能有一个所有者（拷贝会导致同一个 HWND 被析构两次）
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // 创建。失败返回 false，原因写日志
    bool Create(int width, int height, const char* title);

    // 处理所有待处理消息。返回 false 表示窗口已关闭
    bool PollEvents();

    // 阻塞到下一次屏幕刷新，用于限帧（避免主循环空转占满 CPU）
    void WaitForNextFrame();

    bool IsValid() const { return m_hwnd != nullptr; }
    int  Width()  const { return m_width; }
    int  Height() const { return m_height; }

    // 取走「尺寸已变化」标记：读一次就清除。
    // 命名用 Consume 而不是 Is，提醒调用方"读走即失效"。
    bool ConsumeResized() {
        bool resized = m_resized;
        m_resized = false;
        return resized;
    }

private:
    void Destroy();

    void* m_hwnd = nullptr;   // HWND
    void* m_dc = nullptr;     // HDC，CS_OWNDC 保证每帧同一个
    int   m_width = 0;
    int   m_height = 0;
    bool  m_resized = false;
};

} // namespace ks
