/**
 * @file BVulkanModel.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanModel.h"

#include <array>
#include <cstddef>

#include "BVulkanDevice.h"

std::vector<vk::VertexInputBindingDescription> BVulkanModel::Vertex::getBindingDescriptions() {
    std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions.at(0)
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
    return bindingDescriptions;
}

std::vector<vk::VertexInputAttributeDescription> BVulkanModel::Vertex::getAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.push_back({0, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(BVulkanModel::Vertex, position))});
    attributeDescriptions.push_back({1, 0, vk::Format::eR32G32B32A32Sfloat, static_cast<uint32_t>(offsetof(BVulkanModel::Vertex, color))});
    return attributeDescriptions;
}

BVulkanModel::BVulkanModel(BVulkanDevice* device, const std::vector<BVulkanModel::Vertex>& vertices) : m_device(device) {
    createVertexBuffer(vertices);
}

BVulkanModel::~BVulkanModel() {
    m_device->device().waitIdle();
    m_device->device().destroyBuffer(m_vertexBuffer);
    m_device->device().freeMemory(m_vertexBufferMemory);
}

void BVulkanModel::bind(vk::CommandBuffer& commandBuffer) const {
    std::array<vk::Buffer, 1> buffers{m_vertexBuffer};
    commandBuffer.bindVertexBuffers(0, buffers, {0});
}

void BVulkanModel::draw(vk::CommandBuffer& commandBuffer) const {
    commandBuffer.draw(m_vertexCount, 1, 0, 0);
}

void BVulkanModel::createVertexBuffer(const std::vector<Vertex>& vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
    vk::Buffer stagingBuffer{};
    vk::DeviceMemory stagingBufferMemory{};
    m_device->createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);
    auto* data = m_device->device().mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), bufferSize);
    m_device->device().unmapMemory(stagingBufferMemory);
    m_device->createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_vertexBuffer,
        m_vertexBufferMemory);
    m_device->copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
    m_device->device().destroyBuffer(stagingBuffer);
    m_device->device().freeMemory(stagingBufferMemory);
}
