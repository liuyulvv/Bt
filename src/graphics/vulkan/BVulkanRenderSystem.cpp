/**
 * @file BVulkanRenderSystem.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanRenderSystem.h"

#include "BVulkanDevice.h"
#include "BVulkanPipeline.h"

BVulkanRenderSystem::BVulkanRenderSystem(BVulkanDevice* device, const vk::RenderPass& render_pass) : device_(device) {
    CreatePipelineLayout();
    pipeline_ = CreatePipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv", vk::PrimitiveTopology::eTriangleList, render_pass);
}

BVulkanRenderSystem::~BVulkanRenderSystem() {
    device_->Device().destroyPipelineLayout(pipeline_layout_);
    pipeline_.reset();
}

void BVulkanRenderSystem::RenderObjects(vk::CommandBuffer& command_buffer, const std::vector<BVulkanModel>& models) {
    pipeline_->Bind(command_buffer);
    for (auto& model : models) {
        model.Bind(command_buffer);
        model.Draw(command_buffer);
    }
}

void BVulkanRenderSystem::CreatePipelineLayout() {
    vk::PipelineLayoutCreateInfo pipeline_info{};
    pipeline_info
        .setSetLayoutCount(0)
        .setPSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPushConstantRanges(nullptr);
    pipeline_layout_ = device_->Device().createPipelineLayout(pipeline_info);
}

std::unique_ptr<BVulkanPipeline> BVulkanRenderSystem::CreatePipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, vk::PrimitiveTopology primitive_topology, const vk::RenderPass& render_pass) {
    auto pipeline_config = BVulkanPipeline::DefaultPipelineConfigInfo(primitive_topology);
    pipeline_config.render_pass_ = render_pass;
    pipeline_config.pipeline_layout_ = pipeline_layout_;
    return std::make_unique<BVulkanPipeline>(device_, vert_shader_path, frag_shader_path, pipeline_config);
}
