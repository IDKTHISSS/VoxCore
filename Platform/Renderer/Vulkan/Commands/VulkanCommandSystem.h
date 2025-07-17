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
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    [[nodiscard]] vk::CommandBuffer *GetCommandBuffer(uint32_t frameIndex) {
        return &m_commandBuffers[frameIndex];
    }

    [[nodiscard]] vk::Semaphore& GetImageAvailableSemaphore() {
        return m_imageAvailable;
    }

    [[nodiscard]] vk::Semaphore& GetRenderFinishedSemaphore() {
        return m_renderFinished;
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
    GraphicsPipeline* m_graphicsPipeline;
    VkCommandPool m_commandPool{};
    std::vector<vk::CommandBuffer> m_commandBuffers;
    vk::Semaphore m_imageAvailable;
    vk::Semaphore m_renderFinished;
    std::vector<vk::Semaphore> m_imageAvailableSemaphores; // TODO Под вопросомм
    std::vector<vk::Semaphore> m_renderFinishedSemaphores; // TODO Под вопросомм
    std::vector<VkFence> m_inFlightFences;
};

