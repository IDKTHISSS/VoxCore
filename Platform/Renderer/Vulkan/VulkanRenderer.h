//
// Created by IDKTHIS on 02.07.2025.
//

#pragma once
#include <chrono>

#include "../IRenderer.h"
#include <memory>
#include <vector>

#include "Platform/Window/IWindow.h"
#include <vulkan/vulkan.hpp>

#include "Core/VulkanInstance.h"

class VulkanCommandSystem;
class TrianglePipeline;
class CommandSystem;
class GraphicsPipeline2;
class VulkanRenderPass;
class VulkanSwapChain;
class LogicalDevice;
class PhysicalDevice;

// Indices (locations) of Queue Families (if they exist at all) TEMP HERE

class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;



    bool Init(IWindow* window) override;
    // Add more Vulkan-specific methods as needed

private:
    // Vulkan instance, device, etc. (placeholders for now)
    std::unique_ptr<VulkanInstance> m_instance;
    std::unique_ptr<PhysicalDevice> m_physicalDevice;
    std::unique_ptr<LogicalDevice> m_logicalDevice;
    std::unique_ptr<VulkanSwapChain> m_swapchain;
    std::unique_ptr<VulkanRenderPass> m_renderPass;
    std::unique_ptr<GraphicsPipeline2> m_graphicsPipeline;
    std::unique_ptr<VulkanCommandSystem> m_commandSystem;
    vk::SurfaceKHR m_surface = nullptr;
    vk::Semaphore m_imageAvailable;
    vk::Semaphore m_renderFinished;
    uint32_t m_currentFrame = 0;
    std::chrono::high_resolution_clock::time_point m_startTime;
    void Cleanup() override;

    bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtensions);
    void RenderFrame() override;

};