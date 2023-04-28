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
    const vk::RenderPass& getSwapchainRenderPass() const;
    float getAspectRatio() const;
    vk::CommandBuffer beginFrame();
    void endFrame();
    void beginSwapchainRenderPass(vk::CommandBuffer commandBuffer);
    void endSwapchainRenderPass(vk::CommandBuffer commandBuffer);

private:
    void recreateSwapchain();
    void createCommandBuffers();
    void freeCommandBuffers();
    bool isFrameInProgress() const;
    vk::CommandBuffer getCurrentCommandBuffer() const;

private:
    BVulkanDevice* m_device{};
    BGraphicsCanvas* m_canvas{};
    std::vector<vk::CommandBuffer> m_commandBuffers{};
    std::unique_ptr<BVulkanSwapchain> m_swapchain{};
    uint32_t m_currentImageIndex{};
    bool m_isFrameStarted{false};
};