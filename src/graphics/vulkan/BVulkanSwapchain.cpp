/**
 * @file BVulkanSwapchain.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BVulkanSwapchain.h"

#include "BVulkanDevice.h"

BVulkanSwapchain::BVulkanSwapchain(BVulkanDevice* device, int width, int height) : m_device(device) {
    m_canvasExtent.setWidth(width);
    m_canvasExtent.setHeight(height);
    createSwapchain();
    createRenderPass();
    createDepthResources();
    createFrameBuffers();
    createSyncObjects();
}

BVulkanSwapchain::~BVulkanSwapchain() {
    for (auto imageView : m_swapchainImageViews) {
        m_device->device().destroyImageView(imageView);
    }
    m_swapchainImageViews.clear();
    if (m_swapchain) {
        m_device->device().destroySwapchainKHR(m_swapchain);
        m_swapchain = nullptr;
    }
    for (size_t i = 0; i < m_depthImages.size(); ++i) {
        m_device->device().destroyImageView(m_depthImageViews[i]);
        m_device->device().destroyImage(m_depthImages[i]);
        m_device->device().freeMemory(m_depthImageMemories[i]);
    }
    for (auto& framebuffer : m_swapchainFrameBuffers) {
        m_device->device().destroyFramebuffer(framebuffer);
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        m_device->device().destroySemaphore(m_renderFinishedSemaphores[i]);
        m_device->device().destroySemaphore(m_imageAvailableSemaphores[i]);
        m_device->device().destroyFence(m_inFlightFences[i]);
    }
    m_device->device().destroyRenderPass(m_renderPass);
}

vk::Extent2D BVulkanSwapchain::getSwapchainExtent() const {
    return m_swapchainExtent;
}

size_t BVulkanSwapchain::getImageCount() const {
    return m_swapchainImages.size();
}

const vk::RenderPass& BVulkanSwapchain::getRenderPass() const {
    return m_renderPass;
}

float BVulkanSwapchain::getExtentAspectRatio() const {
    return static_cast<float>(m_swapchainExtent.width) / static_cast<float>(m_swapchainExtent.height);
}

uint32_t BVulkanSwapchain::acquireNextImage() {
    [[maybe_unused]] auto res = m_device->device().waitForFences(m_inFlightFences[m_currentFrame], true, (std::numeric_limits<uint64_t>::max)());
    return m_device->device().acquireNextImageKHR(m_swapchain, (std::numeric_limits<uint64_t>::max)(), m_imageAvailableSemaphores[m_currentFrame], nullptr).value;
}
void BVulkanSwapchain::submitCommandBuffers(const vk::CommandBuffer& buffer, uint32_t imageIndex) {
    if (m_imagesInFlight[imageIndex]) {
        [[maybe_unused]] auto res = m_device->device().waitForFences(m_imagesInFlight[imageIndex], true, (std::numeric_limits<uint64_t>::max)());
    }
    m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

    vk::SubmitInfo submitInfo;
    vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(m_imageAvailableSemaphores[m_currentFrame])
        .setWaitDstStageMask(waitDstStageMask)
        .setCommandBufferCount(1)
        .setCommandBuffers(buffer)
        .setSignalSemaphoreCount(1)
        .setSignalSemaphores(m_renderFinishedSemaphores[m_currentFrame]);

    m_device->device().resetFences(m_inFlightFences[m_currentFrame]);

    m_device->getGraphicsQueue().submit(submitInfo, m_inFlightFences[m_currentFrame]);

    vk::PresentInfoKHR presentInfo;
    presentInfo
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(m_renderFinishedSemaphores[m_currentFrame])
        .setSwapchainCount(1)
        .setSwapchains(m_swapchain)
        .setImageIndices(imageIndex);

    [[maybe_unused]] auto res = m_device->getPresentQueue().presentKHR(presentInfo);
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

const vk::Framebuffer& BVulkanSwapchain::getFrameBuffer(size_t index) const {
    return m_swapchainFrameBuffers[index];
}

void BVulkanSwapchain::createSwapchain() {
    auto swapchainSupport = m_device->getSwapchainSupport();
    auto surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    m_swapchainImageFormat = surfaceFormat.format;
    auto presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
    auto extent = chooseSwapExtent(swapchainSupport.capabilities);
    m_swapchainExtent = extent;
    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo
        .setSurface(m_device->surface())
        .setMinImageCount(imageCount)
        .setImageFormat(surfaceFormat.format)
        .setImageColorSpace(surfaceFormat.colorSpace)
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(swapchainSupport.capabilities.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(presentMode)
        .setClipped(true);
    auto indices = m_device->findPhysicalQueueFamilies();
    if (indices.graphicsFamily != indices.presentFamily) {
        std::array<uint32_t, 2> queueFamilyIndices{indices.graphicsFamily, indices.presentFamily};
        createInfo
            .setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(queueFamilyIndices);
    } else {
        createInfo
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndices(indices.graphicsFamily);
    }
    m_swapchain = m_device->device().createSwapchainKHR(createInfo);
    m_swapchainImages = m_device->device().getSwapchainImagesKHR(m_swapchain);
    m_swapchainImageViews.resize(m_swapchainImages.size());
    for (size_t i = 0; i < m_swapchainImages.size(); ++i) {
        m_swapchainImageViews[i] = m_device->createImageView(m_swapchainImages[i], m_swapchainImageFormat, vk::ImageAspectFlagBits::eColor);
    }
}

void BVulkanSwapchain::createRenderPass() {
    vk::AttachmentDescription depthAttachment{};
    depthAttachment
        .setFormat(findDepthFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference depthAttachmentReference;
    depthAttachmentReference
        .setAttachment(1)
        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentDescription colorAttachment;
    colorAttachment
        .setFormat(getSwapchainImageFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference colorAttachmentReference;
    colorAttachmentReference
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass;
    subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setColorAttachments(colorAttachmentReference)
        .setPDepthStencilAttachment(&depthAttachmentReference);
    vk::SubpassDependency dependency;
    dependency
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstSubpass(0)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    std::array<vk::AttachmentDescription, 2> attachments{colorAttachment, depthAttachment};
    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo
        .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
        .setAttachments(attachments)
        .setSubpassCount(1)
        .setSubpasses(subpass)
        .setDependencyCount(1)
        .setDependencies(dependency);
    m_renderPass = m_device->device().createRenderPass(renderPassInfo);
}

void BVulkanSwapchain::createDepthResources() {
    auto depthFormat = findDepthFormat();
    auto swapChainExtent = getSwapchainExtent();
    m_depthImages.resize(getImageCount());
    m_depthImageMemories.resize(getImageCount());
    m_depthImageViews.resize(getImageCount());
    for (size_t i = 0; i < m_depthImages.size(); ++i) {
        m_device->createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, m_depthImages[i], m_depthImageMemories[i]);
        m_depthImageViews[i] = m_device->createImageView(m_depthImages[i], depthFormat, vk::ImageAspectFlagBits::eDepth);
    }
}

void BVulkanSwapchain::createFrameBuffers() {
    m_swapchainFrameBuffers.resize(getImageCount());
    for (size_t i = 0; i < getImageCount(); ++i) {
        std::array<vk::ImageView, 2> attachments{m_swapchainImageViews[i], m_depthImageViews[i]};
        auto swapChainExtent = getSwapchainExtent();
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo
            .setRenderPass(m_renderPass)
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setAttachments(attachments)
            .setWidth(swapChainExtent.width)
            .setHeight(swapChainExtent.height)
            .setLayers(1);
        m_swapchainFrameBuffers[i] = m_device->device().createFramebuffer(framebufferInfo);
    }
}

void BVulkanSwapchain::createSyncObjects() {
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    m_imagesInFlight.resize(getImageCount());
    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        m_imageAvailableSemaphores[i] = m_device->device().createSemaphore(semaphoreInfo);
        m_renderFinishedSemaphores[i] = m_device->device().createSemaphore(semaphoreInfo);
        m_inFlightFences[i] = m_device->device().createFence(fenceInfo);
    }
}

vk::SurfaceFormatKHR BVulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR BVulkanSwapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D BVulkanSwapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    }
    vk::Extent2D actualExtent = m_canvasExtent;
    actualExtent.width = (std::max)(capabilities.minImageExtent.width, (std::min)(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = (std::max)(capabilities.minImageExtent.height, (std::min)(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
}

vk::Format BVulkanSwapchain::findDepthFormat() const {
    return m_device->findSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

const vk::Format& BVulkanSwapchain::getSwapchainImageFormat() const {
    return m_swapchainImageFormat;
}
