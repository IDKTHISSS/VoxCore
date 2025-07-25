//
// Created by IDKTHIS on 14.07.2025.
//

#include "VulkanSwapChain.h"

#include "Core/CVar/CVar.h"
#include "Core/Log/Logger.h"
#include "Platform/Renderer/Vulkan/RenderPass/VulkanRenderPass.h"



VulkanSwapChain::VulkanSwapChain(VulkanInstance *instance, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice) :
    m_vulkanInstance(instance), m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice) {

}

VulkanSwapChain::~VulkanSwapChain() {
    for (auto& view : m_imageViews) {
        m_logicalDevice->GetHandle().destroyImageView(view);
    }
    for (auto& fb : m_framebuffers) {
        m_logicalDevice->GetHandle().destroyFramebuffer(fb);
    }
    m_framebuffers.clear();
    if (m_depthImageView) {
        m_logicalDevice->GetHandle().destroyImageView(m_depthImageView);
        m_depthImageView = nullptr;
    }

    if (m_depthImage) {
        m_logicalDevice->GetHandle().destroyImage(m_depthImage);
        m_depthImage = nullptr;
    }

    if (m_depthImageMemory) {
        m_logicalDevice->GetHandle().freeMemory(m_depthImageMemory);
        m_depthImageMemory = nullptr;
    }
    m_logicalDevice->GetHandle().destroySwapchainKHR(m_swapchain);

}

