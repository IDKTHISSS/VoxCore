//
// Created by IDKTHIS on 15.07.2025.
//


#include "VulkanRenderPass.h"

#include "Platform/Renderer/Vulkan/Swapchain/VulkanSwapChain.h"


VulkanRenderPass::VulkanRenderPass(VulkanInstance *instance, LogicalDevice *logicalDevice, VulkanSwapChain* swapchain)
    : m_vulkanInstance(instance), m_logicalDevice(logicalDevice), m_swapchain(swapchain) {

}

VulkanRenderPass::~VulkanRenderPass() {
    if (m_renderPass) {
        m_logicalDevice->GetHandle().destroyRenderPass(m_renderPass);
    }
}

bool VulkanRenderPass::Init() {
    vk::AttachmentDescription colorAttachment({}, m_swapchain->GetSurfaceFormat().format, vk::SampleCountFlagBits::e1,
           vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
           vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference colorRef(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorRef);
    vk::RenderPassCreateInfo renderPassInfo({}, 1, &colorAttachment, 1, &subpass);
    m_renderPass = m_logicalDevice->GetHandle().createRenderPass(renderPassInfo);
    return true;
}
