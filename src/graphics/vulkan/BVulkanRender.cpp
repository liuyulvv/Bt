/**
 * @file BVulkanRender.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanRender.h"

#include "BGraphicsCanvas.h"
#include "BVulkanDevice.h"
#include "BVulkanSwapchain.h"

BVulkanRender::BVulkanRender(BVulkanDevice* device, BGraphicsCanvas* canvas) : device_(device), canvas_(canvas) {
    RecreateSwapchain();
    CreateCommandBuffers();
}

BVulkanRender::~BVulkanRender() {
    device_->Device().waitIdle();
    swapchain_.reset();
    FreeCommandBuffers();
}

const vk::RenderPass& BVulkanRender::GetSwapchainRenderPass() const {
    return swapchain_->GetRenderPass();
}

float BVulkanRender::GetAspectRatio() const {
    return swapchain_->GetExtentAspectRatio();
}

vk::CommandBuffer BVulkanRender::BeginFrame() {
    try {
        current_image_index_ = swapchain_->AcquireNextImage();
        is_frame_started_ = true;
        auto command_buffer = GetCurrentCommandBuffer();
        vk::CommandBufferBeginInfo begin_info{};
        command_buffer.begin(begin_info);
        return command_buffer;
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        RecreateSwapchain();
        return nullptr;
    }
}

void BVulkanRender::EndFrame() {
    try {
        auto command_buffer = GetCurrentCommandBuffer();
        command_buffer.end();
        swapchain_->SubmitCommandBuffers(command_buffer, current_image_index_);
        is_frame_started_ = false;
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        RecreateSwapchain();
    }
}

void BVulkanRender::BeginSwapchainRenderPass(vk::CommandBuffer command_buffer) {
    vk::RenderPassBeginInfo render_pass_info{};
    render_pass_info
        .setRenderPass(swapchain_->GetRenderPass())
        .setFramebuffer(swapchain_->GetFrameBuffer(current_image_index_));
    render_pass_info.renderArea
        .setOffset({0, 0})
        .setExtent(swapchain_->GetSwapchainExtent());
    std::array<vk::ClearValue, 2> clear_values{};
    clear_values[0].setColor({0.17F, 0.17F, 0.17F, 1.0F});
    clear_values[1].setDepthStencil({1.0F, 0});
    render_pass_info
        .setClearValueCount(static_cast<uint32_t>(clear_values.size()))
        .setClearValues(clear_values);
    command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
    vk::Viewport viewport{};
    viewport
        .setX(0.0F)
        .setY(0.0F)
        .setWidth(static_cast<float>(swapchain_->GetSwapchainExtent().width))
        .setHeight(static_cast<float>(swapchain_->GetSwapchainExtent().height))
        .setMinDepth(0.0F)
        .setMaxDepth(1.0F);
    vk::Rect2D scissor{{0, 0}, swapchain_->GetSwapchainExtent()};
    command_buffer.setViewport(0, viewport);
    command_buffer.setScissor(0, scissor);
}

void BVulkanRender::EndSwapchainRenderPass(vk::CommandBuffer command_buffer) {
    command_buffer.endRenderPass();
}

void BVulkanRender::RecreateSwapchain() {
    device_->Device().waitIdle();
    swapchain_.reset(nullptr);
    swapchain_ = std::make_unique<BVulkanSwapchain>(device_, canvas_->Width(), canvas_->Height());
}

void BVulkanRender::CreateCommandBuffers() {
    command_buffers_.resize(swapchain_->GetImageCount());
    vk::CommandBufferAllocateInfo alloc_info{};
    alloc_info
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(device_->GetCommandPool())
        .setCommandBufferCount(static_cast<uint32_t>(command_buffers_.size()));
    command_buffers_ = device_->Device().allocateCommandBuffers(alloc_info);
}

void BVulkanRender::FreeCommandBuffers() {
    device_->Device().freeCommandBuffers(device_->GetCommandPool(), command_buffers_);
}

bool BVulkanRender::IsFrameInProgress() const {
    return false;
}

vk::CommandBuffer BVulkanRender::GetCurrentCommandBuffer() const {
    return command_buffers_[current_image_index_];
}
