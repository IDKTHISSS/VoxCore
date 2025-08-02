//
// Created by IDKTHIS on 15.07.2025.
//

#include "VulkanCommandSystem.h"

#include "Core/CVar/CVar.h"
#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "Platform/Renderer/Vulkan/RenderPass/VulkanRenderPass.h"
#include "Platform/Renderer/Vulkan/Swapchain/VulkanSwapChain.h"



VulkanCommandSystem::VulkanCommandSystem(VulkanInstance *instance, LogicalDevice *logicalDevice,
                                         VulkanRenderPass *renderPass, VulkanSwapChain *swapChain, GraphicsPipeline *graphicsPipeline) :
    m_vulkanInstance(instance), m_logicalDevice(logicalDevice), m_renderPass(renderPass), m_swapChain(swapChain),
    m_graphicsPipeline(graphicsPipeline)
{
}

bool VulkanCommandSystem::Init(uint32_t graphicsQueueFamilyIndex) {
    vk::CommandPoolCreateInfo poolInfo(
           vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
           graphicsQueueFamilyIndex
       );
    m_commandPool = m_logicalDevice->GetHandle().createCommandPool(poolInfo);

    m_commandBuffers = m_logicalDevice->GetHandle().allocateCommandBuffers({
        m_commandPool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(GET_CVAR(int, "r_max_frames_in_flight"))
    });

    return true;
}

void VulkanCommandSystem::Cleanup() {

    if (!m_commandBuffers.empty() && m_logicalDevice) {
        m_logicalDevice->GetHandle().freeCommandBuffers(m_commandPool, m_commandBuffers);
        m_commandBuffers.clear();
    }
    if (m_commandPool && m_logicalDevice) {
        m_logicalDevice->GetHandle().destroyCommandPool(m_commandPool);
        m_commandPool = nullptr;
    }


}