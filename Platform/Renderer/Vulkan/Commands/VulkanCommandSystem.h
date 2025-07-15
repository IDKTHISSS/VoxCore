//
// Created by IDKTHIS on 15.07.2025.
//

#pragma once

#include "Platform/Renderer/Vulkan/Core/VulkanInstance.h"

class GraphicsPipeline2;
class VulkanSwapChain;
class VulkanRenderPass;
class LogicalDevice;

class VulkanCommandSystem {
public:
    explicit VulkanCommandSystem(VulkanInstance* instance, LogicalDevice* logicalDevice, VulkanRenderPass* renderPass,
        VulkanSwapChain* swapChain, GraphicsPipeline2* graphicsPipeline);
    ~VulkanCommandSystem() = default;
    bool Init(uint32_t graphicsQueueFamilyIndex);
    void Cleanup();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    [[nodiscard]] vk::CommandBuffer *GetCommandBuffer(uint32_t frameIndex) {
        return &m_commandBuffers[frameIndex];
    }

    [[nodiscard]] VkSemaphore GetImageAvailableSemaphore(uint32_t frameIndex) const {
        return m_imageAvailableSemaphores[frameIndex];
    }

    [[nodiscard]] VkSemaphore GetRenderFinishedSemaphore(uint32_t frameIndex) const {
        return m_renderFinishedSemaphores[frameIndex];
    }

    [[nodiscard]] VkFence GetInFlightFence(uint32_t frameIndex) const {
        return m_inFlightFences[frameIndex];
    }
    [[nodiscard]] VkFence* GetInFlightFenceP(uint32_t frameIndex) {
        return &m_inFlightFences[frameIndex];
    }

private:
    VulkanInstance* m_vulkanInstance;
    LogicalDevice* m_logicalDevice;
    VulkanRenderPass* m_renderPass;
    VulkanSwapChain* m_swapChain;
    GraphicsPipeline2* m_graphicsPipeline;
    VkCommandPool m_commandPool{};
    std::vector<vk::CommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};

