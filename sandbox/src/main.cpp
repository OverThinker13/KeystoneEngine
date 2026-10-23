#include "keystone/core/Log.h"
#include "keystone/platform/Window.h"

// 断言：KeystoneEngine 只支持 64 位 Windows
static_assert(sizeof(void*) == 8, "KeystoneEngine targets 64-bit Windows");

int main() {
    KS_LOG_INFO("KeystoneEngine 0.3.0 starting");

    ks::Window window;
    if (!window.Create(1280, 720, "KeystoneEngine")) {
        KS_LOG_ERROR("failed to create window");
        return 1;
    }

    KS_LOG_INFO("window ready, client = %dx%d", window.Width(), window.Height());

    while (window.PollEvents()) {
        // 尺寸变化：拖窗口边框后重建帧缓冲（S06 用得上）
        if (window.ConsumeResized()) {
            KS_LOG_INFO("client size changed -> %dx%d", window.Width(), window.Height());
        }

        // 下一课在这里渲染

        // 限帧：阻塞到下一次屏幕刷新，避免主循环空转把 CPU 跑满
        window.WaitForNextFrame();
    }

    KS_LOG_INFO("main loop exited");
    return 0;
}
