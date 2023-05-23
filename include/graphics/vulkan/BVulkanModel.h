#pragma once

/**
 * @file BVulkanModel.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include <cstdint>
#include <vector>

#include "BVulkanHeader.h"

class BVulkanDevice;

class BVulkanModel {
public:
    struct Vertex {
        glm::vec3 position_;
        glm::vec4 color_;
        bool operator==(const Vertex& other) const;
        static std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();
    };

public:
    BVulkanModel(BVulkanDevice* device, const std::vector<Vertex>& vertices);
    ~BVulkanModel();
    BVulkanModel(const BVulkanModel& model) = default;
    BVulkanModel(BVulkanModel&& model) = default;
    BVulkanModel& operator=(const BVulkanModel& model) = default;
    BVulkanModel& operator=(BVulkanModel&& model) = default;

public:
    void Bind(vk::CommandBuffer& command_buffer) const;
    void Draw(vk::CommandBuffer& command_buffer) const;

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);

private:
    BVulkanDevice* device_{};
    vk::Buffer vertex_buffer_{};
    vk::DeviceMemory vertex_buffer_memory_{};
    uint32_t vertex_count_{0};
};