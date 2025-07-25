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
    void BeginFrame();
    void BeginRender(vk::Framebuffer framebuffer, vk::Extent2D extent);
    void EndRender();
    void EndFrame();
    uint32_t GetCurrentFrame() const { return m_currentFrame; }

private:
    uint32_t m_currentFrame = 0;
    VulkanInstance* m_vulkanInstance;
    LogicalDevice* m_logicalDevice;
    VulkanRenderPass* m_renderPass;
    VulkanSwapChain* m_swapChain;
    GraphicsPipeline* m_graphicsPipeline;
    vk::CommandPool m_commandPool{};
    std::vector<vk::CommandBuffer> m_commandBuffers;
    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;
};

