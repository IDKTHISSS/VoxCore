//
// Created by IDKTHIS on 15.07.2025.
//

#include "VulkanCommandSystem.h"

#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "Platform/Renderer/Vulkan/Swapchain/VulkanSwapChain.h"


VulkanCommandSystem::VulkanCommandSystem(VulkanInstance *instance, LogicalDevice *logicalDevice,
                                         VulkanRenderPass *renderPass, VulkanSwapChain *swapChain, GraphicsPipeline *graphicsPipeline) :
    m_vulkanInstance(instance), m_logicalDevice(logicalDevice), m_renderPass(renderPass), m_swapChain(swapChain),
    m_graphicsPipeline(graphicsPipeline)
{
}

bool VulkanCommandSystem::Init(uint32_t graphicsQueueFamilyIndex) {
    vk::CommandPool cmdPool = m_logicalDevice->GetHandle().createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer }, graphicsQueueFamilyIndex });
    m_commandBuffers= m_logicalDevice->GetHandle().allocateCommandBuffers({ cmdPool, vk::CommandBufferLevel::ePrimary, (uint32_t)m_swapChain->GetFramebuffers().size() });

    m_imageAvailable = m_logicalDevice->GetHandle().createSemaphore({});
    m_renderFinished = m_logicalDevice->GetHandle().createSemaphore({});
    return true;
}

void VulkanCommandSystem::Cleanup() {
    if (!m_logicalDevice) return;
    vk::Device device = m_logicalDevice->GetHandle();
    m_logicalDevice->GetHandle().waitIdle();

    // 1. Сначала освобождаем командные буферы
    if (m_commandPool && !m_commandBuffers.empty()) {
        device.freeCommandBuffers(m_commandPool, m_commandBuffers);
        m_commandBuffers.clear();
    }

    // 2. Уничтожаем командный пул
    if (m_commandPool) {
        device.destroyCommandPool(m_commandPool);
        m_commandPool = nullptr;
    }

    // 3. Уничтожаем фейны
    for (auto fence : m_inFlightFences) {
        if (fence) {
            device.destroyFence(fence);
        }
    }
    m_inFlightFences.clear();

    // 4. Уничтожаем семафоры рендера
    for (auto semaphore : m_renderFinishedSemaphores) {
        if (semaphore) {
            device.destroySemaphore(semaphore);
        }
    }
    m_renderFinishedSemaphores.clear();

    // 5. Уничтожаем семафоры ожидания изображения
    for (auto semaphore : m_imageAvailableSemaphores) {
        if (semaphore) {
            device.destroySemaphore(semaphore);
        }
    }
    m_imageAvailableSemaphores.clear();

    // Если m_imageAvailable и m_renderFinished — отдельные семафоры, уничтожь их тоже (один раз)
    if (m_imageAvailable) {
        device.destroySemaphore(m_imageAvailable);
        m_imageAvailable = nullptr;
    }
    if (m_renderFinished) {
        device.destroySemaphore(m_renderFinished);
        m_renderFinished = nullptr;
    }
}
