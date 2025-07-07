//
// Created by IDKTHIS on 02.07.2025.
//

#pragma once
#include "../IRenderer.h"
#include <memory>
#include <vector>

#include "../../Window/IWindow.h"
#include <vulkan/vulkan.h>

#include "VulkanInstance.h"

class CommandSystem;
class GraphicsPipeline;
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
    std::unique_ptr<VulkanSwapChain> m_swapChain;
    std::unique_ptr<VulkanRenderPass> m_renderPass;
    std::unique_ptr<GraphicsPipeline> m_graphicsPipeline;
    std::unique_ptr<CommandSystem> m_commandSystem;
    VkSurfaceKHR m_surface = nullptr;

    uint32_t m_currentFrame = 0;
    void Cleanup() override;

    bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtensions);
    void RenderFrame() override;

};