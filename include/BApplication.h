#pragma once

/**
 * @file BApplication.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BPlatform.h"
#include "BGraphicsVulkan.h"

class BCanvas;

class BApplication final {
public:
    BApplication();
    ~BApplication();
    BApplication(const BApplication& application) = delete;
    BApplication(BApplication&& application) = delete;
    BApplication& operator=(const BApplication& application) = delete;
    BApplication& operator=(BApplication&& application) = delete;

#if defined(_WIN32)
private:
    static LRESULT CALLBACK eventProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

public:
    int exec();

private:
    BCanvas* m_mainCanvas{};

private:
    BVulkanDevice* m_device{};
    BVulkanRender* m_render{};
    std::vector<BVulkanModel> m_models{};
    BVulkanRenderSystem* m_renderSystem{};
};