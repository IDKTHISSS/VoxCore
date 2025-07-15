//
// Created by IDKTHIS on 15.07.2025.
//

#include "VulkanCommandSystem.h"

#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "Platform/Renderer/Vulkan/Swapchain/VulkanSwapChain.h"


VulkanCommandSystem::VulkanCommandSystem(VulkanInstance *instance, LogicalDevice *logicalDevice,
                                         VulkanRenderPass *renderPass, VulkanSwapChain *swapChain, GraphicsPipeline2 *graphicsPipeline) :
    m_vulkanInstance(instance), m_logicalDevice(logicalDevice), m_renderPass(renderPass), m_swapChain(swapChain),
    m_graphicsPipeline(graphicsPipeline)
{
}

bool VulkanCommandSystem::Init(uint32_t graphicsQueueFamilyIndex) {
    vk::CommandPool cmdPool = m_logicalDevice->GetHandle().createCommandPool({ {}, graphicsQueueFamilyIndex });
    m_commandBuffers= m_logicalDevice->GetHandle().allocateCommandBuffers({ cmdPool, vk::CommandBufferLevel::ePrimary, (uint32_t)m_swapChain->GetFramebuffers().size() });
    return true;
}
