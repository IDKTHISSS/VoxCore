//
// Created by IDKTHIS on 14.07.2025.
//

#pragma once

#include "Platform/Renderer/Vulkan/Core/VulkanInstance.h"
#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "Platform/Renderer/Vulkan/Devices/PhysicalDevice.h"

class VulkanRenderPass;

class VulkanSwapChain {
public:
    VulkanSwapChain(VulkanInstance* instance, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice);
    ~VulkanSwapChain();

    bool Init(vk::SurfaceKHR surface, vk::Extent2D extent);
    bool CreateFramebuffers(VulkanRenderPass* renderPass);
    void BeginRender(vk::Semaphore& imageAvailableSemaphore);
    void Present(vk::Semaphore& renderSemaphore);

    vk::SurfaceFormatKHR& GetSurfaceFormat() { return m_surfaceFormat; }
    vk::Format& GetDepthFormat() { return m_depthFormat; }
    vk::Extent2D& GetSwapExtent() { return m_swapExtent; }
    vk::Framebuffer& GetFramebuffer(uint32_t index) { return m_framebuffers[index]; }
    uint32_t GetImageCount() const {
        return static_cast<uint32_t>(m_imageViews.size());
    }
    uint32_t GetCurrentImageIndex() const {
        return m_imageRenderIndex;
    }
    uint32_t AcquireNextImage(vk::Semaphore imageAvailableSemaphore);

private:
    VulkanInstance* m_vulkanInstance;
    vk::SwapchainKHR m_swapchain;
    PhysicalDevice* m_physicalDevice;
    LogicalDevice* m_logicalDevice;
    std::vector<vk::ImageView> m_imageViews;
    std::vector<vk::Framebuffer> m_framebuffers;
    vk::SurfaceFormatKHR m_surfaceFormat;
    vk::Extent2D m_swapExtent;

    vk::Image m_depthImage;
    vk::DeviceMemory m_depthImageMemory;
    vk::ImageView m_depthImageView;
    vk::Format m_depthFormat;
    uint32_t m_imageRenderIndex;
};
