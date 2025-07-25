//
// Created by IDKTHIS on 15.07.2025.
//

#include "VulkanCommandSystem.h"

#include "Core/CVar/CVar.h"
#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "Platform/Renderer/Vulkan/RenderPass/VulkanRenderPass.h"
#include "Platform/Renderer/Vulkan/Swapchain/VulkanSwapChain.h"

CONVAR("r_max_frames_in_flight", 2, "Maximum number of frames in flight for Vulkan swapchain", CVAR_RUNTIME_ONLY);

VulkanCommandSystem::VulkanCommandSystem(VulkanInstance *instance, LogicalDevice *logicalDevice,
                                         VulkanRenderPass *renderPass, VulkanSwapChain *swapChain, GraphicsPipeline *graphicsPipeline) :
    m_vulkanInstance(instance), m_logicalDevice(logicalDevice), m_renderPass(renderPass), m_swapChain(swapChain),
    m_graphicsPipeline(graphicsPipeline)
{
}

bool VulkanCommandSystem::Init(uint32_t graphicsQueueFamilyIndex) {
    vk::CommandPoolCreateInfo poolInfo = {
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        graphicsQueueFamilyIndex
    };
    m_commandPool = m_logicalDevice->GetHandle().createCommandPool(poolInfo);
    m_commandBuffers = m_logicalDevice->GetHandle().allocateCommandBuffers({
           m_commandPool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(GET_CVAR(int, "r_max_frames_in_flight"))
       });

    m_imageAvailableSemaphores.resize(GET_CVAR(int, "r_max_frames_in_flight"));
    m_renderFinishedSemaphores.resize(GET_CVAR(int, "r_max_frames_in_flight"));
    m_inFlightFences.resize(GET_CVAR(int, "r_max_frames_in_flight"));
    for (uint32_t i = 0; i < GET_CVAR(int, "r_max_frames_in_flight"); ++i) {
        m_imageAvailableSemaphores[i] = m_logicalDevice->GetHandle().createSemaphore({});
        m_renderFinishedSemaphores[i] = m_logicalDevice->GetHandle().createSemaphore({});
        m_inFlightFences[i] = m_logicalDevice->GetHandle().createFence({ vk::FenceCreateFlagBits::eSignaled });
    }
    return true;
}

void VulkanCommandSystem::Cleanup() {
    for (auto& semaphore : m_imageAvailableSemaphores) {
        if (semaphore) {
            m_logicalDevice->GetHandle().destroySemaphore(semaphore);
        }
    }
    m_imageAvailableSemaphores.clear();

    for (auto& semaphore : m_renderFinishedSemaphores) {
        if (semaphore) {
            m_logicalDevice->GetHandle().destroySemaphore(semaphore);
        }
    }
    m_renderFinishedSemaphores.clear();

    for (auto& fence : m_inFlightFences) {
        if (fence) {
            m_logicalDevice->GetHandle().destroyFence(fence);
        }
    }
    m_inFlightFences.clear();

    if (m_commandPool) {
        m_logicalDevice->GetHandle().destroyCommandPool(m_commandPool);
        m_commandPool = nullptr;
    }

}

void VulkanCommandSystem::BeginFrame() {
    m_logicalDevice->GetHandle().waitForFences(1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
    m_logicalDevice->GetHandle().resetFences(1, &m_inFlightFences[m_currentFrame]);
    m_swapChain->BeginRender(m_imageAvailableSemaphores[m_currentFrame]);
    m_commandBuffers[m_currentFrame].reset();
}

void VulkanCommandSystem::BeginRender(vk::Framebuffer framebuffer, vk::Extent2D extent) {
    auto& cmd = m_commandBuffers[m_currentFrame];

    cmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = vk::ClearColorValue(std::array<float, 4>{ 0.1f, 0.1f, 0.1f, 1.0f });
    clearValues[1].depthStencil = vk::ClearDepthStencilValue( 1.0f, 0 );

    vk::RenderPassBeginInfo renderPassInfo(
        m_renderPass->GetHandle(),
        framebuffer,
        { {0, 0}, extent },
        static_cast<uint32_t>(clearValues.size()),
        clearValues.data()
    );

    cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void VulkanCommandSystem::EndRender() {
    m_commandBuffers[m_currentFrame].endRenderPass();
    m_commandBuffers[m_currentFrame].end();
}

void VulkanCommandSystem::EndFrame() {
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo(
        1, &m_imageAvailableSemaphores[m_currentFrame],
        &waitStage,
        1,  &m_commandBuffers[m_currentFrame],
        1, &m_renderFinishedSemaphores[m_currentFrame]
    );

    m_logicalDevice->GetGraphicsQueue().submit(submitInfo, m_inFlightFences[m_currentFrame]);
    m_swapChain->Present(m_renderFinishedSemaphores[m_currentFrame]);

    m_currentFrame = (m_currentFrame + 1) % GET_CVAR(int, "r_max_frames_in_flight");
}
