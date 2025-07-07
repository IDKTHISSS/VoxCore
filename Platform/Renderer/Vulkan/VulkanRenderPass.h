//
// Created by IDKTHIS on 07.07.2025.
//

#pragma once

#include <vulkan/vulkan.h>

#include "VulkanSwapChain.h"

class LogicalDevice;

class VulkanRenderPass {
public:
    VulkanRenderPass();
    ~VulkanRenderPass();

    bool Init(LogicalDevice* logicalDevice, VulkanSwapChain* swapChain);
    [[nodiscard]] VkRenderPass GetRenderPass() const { return m_renderPass; }
private:
    VkRenderPass m_renderPass;
};
