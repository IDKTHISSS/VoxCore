//
// Created by IDKTHIS on 14.07.2025.
//

#include "VulkanSwapChain.h"

#include "Platform/Renderer/Vulkan/RenderPass/VulkanRenderPass.h"

VulkanSwapChain::VulkanSwapChain(VulkanInstance *instance, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice) :
    m_vulkanInstance(instance), m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice) {

}

VulkanSwapChain::~VulkanSwapChain() {
    m_swapchain = nullptr;
}

bool VulkanSwapChain::Init(vk::SurfaceKHR surface) {
    auto formats = m_physicalDevice->GetHandle().getSurfaceFormatsKHR(surface);
    auto presentModes = m_physicalDevice->GetHandle().getSurfacePresentModesKHR(surface);
    auto surfaceCaps = m_physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);
    m_surfaceFormat = formats[0];
    vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
    m_swapExtent = surfaceCaps.currentExtent;

    // Swapchain
    vk::SwapchainCreateInfoKHR swapchainInfo({}, surface, 2, m_surfaceFormat.format,
        m_surfaceFormat.colorSpace, m_swapExtent, 1, vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eExclusive, 0, nullptr, surfaceCaps.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode);
    m_swapchain = m_logicalDevice->GetHandle().createSwapchainKHR(swapchainInfo);
    auto images = m_logicalDevice->GetHandle().getSwapchainImagesKHR(m_swapchain);

    m_imageViews.clear();
    for (auto img : images) {
        vk::ImageViewCreateInfo viewInfo({}, img, vk::ImageViewType::e2D, m_surfaceFormat.format,
            {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        m_imageViews.push_back(m_logicalDevice->GetHandle().createImageView(viewInfo));
    }
    return true;
}

bool VulkanSwapChain::CreateFramebuffers(VulkanRenderPass* renderPass) {
    for (auto& view : m_imageViews) {
        vk::FramebufferCreateInfo fbInfo({}, renderPass->GetHandle(), 1, &view, m_swapExtent.width, m_swapExtent.height, 1);
        m_framebuffers.push_back(m_logicalDevice->GetHandle().createFramebuffer(fbInfo));
    }
    return true;
}
