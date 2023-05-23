/**
 * @file BApplication.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BApplication.h"

#include <stdexcept>

#include "BCanvas.h"

BApplication::BApplication() {
#if defined(_WIN32)
    auto instance = GetModuleHandle(nullptr);
    WNDCLASS window_class{};
    window_class.lpfnWndProc = EventProcess;
    window_class.hInstance = instance;
    window_class.lpszClassName = B_CLASS_NAME;
    RegisterClass(&window_class);
#endif
    main_canvas_ = new BCanvas();
    main_canvas_->Show();
    device_ = new BVulkanDevice({{}, instance, main_canvas_->GetCanvasID()});
    render_ = new BVulkanRender(device_, main_canvas_);
    render_system_ = new BVulkanRenderSystem(device_, render_->GetSwapchainRenderPass());

    if (auto command_buffer = render_->BeginFrame()) {
        render_->BeginSwapchainRenderPass(command_buffer);
        render_system_->RenderObjects(command_buffer, models_);
        render_->EndSwapchainRenderPass(command_buffer);
        render_->EndFrame();
    }
}

BApplication::~BApplication() {
    if (main_canvas_)
        delete main_canvas_;
}

LRESULT BApplication::EventProcess(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    auto* canvas = reinterpret_cast<BCanvas*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (canvas) {
        switch (msg) {
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
            case WM_MOVE: {
                auto x = static_cast<int32_t>(LOWORD(l_param));
                auto y = static_cast<int32_t>(HIWORD(l_param));
                canvas->MoveEvent({x, y});
                return 0;
            }
            case WM_SIZE: {
                auto width = static_cast<uint32_t>(LOWORD(l_param));
                auto height = static_cast<uint32_t>(HIWORD(l_param));
                canvas->ResizeEvent({width, height});
                return 0;
            }
        }
    }
    return DefWindowProc(hwnd, msg, w_param, l_param);
}

int BApplication::Exec() {
#if defined(_WIN32)
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
    return 0;
}
