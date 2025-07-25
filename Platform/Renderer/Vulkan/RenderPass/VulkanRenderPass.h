//
// Created by IDKTHIS on 15.07.2025.
//

#pragma once

#include "Platform/Renderer/Vulkan/Core/VulkanInstance.h"

class LogicalDevice;
class VulkanSwapChain;

class VulkanRenderPass {
public:
    VulkanRenderPass(VulkanInstance* instance, LogicalDevice* logicalDevice, VulkanSwapChain* swapchain);
    ~VulkanRenderPass();
    bool Init();
    vk::RenderPass& GetHandle() { return m_renderPass; }

private:
    VulkanInstance* m_vulkanInstance; // Pointer to the Vulkan instance
    vk::RenderPass m_renderPass; // Handle to the render pass
    LogicalDevice* m_logicalDevice;
    VulkanSwapChain* m_swapchain;
};