bool VulkanSwapChain::Init(vk::SurfaceKHR surface) {
    auto formats = m_physicalDevice->GetHandle().getSurfaceFormatsKHR(surface);
    auto presentModes = m_physicalDevice->GetHandle().getSurfacePresentModesKHR(surface);
    auto surfaceCaps = m_physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);
    m_surfaceFormat = formats[0];
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            m_surfaceFormat = availableFormat;
            break;
            }
    }
    vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
    for (auto pm : presentModes) {
        if (pm == vk::PresentModeKHR::eMailbox) {
            presentMode = vk::PresentModeKHR::eMailbox;
            break;
        }
    }
    m_swapExtent = surfaceCaps.currentExtent;
    LOG_INFO("Vulkan", "Swapchain surface format: {}, present mode: {}, extent: {}x{}",
             vk::to_string(m_surfaceFormat.format), vk::to_string(presentMode), m_swapExtent.width, m_swapExtent.height);
    uint32_t imageCount = surfaceCaps.minImageCount + 1;
    if (surfaceCaps.maxImageCount > 0 && imageCount > surfaceCaps.maxImageCount) {
        imageCount = surfaceCaps.maxImageCount;
    }
    uint32_t graphicsQueueFamily = m_physicalDevice->GetGraphicsQueueFamilyIndex();
    uint32_t presentQueueFamily = m_physicalDevice->GetPresentQueueFamilyIndex();

    vk::SharingMode sharingMode;
    std::vector<uint32_t> queueFamilyIndices;

    if (graphicsQueueFamily != presentQueueFamily) {
        sharingMode = vk::SharingMode::eConcurrent;
        queueFamilyIndices = { graphicsQueueFamily, presentQueueFamily };
    } else {
        sharingMode = vk::SharingMode::eExclusive;
    }
    vk::SwapchainCreateInfoKHR swapchainInfo({}, surface, imageCount, m_surfaceFormat.format,
        m_surfaceFormat.colorSpace, m_swapExtent, 1, vk::ImageUsageFlagBits::eColorAttachment,
        sharingMode, static_cast<uint32_t>(queueFamilyIndices.size()),
        queueFamilyIndices.empty() ? nullptr : queueFamilyIndices.data(),
        surfaceCaps.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode);

    m_swapchain = m_logicalDevice->GetHandle().createSwapchainKHR(swapchainInfo);


    auto images = m_logicalDevice->GetHandle().getSwapchainImagesKHR(m_swapchain);
    m_imageViews.clear();
    m_imageViews.reserve(images.size());
    for (auto img : images) {
        vk::ImageViewCreateInfo viewInfo({}, img, vk::ImageViewType::e2D, m_surfaceFormat.format,
            {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        m_imageViews.push_back(m_logicalDevice->GetHandle().createImageView(viewInfo));
    }
    LOG_INFO("Vulkan",
         "Swapchain created: {} images, format={}, present mode={}, extent={}x{}, sharing={}, queue families: G={}, P={}",
         m_imageViews.size(),
         vk::to_string(m_surfaceFormat.format),
         vk::to_string(presentMode),
         m_swapExtent.width, m_swapExtent.height,
         (sharingMode == vk::SharingMode::eExclusive ? "Exclusive" : "Concurrent"),
         graphicsQueueFamily, presentQueueFamily);

    std::vector<vk::Format> candidates = {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
        vk::Format::eD16Unorm
    };
    m_depthFormat = m_physicalDevice->FindSupportedFormat(candidates, vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    vk::ImageCreateInfo depthImgInfo(
        {},
        vk::ImageType::e2D,
        m_depthFormat,
        vk::Extent3D{m_swapExtent.width, m_swapExtent.height, 1},
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment
    );
    m_depthImage = m_logicalDevice->GetHandle().createImage(depthImgInfo);

    vk::MemoryRequirements memReq = m_logicalDevice->GetHandle().getImageMemoryRequirements(m_depthImage);

    auto memProps = m_physicalDevice->GetHandle().getMemoryProperties();
    uint32_t memTypeIndex = 0;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((memReq.memoryTypeBits & (1 << i)) &&
            (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)) {
            memTypeIndex = i;
            break;
            }
    }

    vk::MemoryAllocateInfo allocInfo(memReq.size, memTypeIndex);
    m_depthImageMemory = m_logicalDevice->GetHandle().allocateMemory(allocInfo);
    m_logicalDevice->GetHandle().bindImageMemory(m_depthImage, m_depthImageMemory, 0);

    vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eDepth;
    if (m_depthFormat == vk::Format::eD24UnormS8Uint || m_depthFormat == vk::Format::eD32SfloatS8Uint) {
        aspectFlags |= vk::ImageAspectFlagBits::eStencil;
    }

    vk::ImageViewCreateInfo depthViewInfo(
        {},
        m_depthImage,
        vk::ImageViewType::e2D,
        m_depthFormat,
        {},
        {aspectFlags, 0, 1, 0, 1}
    );
    m_depthImageView = m_logicalDevice->GetHandle().createImageView(depthViewInfo);
    LOG_INFO("Vulkan", "Depth image created: format={}, size={}x{}, memory type={}",
             vk::to_string(m_depthFormat), m_swapExtent.width, m_swapExtent.height,
             vk::to_string(memProps.memoryTypes[memTypeIndex].propertyFlags));
    return true;
}

bool VulkanSwapChain::CreateFramebuffers(VulkanRenderPass* renderPass) {
    for (auto& fb : m_framebuffers) {
        m_logicalDevice->GetHandle().destroyFramebuffer(fb);
    }
    m_framebuffers.clear();

    try {
        for (auto& view : m_imageViews) {
            std::array<vk::ImageView, 2> attachments = { view, m_depthImageView };
            vk::FramebufferCreateInfo fbInfo({}, renderPass->GetHandle(), attachments.size(), attachments.data(), m_swapExtent.width, m_swapExtent.height, 1);
            m_framebuffers.push_back(m_logicalDevice->GetHandle().createFramebuffer(fbInfo));
        }
    } catch (const std::exception& e) {
        LOG_FATAL("Vulkan", "Failed to create framebuffer: {}", e.what());
        return false;
    }

    return true;
}

void VulkanSwapChain::BeginRender(vk::Semaphore &imageAvalibleSemaphore) {
    m_imageRenderIndex = m_logicalDevice->GetHandle().acquireNextImageKHR(m_swapchain, UINT64_MAX, imageAvalibleSemaphore).value;
}

void VulkanSwapChain::Present(vk::Semaphore &renderSemaphore) {
    vk::PresentInfoKHR presentInfo(
        1, &renderSemaphore,
        1, &m_swapchain,
        &m_imageRenderIndex
    );
    m_logicalDevice->GetGraphicsQueue().presentKHR(presentInfo);
}

