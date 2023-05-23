/**
 * @file BVulkanSwapchain.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanSwapchain.h"

#include "BVulkanDevice.h"

BVulkanSwapchain::BVulkanSwapchain(BVulkanDevice* device, int width, int height) : device_(device) {
    canvas_extent_.setWidth(width);
    canvas_extent_.setHeight(height);
    CreateSwapchain();
    CreateRenderPass();
    CreateDepthResources();
    CreateFrameBuffers();
    CreateSyncObjects();
}

BVulkanSwapchain::~BVulkanSwapchain() {
    for (auto image_view : swapchain_image_views_) {
        device_->Device().destroyImageView(image_view);
    }
    swapchain_image_views_.clear();
    if (swapchain_) {
        device_->Device().destroySwapchainKHR(swapchain_);
        swapchain_ = nullptr;
    }
    for (size_t i = 0; i < depth_images_.size(); ++i) {
        device_->Device().destroyImageView(depth_image_views_[i]);
        device_->Device().destroyImage(depth_images_[i]);
        device_->Device().freeMemory(depth_image_memories_[i]);
    }
    for (auto& framebuffer : swapchain_frame_buffers_) {
        device_->Device().destroyFramebuffer(framebuffer);
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        device_->Device().destroySemaphore(render_finished_semaphores_[i]);
        device_->Device().destroySemaphore(image_available_semaphores_[i]);
        device_->Device().destroyFence(in_flight_fences_[i]);
    }
    device_->Device().destroyRenderPass(render_pass_);
}

vk::Extent2D BVulkanSwapchain::GetSwapchainExtent() const {
    return swapchain_extent_;
}

size_t BVulkanSwapchain::GetImageCount() const {
    return swapchain_images_.size();
}

const vk::RenderPass& BVulkanSwapchain::GetRenderPass() const {
    return render_pass_;
}

float BVulkanSwapchain::GetExtentAspectRatio() const {
    return static_cast<float>(swapchain_extent_.width) / static_cast<float>(swapchain_extent_.height);
}

uint32_t BVulkanSwapchain::AcquireNextImage() {
    [[maybe_unused]] auto res = device_->Device().waitForFences(in_flight_fences_[current_frame_], true, (std::numeric_limits<uint64_t>::max)());
    return device_->Device().acquireNextImageKHR(swapchain_, (std::numeric_limits<uint64_t>::max)(), image_available_semaphores_[current_frame_], nullptr).value;
}

void BVulkanSwapchain::SubmitCommandBuffers(const vk::CommandBuffer& buffer, uint32_t image_index) {
    if (images_in_flight_[image_index]) {
        [[maybe_unused]] auto res = device_->Device().waitForFences(images_in_flight_[image_index], true, (std::numeric_limits<uint64_t>::max)());
    }
    images_in_flight_[image_index] = in_flight_fences_[current_frame_];

    vk::SubmitInfo submit_info;
    vk::PipelineStageFlags wait_dst_stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit_info
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(image_available_semaphores_[current_frame_])
        .setWaitDstStageMask(wait_dst_stage_mask)
        .setCommandBufferCount(1)
        .setCommandBuffers(buffer)
        .setSignalSemaphoreCount(1)
        .setSignalSemaphores(render_finished_semaphores_[current_frame_]);

    device_->Device().resetFences(in_flight_fences_[current_frame_]);

    device_->GetGraphicsQueue().submit(submit_info, in_flight_fences_[current_frame_]);

    vk::PresentInfoKHR present_info;
    present_info
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(render_finished_semaphores_[current_frame_])
        .setSwapchainCount(1)
        .setSwapchains(swapchain_)
        .setImageIndices(image_index);

    [[maybe_unused]] auto res = device_->GetPresentQueue().presentKHR(present_info);
    current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

const vk::Framebuffer& BVulkanSwapchain::GetFrameBuffer(size_t index) const {
    return swapchain_frame_buffers_[index];
}

void BVulkanSwapchain::CreateSwapchain() {
    auto swapchain_support = device_->GetSwapchainSupport();
    auto surface_format = ChooseSwapSurfaceFormat(swapchain_support.formats_);
    swapchain_image_format_ = surface_format.format;
    auto present_mode = ChooseSwapPresentMode(swapchain_support.present_modes_);
    auto extent = ChooseSwapExtent(swapchain_support.capabilities_);
    swapchain_extent_ = extent;
    uint32_t image_count = swapchain_support.capabilities_.minImageCount + 1;
    if (swapchain_support.capabilities_.maxImageCount > 0 && image_count > swapchain_support.capabilities_.maxImageCount) {
        image_count = swapchain_support.capabilities_.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR create_info{};
    create_info
        .setSurface(device_->Surface())
        .setMinImageCount(image_count)
        .setImageFormat(surface_format.format)
        .setImageColorSpace(surface_format.colorSpace)
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(swapchain_support.capabilities_.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(present_mode)
        .setClipped(true);
    auto indices = device_->FindPhysicalQueueFamilies();
    if (indices.graphics_family_ != indices.present_family_) {
        std::array<uint32_t, 2> queue_family_indices{indices.graphics_family_, indices.present_family_};
        create_info
            .setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(queue_family_indices);
    } else {
        create_info
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndices(indices.graphics_family_);
    }
    swapchain_ = device_->Device().createSwapchainKHR(create_info);
    swapchain_images_ = device_->Device().getSwapchainImagesKHR(swapchain_);
    swapchain_image_views_.resize(swapchain_images_.size());
    for (size_t i = 0; i < swapchain_images_.size(); ++i) {
        swapchain_image_views_[i] = device_->CreateImageView(swapchain_images_[i], swapchain_image_format_, vk::ImageAspectFlagBits::eColor);
    }
}

void BVulkanSwapchain::CreateRenderPass() {
    vk::AttachmentDescription depth_attachment{};
    depth_attachment
        .setFormat(FindDepthFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference depth_attachment_reference;
    depth_attachment_reference
        .setAttachment(1)
        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentDescription color_attachment;
    color_attachment
        .setFormat(GetSwapchainImageFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass;
    subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setColorAttachments(color_attachment_reference)
        .setPDepthStencilAttachment(&depth_attachment_reference);
    vk::SubpassDependency dependency;
    dependency
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstSubpass(0)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    std::array<vk::AttachmentDescription, 2> attachments{color_attachment, depth_attachment};
    vk::RenderPassCreateInfo render_pass_info;
    render_pass_info
        .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
        .setAttachments(attachments)
        .setSubpassCount(1)
        .setSubpasses(subpass)
        .setDependencyCount(1)
        .setDependencies(dependency);
    render_pass_ = device_->Device().createRenderPass(render_pass_info);
}

void BVulkanSwapchain::CreateDepthResources() {
    auto depth_format = FindDepthFormat();
    auto swapchain_extent = GetSwapchainExtent();
    depth_images_.resize(GetImageCount());
    depth_image_memories_.resize(GetImageCount());
    depth_image_views_.resize(GetImageCount());
    for (size_t i = 0; i < depth_images_.size(); ++i) {
        device_->CreateImage(swapchain_extent.width, swapchain_extent.height, depth_format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depth_images_[i], depth_image_memories_[i]);
        depth_image_views_[i] = device_->CreateImageView(depth_images_[i], depth_format, vk::ImageAspectFlagBits::eDepth);
    }
}

void BVulkanSwapchain::CreateFrameBuffers() {
    swapchain_frame_buffers_.resize(GetImageCount());
    for (size_t i = 0; i < GetImageCount(); ++i) {
        std::array<vk::ImageView, 2> attachments{swapchain_image_views_[i], depth_image_views_[i]};
        auto swapchain_extent = GetSwapchainExtent();
        vk::FramebufferCreateInfo framebuffer_info{};
        framebuffer_info
            .setRenderPass(render_pass_)
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setAttachments(attachments)
            .setWidth(swapchain_extent.width)
            .setHeight(swapchain_extent.height)
            .setLayers(1);
        swapchain_frame_buffers_[i] = device_->Device().createFramebuffer(framebuffer_info);
    }
}

void BVulkanSwapchain::CreateSyncObjects() {
    image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
    images_in_flight_.resize(GetImageCount());
    vk::SemaphoreCreateInfo semaphore_info{};
    vk::FenceCreateInfo fence_info{};
    fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        image_available_semaphores_[i] = device_->Device().createSemaphore(semaphore_info);
        render_finished_semaphores_[i] = device_->Device().createSemaphore(semaphore_info);
        in_flight_fences_[i] = device_->Device().createFence(fence_info);
    }
}

vk::SurfaceFormatKHR BVulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    for (const auto& available_format : availableFormats) {
        if (available_format.format == vk::Format::eR8G8B8A8Srgb && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return available_format;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR BVulkanSwapchain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    for (const auto& available_present_mode : availablePresentModes) {
        if (available_present_mode == vk::PresentModeKHR::eMailbox) {
            return available_present_mode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D BVulkanSwapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    }
    vk::Extent2D actual_extent = canvas_extent_;
    actual_extent.width = (std::max)(capabilities.minImageExtent.width, (std::min)(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = (std::max)(capabilities.minImageExtent.height, (std::min)(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
}

vk::Format BVulkanSwapchain::FindDepthFormat() const {
    return device_->FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

const vk::Format& BVulkanSwapchain::GetSwapchainImageFormat() const {
    return swapchain_image_format_;
}
