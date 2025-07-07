//
// Created by IDKTHIS on 07.07.2025.
//

#include "CommandSystem.h"

#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "../../../Core/CVar/CVar.h"

CONVAR("r_max_frames_in_flight", 2, "Maximum number of frames in flight", CVarFlags::CVAR_ARCHIVE);


CommandSystem::CommandSystem(LogicalDevice *logicalDevice, VulkanRenderPass *renderPass, VulkanSwapChain *swapChain,
    GraphicsPipeline *graphicsPipeline) : m_logicalDevice(logicalDevice), m_renderPass(renderPass), m_swapChain(swapChain),
m_graphicsPipeline(graphicsPipeline)
{
}

CommandSystem::~CommandSystem() {
}

bool CommandSystem::Init(uint32_t graphicsQueueFamilyIndex) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    if (vkCreateCommandPool(m_logicalDevice->GetDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        return false;

    // 2. Command buffers
    m_commandBuffers.resize(GET_CVAR(int, "r_max_frames_in_flight"));
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if (vkAllocateCommandBuffers(m_logicalDevice->GetDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
        return false;

    // 3. Semaphores and fences
    m_imageAvailableSemaphores.resize(GET_CVAR(int, "r_max_frames_in_flight"));
    m_renderFinishedSemaphores.resize(GET_CVAR(int, "r_max_frames_in_flight"));
    m_inFlightFences.resize(GET_CVAR(int, "r_max_frames_in_flight"));

    VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < GET_CVAR(int, "r_max_frames_in_flight"); ++i) {
        if (vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_logicalDevice->GetDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}

void CommandSystem::Cleanup() {
}

void CommandSystem::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkClearValue clearValue = { { 0.0f, 0.0f, 0.0f, 1.0f } };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_renderPass->GetRenderPass();
    renderPassBeginInfo.framebuffer = *m_swapChain->GetSwapChainFramebuffer(imageIndex);
    renderPassBeginInfo.renderArea.offset = { 0,0 };
    renderPassBeginInfo.renderArea.extent = m_swapChain->GetSwapChainExtent();
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;


    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->GetGraphicsPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapChain->GetSwapChainExtent().width;
    viewport.height = (float)m_swapChain->GetSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = m_swapChain->GetSwapChainExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);
}
