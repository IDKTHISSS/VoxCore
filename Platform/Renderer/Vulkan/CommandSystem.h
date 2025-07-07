//
// Created by IDKTHIS on 07.07.2025.
//

#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanRenderPass.h"


class GraphicsPipeline;
class LogicalDevice;

class CommandSystem {
public:
    explicit CommandSystem(LogicalDevice* logicalDevice, VulkanRenderPass* renderPass,
        VulkanSwapChain* swapChain, GraphicsPipeline* graphicsPipeline);
    ~CommandSystem();
    bool Init(uint32_t graphicsQueueFamilyIndex);
    void Cleanup();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    [[nodiscard]] VkCommandBuffer* GetCommandBuffer(uint32_t frameIndex) {
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
    LogicalDevice* m_logicalDevice;
    VulkanRenderPass* m_renderPass;
    VulkanSwapChain* m_swapChain;
    GraphicsPipeline* m_graphicsPipeline;
    VkCommandPool m_commandPool{};
    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};
