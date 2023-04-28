/**
 * @file BVulkanRenderSystem.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanRenderSystem.h"

#include "BVulkanDevice.h"
#include "BVulkanPipeline.h"

BVulkanRenderSystem::BVulkanRenderSystem(BVulkanDevice* device, const vk::RenderPass& renderPass) : m_device(device) {
    createPipelineLayout();
    m_pipelineLine = createPipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv", vk::PrimitiveTopology::eTriangleList, renderPass);
}

BVulkanRenderSystem::~BVulkanRenderSystem() {
    m_device->device().destroyPipelineLayout(m_pipelineLayout);
    m_pipelineLine.reset();
}

void BVulkanRenderSystem::renderObjects(vk::CommandBuffer& commandBuffer, const std::vector<BVulkanModel>& models) {
    m_pipelineLine->bind(commandBuffer);
    for (auto& model : models) {
        model.bind(commandBuffer);
        model.draw(commandBuffer);
    }
}

void BVulkanRenderSystem::createPipelineLayout() {
    vk::PipelineLayoutCreateInfo pipelineInfo{};
    pipelineInfo
        .setSetLayoutCount(0)
        .setPSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPushConstantRanges(nullptr);
    m_pipelineLayout = m_device->device().createPipelineLayout(pipelineInfo);
}

std::unique_ptr<BVulkanPipeline> BVulkanRenderSystem::createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, vk::PrimitiveTopology primitiveTopology, const vk::RenderPass& renderPass) {
    auto pipelineConfig = BVulkanPipeline::defaultPipelineConfigInfo(primitiveTopology);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    return std::make_unique<BVulkanPipeline>(m_device, vertShaderPath, fragShaderPath, pipelineConfig);
}
