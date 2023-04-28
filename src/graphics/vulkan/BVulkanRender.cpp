/**
 * @file BVulkanRender.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanRender.h"

#include "BGraphicsCanvas.h"
#include "BVulkanDevice.h"
#include "BVulkanSwapchain.h"

BVulkanRender::BVulkanRender(BVulkanDevice* device, BGraphicsCanvas* canvas) : m_device(device), m_canvas(canvas) {
    recreateSwapchain();
    createCommandBuffers();
}

BVulkanRender::~BVulkanRender() {
    m_device->device().waitIdle();
    m_swapchain.reset();
    freeCommandBuffers();
}

const vk::RenderPass& BVulkanRender::getSwapchainRenderPass() const {
    return m_swapchain->getRenderPass();
}

float BVulkanRender::getAspectRatio() const {
    return m_swapchain->getExtentAspectRatio();
}

vk::CommandBuffer BVulkanRender::beginFrame() {
    try {
        m_currentImageIndex = m_swapchain->acquireNextImage();
        m_isFrameStarted = true;
        auto commandBuffer = getCurrentCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo{};
        commandBuffer.begin(beginInfo);
        return commandBuffer;
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        recreateSwapchain();
        return nullptr;
    }
}
void BVulkanRender::endFrame() {
    try {
        auto commandBuffer = getCurrentCommandBuffer();
        commandBuffer.end();
        m_swapchain->submitCommandBuffers(commandBuffer, m_currentImageIndex);
        m_isFrameStarted = false;
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        recreateSwapchain();
    }
}

void BVulkanRender::beginSwapchainRenderPass(vk::CommandBuffer commandBuffer) {
    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo
        .setRenderPass(m_swapchain->getRenderPass())
        .setFramebuffer(m_swapchain->getFrameBuffer(m_currentImageIndex));
    renderPassInfo.renderArea
        .setOffset({0, 0})
        .setExtent(m_swapchain->getSwapchainExtent());
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].setColor({0.17F, 0.17F, 0.17F, 1.0F});
    clearValues[1].setDepthStencil({1.0F, 0});
    renderPassInfo
        .setClearValueCount(static_cast<uint32_t>(clearValues.size()))
        .setClearValues(clearValues);
    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    vk::Viewport viewport{};
    viewport
        .setX(0.0F)
        .setY(0.0F)
        .setWidth(static_cast<float>(m_swapchain->getSwapchainExtent().width))
        .setHeight(static_cast<float>(m_swapchain->getSwapchainExtent().height))
        .setMinDepth(0.0F)
        .setMaxDepth(1.0F);
    vk::Rect2D scissor{{0, 0}, m_swapchain->getSwapchainExtent()};
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, scissor);
}
void BVulkanRender::endSwapchainRenderPass(vk::CommandBuffer commandBuffer) {
    commandBuffer.endRenderPass();
}

void BVulkanRender::recreateSwapchain() {
    m_device->device().waitIdle();
    m_swapchain.reset(nullptr);
    m_swapchain = std::make_unique<BVulkanSwapchain>(m_device, m_canvas->width(), m_canvas->height());
}

void BVulkanRender::createCommandBuffers() {
    m_commandBuffers.resize(m_swapchain->getImageCount());
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(m_device->getCommandPool())
        .setCommandBufferCount(static_cast<uint32_t>(m_commandBuffers.size()));
    m_commandBuffers = m_device->device().allocateCommandBuffers(allocInfo);
}

void BVulkanRender::freeCommandBuffers() {
    m_device->device().freeCommandBuffers(m_device->getCommandPool(), m_commandBuffers);
}

bool BVulkanRender::isFrameInProgress() const {
    return false;
}

vk::CommandBuffer BVulkanRender::getCurrentCommandBuffer() const {
    return m_commandBuffers[m_currentImageIndex];
}
