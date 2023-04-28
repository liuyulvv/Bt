/**
 * @file BVulkanPipeline.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanPipeline.h"

#include <fstream>
#include <stdexcept>

#include "BVulkanDevice.h"
#include "BVulkanModel.h"

BVulkanPipeline::BVulkanPipeline(BVulkanDevice* device, const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config) : m_device(device) {
    createGraphicsPipeline(vertShaderPath, fragShaderPath, config);
}

BVulkanPipeline::~BVulkanPipeline() {
    m_device->device().destroyShaderModule(vertShaderModule);
    m_device->device().destroyShaderModule(fragShaderModule);
    m_device->device().destroyPipeline(m_graphicsPipeline);
}

BVulkanPipeline::PipelineConfigInfo BVulkanPipeline::defaultPipelineConfigInfo(vk::PrimitiveTopology primitiveTopology) {
    PipelineConfigInfo config{};
    config.viewportInfo
        .setViewportCount(1)
        .setPViewports(nullptr)
        .setScissorCount(1)
        .setPScissors(nullptr);
    config.inputAssemblyInfo
        .setTopology(primitiveTopology)
        .setPrimitiveRestartEnable(false);
    config.rasterizationInfo
        .setDepthClampEnable(false)
        .setRasterizerDiscardEnable(false)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1.0F)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setDepthBiasEnable(false)
        .setDepthBiasConstantFactor(0.0F)
        .setDepthBiasClamp(0.0F)
        .setDepthBiasSlopeFactor(0.0F);
    config.multisampleInfo
        .setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.0F)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(false)
        .setAlphaToOneEnable(false);
    config.colorBlendAttachment
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
        .setBlendEnable(false)
        .setSrcColorBlendFactor(vk::BlendFactor::eOne)
        .setDstColorBlendFactor(vk::BlendFactor::eZero)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd);
    config.colorBlendInfo
        .setLogicOpEnable(false)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachmentCount(1)
        .setAttachments(config.colorBlendAttachment)
        .setBlendConstants({0.0F, 0.0F, 0.0F, 0.0F});
    config.depthStencilInfo
        .setDepthTestEnable(true)
        .setDepthWriteEnable(true)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(false)
        .setMinDepthBounds(0.0F)
        .setMaxDepthBounds(1.0F)
        .setStencilTestEnable(false)
        .setFront({})
        .setBack({});
    config.dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    config.dynamicStateInfo
        .setDynamicStateCount(static_cast<uint32_t>(config.dynamicStates.size()))
        .setDynamicStates(config.dynamicStates);
    return config;
}

void BVulkanPipeline::bind(const vk::CommandBuffer& buffer) {
    buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline);
}

void BVulkanPipeline::createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config) {
    auto vertShaderCode = readFile(vertShaderPath);
    auto fragShaderCode = readFile(fragShaderPath);
    vertShaderModule = createShaderModule(vertShaderCode);
    fragShaderModule = createShaderModule(fragShaderCode);
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(vertShaderModule)
        .setPName("main");
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(fragShaderModule)
        .setPName("main");
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    auto bindingDescriptions = BVulkanModel::Vertex::getBindingDescriptions();
    auto attributeDescriptions = BVulkanModel::Vertex::getAttributeDescriptions();
    vertexInputInfo
        .setVertexBindingDescriptionCount(static_cast<uint32_t>(bindingDescriptions.size()))
        .setVertexBindingDescriptions(bindingDescriptions)
        .setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()))
        .setVertexAttributeDescriptions(attributeDescriptions);

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages{vertShaderStageInfo, fragShaderStageInfo};

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo
        .setStageCount(static_cast<uint32_t>(shaderStages.size()))
        .setStages(shaderStages)
        .setPVertexInputState(&vertexInputInfo)
        .setPInputAssemblyState(&config.inputAssemblyInfo)
        .setPViewportState(&config.viewportInfo)
        .setPRasterizationState(&config.rasterizationInfo)
        .setPMultisampleState(&config.multisampleInfo)
        .setPColorBlendState(&config.colorBlendInfo)
        .setPDepthStencilState(&config.depthStencilInfo)
        .setPDynamicState(&config.dynamicStateInfo)
        .setLayout(config.pipelineLayout)
        .setRenderPass(config.renderPass)
        .setSubpass(config.subpass)
        .setBasePipelineIndex(-1)
        .setBasePipelineHandle(nullptr);
    m_graphicsPipeline = m_device->device().createGraphicsPipeline(nullptr, pipelineInfo).value;
}

std::vector<char> BVulkanPipeline::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + ".");
    }
    auto fileSize = static_cast<std::streamsize>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

vk::ShaderModule BVulkanPipeline::createShaderModule(const std::vector<char>& code) {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(code.size());
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    return m_device->device().createShaderModule(createInfo);
}
