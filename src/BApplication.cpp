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
    WNDCLASS windowClass{};
    windowClass.lpfnWndProc = eventProcess;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = B_CLASS_NAME;
    RegisterClass(&windowClass);
#endif
    m_mainCanvas = new BCanvas();
    m_mainCanvas->show();
    m_device = new BVulkanDevice({{}, instance, m_mainCanvas->getCanvasID()});
    m_render = new BVulkanRender(m_device, m_mainCanvas);
    m_renderSystem = new BVulkanRenderSystem(m_device, m_render->getSwapchainRenderPass());

    if (auto commandBuffer = m_render->beginFrame()) {
        m_render->beginSwapchainRenderPass(commandBuffer);
        m_renderSystem->renderObjects(commandBuffer, m_models);
        m_render->endSwapchainRenderPass(commandBuffer);
        m_render->endFrame();
    }
}

BApplication::~BApplication() {
    if (m_mainCanvas)
        delete m_mainCanvas;
}

LRESULT BApplication::eventProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* canvas = reinterpret_cast<BCanvas*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (canvas) {
        switch (msg) {
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
            case WM_MOVE: {
                auto x = static_cast<int32_t>(LOWORD(lParam));
                auto y = static_cast<int32_t>(HIWORD(lParam));
                canvas->moveEvent({x, y});
                return 0;
            }
            case WM_SIZE: {
                auto width = static_cast<uint32_t>(LOWORD(lParam));
                auto height = static_cast<uint32_t>(HIWORD(lParam));
                canvas->resizeEvent({width, height});
                return 0;
            }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int BApplication::exec() {
#if defined(_WIN32)
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
    return 0;
}
