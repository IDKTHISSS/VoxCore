//
// Created by IDKTHIS on 14.07.2025.
//

#include "VulkanSwapChain.h"

#include "Platform/Renderer/Vulkan/RenderPass/VulkanRenderPass.h"

VulkanSwapChain::VulkanSwapChain(VulkanInstance *instance, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice) :
    m_vulkanInstance(instance), m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice) {

}

VulkanSwapChain::~VulkanSwapChain() {
    for (auto framebuffer : m_framebuffers) {
        if (framebuffer) {
            m_logicalDevice->GetHandle().destroyFramebuffer(framebuffer);
        }
    }
    m_framebuffers.clear();

    for (auto imageView : m_imageViews) {
        if (imageView) {
            m_logicalDevice->GetHandle().destroyImageView(imageView);
        }
    }
    m_imageViews.clear();

    if (m_swapchain) {
        m_logicalDevice->GetHandle().destroySwapchainKHR(m_swapchain);
    }
}

bool VulkanSwapChain::Init(vk::SurfaceKHR surface) {
    auto formats = m_physicalDevice->GetHandle().getSurfaceFormatsKHR(surface);
    auto presentModes = m_physicalDevice->GetHandle().getSurfacePresentModesKHR(surface);
    auto surfaceCaps = m_physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);
    m_surfaceFormat = formats[0];
    vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
    for (auto pm : presentModes) {
        if (pm == vk::PresentModeKHR::eMailbox) {
            presentMode = vk::PresentModeKHR::eMailbox;
            break;
        }
    }
    m_swapExtent = surfaceCaps.currentExtent;

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
    vk::ImageCreateInfo depthImgInfo({}, vk::ImageType::e2D, m_depthFormat,
        vk::Extent3D{m_swapExtent.width, m_swapExtent.height, 1}, 1, 1, vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment);
    m_depthImage = m_logicalDevice->GetHandle().createImage(depthImgInfo);
    vk::MemoryRequirements memReq = m_logicalDevice->GetHandle().getImageMemoryRequirements(m_depthImage);
    uint32_t memTypeIndex = m_physicalDevice->GetHandle().getMemoryProperties().memoryTypeCount > 0 ? 0 : 0;
    auto memProps = m_physicalDevice->GetHandle().getMemoryProperties();
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((memReq.memoryTypeBits & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)) {
            memTypeIndex = i;
            break;
        }
    }
    vk::MemoryAllocateInfo allocInfo(memReq.size, memTypeIndex);
    m_depthImageMemory = m_logicalDevice->GetHandle().allocateMemory(allocInfo);
    m_logicalDevice->GetHandle().bindImageMemory(m_depthImage, m_depthImageMemory, 0);
    vk::ImageViewCreateInfo depthViewInfo({}, m_depthImage, vk::ImageViewType::e2D, m_depthFormat,
        {}, {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});
    m_depthImageView = m_logicalDevice->GetHandle().createImageView(depthViewInfo);
    return true;
}

bool VulkanSwapChain::CreateFramebuffers(VulkanRenderPass* renderPass) {
    m_framebuffers.clear();
    for (auto& view : m_imageViews) {
        std::array<vk::ImageView, 2> attachments = { view, m_depthImageView };
        vk::FramebufferCreateInfo fbInfo({}, renderPass->GetHandle(), attachments.size(), attachments.data(), m_swapExtent.width, m_swapExtent.height, 1);
        m_framebuffers.push_back(m_logicalDevice->GetHandle().createFramebuffer(fbInfo));
    }
    return true;
}
