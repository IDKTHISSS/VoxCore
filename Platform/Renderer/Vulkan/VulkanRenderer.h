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
#include <glm/glm.hpp>

#include "Core/VulkanInstance.h"
#include "Camera.h"
#include "Pipeline/GraphicsPipeline.h"

struct CameraUBO {
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanCommandSystem;
class TrianglePipeline;
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
    bool Init(IWindow* window, UWorld* world) override;
    Camera* GetCamera() { return m_camera.get(); }

private:

    UWorld* m_world = nullptr;
    // Vulkan instance, device, etc. (placeholders for now)
    std::unique_ptr<VulkanInstance> m_instance;
    std::unique_ptr<PhysicalDevice> m_physicalDevice;
    std::unique_ptr<LogicalDevice> m_logicalDevice;
    std::unique_ptr<VulkanSwapChain> m_swapchain;
    std::unique_ptr<VulkanRenderPass> m_renderPass;
    std::unique_ptr<GraphicsPipeline> m_graphicsPipeline;
    std::unique_ptr<VulkanCommandSystem> m_commandSystem;
    std::unique_ptr<Camera> m_camera;
    // uniform buffer для камеры
    BufferResource m_cameraBuffer;
    vk::DescriptorSetLayout m_cameraDescriptorSetLayout;
    vk::DescriptorPool m_cameraDescriptorPool;
    vk::DescriptorSet m_cameraDescriptorSet;
    vk::SurfaceKHR m_surface = nullptr;
    uint32_t m_currentFrame = 0;
    std::chrono::high_resolution_clock::time_point m_startTime;
    void Cleanup() override;

    bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtensions);
    void RenderFrame() override;

};