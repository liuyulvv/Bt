#pragma once

/**
 * @file BVulkanDevice.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include <cstdint>
#include <vector>

#include "BVulkanHeader.h"

class BVulkanDevice {
public:
    struct QueueFamilyIndices {
        uint32_t graphics_family_;
        uint32_t present_family_;

        bool has_graphics_family_ = false;
        bool has_present_family_ = false;

        operator bool() {
            return has_graphics_family_ && has_present_family_;
        }
    };

    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities_;
        std::vector<vk::SurfaceFormatKHR> formats_;
        std::vector<vk::PresentModeKHR> present_modes_;
    };

public:
#if defined(_WIN32)
    explicit BVulkanDevice(const vk::Win32SurfaceCreateInfoKHR& surface_info);
#endif
    ~BVulkanDevice() = default;
    BVulkanDevice(const BVulkanDevice& device) = delete;
    BVulkanDevice(BVulkanDevice&& device) = delete;
    BVulkanDevice& operator=(const BVulkanDevice& device) = delete;
    BVulkanDevice& operator=(BVulkanDevice&& device) = delete;

public:
    const vk::Device& Device() const;
    void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
    void CopyBuffer(const vk::Buffer& src, vk::Buffer& dst, vk::DeviceSize size);
    SwapchainSupportDetails GetSwapchainSupport() const;
    QueueFamilyIndices FindPhysicalQueueFamilies() const;
    const vk::CommandPool& GetCommandPool() const;
    const vk::Queue& GetGraphicsQueue() const;
    const vk::Queue& GetPresentQueue() const;
    const vk::SurfaceKHR& Surface() const;
    vk::ImageView CreateImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect_flags);
    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
    void CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& memory);

private:
    void CreateInstance();
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();

private:
    bool CheckValidationLayerSupport() const;
    void CheckExtensionsSupport() const;
    std::vector<const char*> GetRequiredExtensions() const;
    static void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info);
    bool IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device) const;
    QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device) const;
    SwapchainSupportDetails QuerySwapchainSupport(const vk::PhysicalDevice& device) const;
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    vk::CommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(vk::CommandBuffer command_buffer);

private:
    vk::Instance instance_{};
    vk::SurfaceKHR surface_{};
    vk::PhysicalDevice physical_{};
    vk::Device device_{};
    vk::Queue graphics_queue_{};
    vk::Queue present_queue_{};
    vk::CommandPool command_pool_{};

#if defined(_WIN32)
    std::vector<const char*> device_extensions_ = {"VK_KHR_swapchain"};
#else
    std::vector<const char*> device_extensions_ = {"VK_KHR_portability_subset", "VK_KHR_swapchain"};
#endif

#if defined(NOT_DEBUG)
    bool enable_validation_layers__ = false;
#else
    bool enable_validation_layers__ = true;
    std::vector<const char*> validation_layers_{"VK_LAYER_KHRONOS_validation"};
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                        VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                        void* user_data);
    vk::DebugUtilsMessengerEXT debug_utils_messenger_;
#endif  // NOT_DEBUG
};