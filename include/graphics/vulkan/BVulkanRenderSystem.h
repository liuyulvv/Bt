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
    BVulkanRenderSystem(BVulkanDevice* device, const vk::RenderPass& renderPass);
    ~BVulkanRenderSystem();
    BVulkanRenderSystem(const BVulkanRenderSystem& system) = delete;
    BVulkanRenderSystem(BVulkanRenderSystem&& system) = delete;
    BVulkanRenderSystem& operator=(const BVulkanRenderSystem& system) = delete;
    BVulkanRenderSystem& operator=(BVulkanRenderSystem&& system) = delete;

public:
    void renderObjects(vk::CommandBuffer& commandBuffer, const std::vector<BVulkanModel>& models);

private:
    void createPipelineLayout();
    std::unique_ptr<BVulkanPipeline> createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, vk::PrimitiveTopology primitiveTopology, const vk::RenderPass& renderPass);

private:
    BVulkanDevice* m_device;
    vk::PipelineLayout m_pipelineLayout{};
    std::unique_ptr<BVulkanPipeline> m_pipelineLine{};
};