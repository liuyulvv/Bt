#pragma once

/**
 * @file BVulkanRenderSystem.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include <memory>

#include "BVulkanHeader.h"
#include "BVulkanModel.h"

class BVulkanDevice;
class BVulkanPipeline;

class BVulkanRenderSystem {
public:
    BVulkanRenderSystem(BVulkanDevice* device, const vk::RenderPass& render_pass);
    ~BVulkanRenderSystem();
    BVulkanRenderSystem(const BVulkanRenderSystem& system) = delete;
    BVulkanRenderSystem(BVulkanRenderSystem&& system) = delete;
    BVulkanRenderSystem& operator=(const BVulkanRenderSystem& system) = delete;
    BVulkanRenderSystem& operator=(BVulkanRenderSystem&& system) = delete;

public:
    void RenderObjects(vk::CommandBuffer& command_buffer, const std::vector<BVulkanModel>& models);

private:
    void CreatePipelineLayout();
    std::unique_ptr<BVulkanPipeline> CreatePipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, vk::PrimitiveTopology primitive_topology, const vk::RenderPass& render_pass);

private:
    BVulkanDevice* device_;
    vk::PipelineLayout pipeline_layout_{};
    std::unique_ptr<BVulkanPipeline> pipeline_{};
};