#pragma once

/**
 * @file BVulkanPipeline.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include <string>
#include <vector>

#include "BVulkanHeader.h"

class BVulkanDevice;

class BVulkanPipeline {
public:
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;

        vk::PipelineViewportStateCreateInfo viewportInfo{};
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
        vk::PipelineMultisampleStateCreateInfo multisampleInfo{};
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
        std::vector<vk::DynamicState> dynamicStates{};
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        vk::PipelineLayout pipelineLayout{nullptr};
        vk::RenderPass renderPass{nullptr};
        uint32_t subpass{0};
    };

public:
    BVulkanPipeline(BVulkanDevice* device, const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config);
    ~BVulkanPipeline();
    BVulkanPipeline(const BVulkanPipeline& pipeline) = delete;
    BVulkanPipeline(BVulkanPipeline&& pipeline) = delete;
    BVulkanPipeline& operator=(const BVulkanPipeline& pipeline) = delete;
    BVulkanPipeline& operator=(BVulkanPipeline&& pipeline) = delete;

public:
    static PipelineConfigInfo defaultPipelineConfigInfo(vk::PrimitiveTopology primitiveTopology = vk::PrimitiveTopology::eTriangleList);
    void bind(const vk::CommandBuffer& buffer);

private:
    void createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config);
    static std::vector<char> readFile(const std::string& path);
    vk::ShaderModule createShaderModule(const std::vector<char>& code);

private:
    BVulkanDevice* m_device;
    vk::Pipeline m_graphicsPipeline{};
    vk::ShaderModule vertShaderModule{};
    vk::ShaderModule fragShaderModule{};
};