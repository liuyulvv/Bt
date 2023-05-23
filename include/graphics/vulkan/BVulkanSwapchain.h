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
    vk::Extent2D GetSwapchainExtent() const;
    size_t GetImageCount() const;
    const vk::RenderPass& GetRenderPass() const;
    float GetExtentAspectRatio() const;
    uint32_t AcquireNextImage();
    void SubmitCommandBuffers(const vk::CommandBuffer& buffer, uint32_t image_index);
    const vk::Framebuffer& GetFrameBuffer(size_t index) const;

private:
    void CreateSwapchain();
    void CreateRenderPass();
    void CreateDepthResources();
    void CreateFrameBuffers();
    void CreateSyncObjects();

private:
    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);
    static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);
    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    vk::Format FindDepthFormat() const;
    const vk::Format& GetSwapchainImageFormat() const;

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT{2};

private:
    BVulkanDevice* device_{};
    vk::Extent2D canvas_extent_{};
    vk::Format swapchain_image_format_{};
    vk::Extent2D swapchain_extent_{};
    vk::SwapchainKHR swapchain_{};
    std::vector<vk::Image> swapchain_images_{};
    std::vector<vk::ImageView> swapchain_image_views_{};
    vk::RenderPass render_pass_{};
    std::vector<vk::Image> depth_images_{};
    std::vector<vk::DeviceMemory> depth_image_memories_{};
    std::vector<vk::ImageView> depth_image_views_{};
    std::vector<vk::Framebuffer> swapchain_frame_buffers_{};
    std::vector<vk::Semaphore> image_available_semaphores_{};
    std::vector<vk::Semaphore> render_finished_semaphores_{};
    std::vector<vk::Fence> in_flight_fences_{};
    std::vector<vk::Fence> images_in_flight_{};
    size_t current_frame_{0};
};