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
        uint32_t graphicsFamily;
        uint32_t presentFamily;

        bool hasGraphicsFamily = false;
        bool hasPresentFamily = false;

        operator bool() {
            return hasGraphicsFamily && hasPresentFamily;
        }
    };

    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

public:
#if defined(_WIN32)
    explicit BVulkanDevice(const vk::Win32SurfaceCreateInfoKHR& surfaceInfo);
#endif
    ~BVulkanDevice() = default;
    BVulkanDevice(const BVulkanDevice& device) = delete;
    BVulkanDevice(BVulkanDevice&& device) = delete;
    BVulkanDevice& operator=(const BVulkanDevice& device) = delete;
    BVulkanDevice& operator=(BVulkanDevice&& device) = delete;

public:
    const vk::Device& device() const;
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
    void copyBuffer(const vk::Buffer& src, vk::Buffer& dst, vk::DeviceSize size);
    SwapchainSupportDetails getSwapchainSupport() const;
    QueueFamilyIndices findPhysicalQueueFamilies() const;
    const vk::CommandPool& getCommandPool() const;
    const vk::Queue& getGraphicsQueue() const;
    const vk::Queue& getPresentQueue() const;
    const vk::SurfaceKHR& surface() const;
    vk::ImageView createImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);
    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
    void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& memory);

private:
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

private:
    bool checkValidationLayerSupport() const;
    void checkExtensionsSupport() const;
    std::vector<const char*> getRequiredExtensions() const;
    static void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);
    bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& device) const;
    QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;
    SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& device) const;
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

private:
    vk::Instance m_instance{};
    vk::SurfaceKHR m_surface{};
    vk::PhysicalDevice m_physical{};
    vk::Device m_device{};
    vk::Queue m_graphicsQueue{};
    vk::Queue m_presentQueue{};
    vk::CommandPool m_commandPool{};

#if defined(_WIN32)
    std::vector<const char*> m_deviceExtensions = {"VK_KHR_swapchain"};
#else
    std::vector<const char*> m_deviceExtensions = {"VK_KHR_portability_subset", "VK_KHR_swapchain"};
#endif

#if defined(NOT_DEBUG)
    bool m_enableValidationLayers = false;
#else
    bool m_enableValidationLayers = true;
    std::vector<const char*> m_validationLayers{"VK_LAYER_KHRONOS_validation"};
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                        void* userData);
    vk::DebugUtilsMessengerEXT m_debugUtilsMessenger;
#endif  // NOT_DEBUG
};