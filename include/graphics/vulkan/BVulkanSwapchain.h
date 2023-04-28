#pragma once

/**
 * @file BVulkanSwapchain.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include <vector>

#include "BVulkanHeader.h"

class BVulkanDevice;

class BVulkanSwapchain {
public:
    BVulkanSwapchain(BVulkanDevice* device, int width, int height);
    ~BVulkanSwapchain();
    BVulkanSwapchain(const BVulkanSwapchain& swapchain) = delete;
    BVulkanSwapchain(BVulkanSwapchain&& swapchain) = delete;
    BVulkanSwapchain& operator=(const BVulkanSwapchain& swapchain) = delete;
    BVulkanSwapchain& operator=(BVulkanSwapchain&& swapchain) = delete;

public:
    vk::Extent2D getSwapchainExtent() const;
    size_t getImageCount() const;
    const vk::RenderPass& getRenderPass() const;
    float getExtentAspectRatio() const;
    uint32_t acquireNextImage();
    void submitCommandBuffers(const vk::CommandBuffer& buffer, uint32_t imageIndex);
    const vk::Framebuffer& getFrameBuffer(size_t index) const;

private:
    void createSwapchain();
    void createRenderPass();
    void createDepthResources();
    void createFrameBuffers();
    void createSyncObjects();

private:
    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    vk::Format findDepthFormat() const;
    const vk::Format& getSwapchainImageFormat() const;

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT{2};

private:
    BVulkanDevice* m_device{};
    vk::Extent2D m_canvasExtent{};
    vk::Format m_swapchainImageFormat{};
    vk::Extent2D m_swapchainExtent{};
    vk::SwapchainKHR m_swapchain{};
    std::vector<vk::Image> m_swapchainImages{};
    std::vector<vk::ImageView> m_swapchainImageViews{};
    vk::RenderPass m_renderPass{};
    std::vector<vk::Image> m_depthImages{};
    std::vector<vk::DeviceMemory> m_depthImageMemories{};
    std::vector<vk::ImageView> m_depthImageViews{};
    std::vector<vk::Framebuffer> m_swapchainFrameBuffers{};
    std::vector<vk::Semaphore> m_imageAvailableSemaphores{};
    std::vector<vk::Semaphore> m_renderFinishedSemaphores{};
    std::vector<vk::Fence> m_inFlightFences{};
    std::vector<vk::Fence> m_imagesInFlight{};
    size_t m_currentFrame{0};
};