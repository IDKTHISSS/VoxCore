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

    // Initialize the physical device with the given Vulkan instance
    bool Init(vk::SurfaceKHR surface);
    bool CreateFramebuffers(VulkanRenderPass* renderPass);

    [[nodiscard]] vk::SwapchainKHR& GetHandle() { return m_swapchain; }
    [[nodiscard]] vk::SurfaceFormatKHR& GetSurfaceFormat() { return m_surfaceFormat; }
    [[nodiscard]] const std::vector<vk::ImageView>& GetImageViews() { return m_imageViews; }
    [[nodiscard]] vk::Extent2D& GetSwapExtent() { return m_swapExtent; }
    [[nodiscard]] const std::vector<vk::Framebuffer>& GetFramebuffers() { return m_framebuffers; }


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
    vk::Format m_depthFormat = vk::Format::eD32Sfloat;
};
