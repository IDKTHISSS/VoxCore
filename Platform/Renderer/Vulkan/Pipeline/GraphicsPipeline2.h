//
// Created by IDKTHIS on 15.07.2025.
//

#pragma once
#include "Platform/Renderer/Vulkan/Core/VulkanInstance.h"


class VulkanSwapChain;
class LogicalDevice;
class VulkanRenderPass;

class GraphicsPipeline2 {
public:
    GraphicsPipeline2(VulkanInstance* instance, LogicalDevice* logicalDevice, VulkanSwapChain* swapChain,  VulkanRenderPass* renderPass);
    ~GraphicsPipeline2() = default;

    // Initialize the physical device with the given Vulkan instance
    bool Init();

    vk::Pipeline& GetHandle() { return m_pipeline; }
    vk::PipelineLayout& GetPipelineLayout() { return m_pipelineLayout; }

private:
    std::vector<char> readFile(const std::string& filename);
    VulkanInstance* m_vulkanInstance;
    LogicalDevice* m_logicalDevice; // Pointer to the logical device
    VulkanSwapChain* m_swapChain; // Pointer to the swapchain
    VulkanRenderPass* m_renderPass; // Pointer to the render pass
    vk::PipelineLayout m_pipelineLayout; // Handle to the pipeline layout
    vk::Pipeline m_pipeline; // Handle to the graphics pipeline
};
