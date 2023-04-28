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
        glm::vec3 position;
        glm::vec4 color;
        bool operator==(const Vertex& other) const;
        static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();
        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    };

public:
    BVulkanModel(BVulkanDevice* device, const std::vector<Vertex>& vertices);
    ~BVulkanModel();
    BVulkanModel(const BVulkanModel& model) = default;
    BVulkanModel(BVulkanModel&& model) = default;
    BVulkanModel& operator=(const BVulkanModel& model) = default;
    BVulkanModel& operator=(BVulkanModel&& model) = default;

public:
    void bind(vk::CommandBuffer& commandBuffer) const;
    void draw(vk::CommandBuffer& commandBuffer) const;

private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);

private:
    BVulkanDevice* m_device{};
    vk::Buffer m_vertexBuffer{};
    vk::DeviceMemory m_vertexBufferMemory{};
    uint32_t m_vertexCount{0};
};