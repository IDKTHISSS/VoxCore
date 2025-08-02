//
// Created by IDKTHIS on 15.07.2025.
//

#pragma once

#include "Platform/Renderer/Vulkan/Core/VulkanInstance.h"

class GraphicsPipeline;
class VulkanSwapChain;
class VulkanRenderPass;
class LogicalDevice;

class VulkanCommandSystem {
public:
    explicit VulkanCommandSystem(VulkanInstance* instance, LogicalDevice* logicalDevice, VulkanRenderPass* renderPass,
        VulkanSwapChain* swapChain, GraphicsPipeline* graphicsPipeline);
    ~VulkanCommandSystem()
    {
        Cleanup();
    }
    bool Init(uint32_t graphicsQueueFamilyIndex);
    void Cleanup();
    vk::CommandBuffer& GetCommandBuffer(const int frameIndex = -1) {
        return m_commandBuffers[frameIndex];
    }

private:
    VulkanInstance* m_vulkanInstance;
    LogicalDevice* m_logicalDevice;
    VulkanRenderPass* m_renderPass;
    VulkanSwapChain* m_swapChain;
    GraphicsPipeline* m_graphicsPipeline;
    vk::CommandPool m_commandPool{};
    std::vector<vk::CommandBuffer> m_commandBuffers;
};

