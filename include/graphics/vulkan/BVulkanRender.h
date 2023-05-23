#pragma once

/**
 * @file BVulkanRender.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include <cstdint>
#include <memory>
#include <vector>

#include "BVulkanHeader.h"

class BVulkanDevice;
class BGraphicsCanvas;
class BVulkanSwapchain;

class BVulkanRender {
public:
    BVulkanRender(BVulkanDevice* device, BGraphicsCanvas* canvas);
    ~BVulkanRender();
    BVulkanRender(const BVulkanRender& render) = delete;
    BVulkanRender(BVulkanRender&& render) = delete;
    BVulkanRender& operator=(const BVulkanRender& render) = delete;
    BVulkanRender& operator=(BVulkanRender&& render) = delete;

public:
    const vk::RenderPass& GetSwapchainRenderPass() const;
    float GetAspectRatio() const;
    vk::CommandBuffer BeginFrame();
    void EndFrame();
    void BeginSwapchainRenderPass(vk::CommandBuffer command_buffer);
    void EndSwapchainRenderPass(vk::CommandBuffer command_buffer);

private:
    void RecreateSwapchain();
    void CreateCommandBuffers();
    void FreeCommandBuffers();
    bool IsFrameInProgress() const;
    vk::CommandBuffer GetCurrentCommandBuffer() const;

private:
    BVulkanDevice* device_{};
    BGraphicsCanvas* canvas_{};
    std::vector<vk::CommandBuffer> command_buffers_{};
    std::unique_ptr<BVulkanSwapchain> swapchain_{};
    uint32_t current_image_index_{};
    bool is_frame_started_{false};
};