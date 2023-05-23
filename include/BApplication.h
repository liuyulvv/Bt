#pragma once

/**
 * @file BApplication.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BGraphicsVulkan.h"
#include "BPlatform.h"

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
    static LRESULT CALLBACK EventProcess(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
#endif

public:
    int Exec();

private:
    BCanvas* main_canvas_{};

private:
    BVulkanDevice* device_{};
    BVulkanRender* render_{};
    std::vector<BVulkanModel> models_{};
    BVulkanRenderSystem* render_system_{};
};