/**
 * @file BVulkanModel.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanModel.h"

#include <array>
#include <cstddef>

#include "BVulkanDevice.h"

std::vector<vk::VertexInputBindingDescription> BVulkanModel::Vertex::GetBindingDescriptions() {
    std::vector<vk::VertexInputBindingDescription> binding_descriptions(1);
    binding_descriptions.at(0)
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
    return binding_descriptions;
}

std::vector<vk::VertexInputAttributeDescription> BVulkanModel::Vertex::GetAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> attribute_descriptions{};
    attribute_descriptions.push_back({0, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(BVulkanModel::Vertex, position_))});
    attribute_descriptions.push_back({1, 0, vk::Format::eR32G32B32A32Sfloat, static_cast<uint32_t>(offsetof(BVulkanModel::Vertex, color_))});
    return attribute_descriptions;
}

BVulkanModel::BVulkanModel(BVulkanDevice* device, const std::vector<BVulkanModel::Vertex>& vertices) : device_(device) {
    CreateVertexBuffer(vertices);
}

BVulkanModel::~BVulkanModel() {
    device_->Device().waitIdle();
    device_->Device().destroyBuffer(vertex_buffer_);
    device_->Device().freeMemory(vertex_buffer_memory_);
}

void BVulkanModel::Bind(vk::CommandBuffer& command_buffer) const {
    std::array<vk::Buffer, 1> buffers{vertex_buffer_};
    command_buffer.bindVertexBuffers(0, buffers, {0});
}

void BVulkanModel::Draw(vk::CommandBuffer& command_buffer) const {
    command_buffer.draw(vertex_count_, 1, 0, 0);
}

void BVulkanModel::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    vertex_count_ = static_cast<uint32_t>(vertices.size());
    vk::DeviceSize buffer_size = sizeof(vertices[0]) * vertex_count_;
    vk::Buffer staging_buffer{};
    vk::DeviceMemory staging_buffer_memory{};
    device_->CreateBuffer(
        buffer_size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        staging_buffer,
        staging_buffer_memory);
    auto* data = device_->Device().mapMemory(staging_buffer_memory, 0, buffer_size);
    memcpy(data, vertices.data(), buffer_size);
    device_->Device().unmapMemory(staging_buffer_memory);
    device_->CreateBuffer(
        buffer_size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vertex_buffer_,
        vertex_buffer_memory_);
    device_->CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);
    device_->Device().destroyBuffer(staging_buffer);
    device_->Device().freeMemory(staging_buffer_memory);
}
