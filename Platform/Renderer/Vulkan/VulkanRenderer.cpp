//
// Created by IDKTHIS on 02.07.2025.
//

#include "VulkanRenderer.h"
#include <vulkan/vulkan.h>
#include <cassert>
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>

#include "CommandSystem.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "../../../Core/CVar/CVar.h"
#include "../../../Core/Log/Logger.h"

VulkanRenderer::VulkanRenderer() {
	m_instance = std::make_unique<VulkanInstance>();
	m_physicalDevice = std::make_unique<PhysicalDevice>(m_instance.get());
	m_logicalDevice = std::make_unique<LogicalDevice>();
	m_swapChain = std::make_unique<VulkanSwapChain>();
	m_renderPass = std::make_unique<VulkanRenderPass>();
	m_graphicsPipeline = std::make_unique<GraphicsPipeline>(m_logicalDevice.get(), m_physicalDevice.get(), m_renderPass.get());
	m_commandSystem = std::make_unique<CommandSystem>(m_logicalDevice.get(), m_renderPass.get(),
		m_swapChain.get(), m_graphicsPipeline.get());
};
VulkanRenderer::~VulkanRenderer() = default;

bool VulkanRenderer::Init(IWindow *window) {
    assert(window != nullptr);
	try {
		m_instance->Init();
		SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(window->GetNativeHandle()), m_instance->GetInstance(), nullptr, &m_surface);
		m_physicalDevice->Init(m_surface);
		m_logicalDevice->Init(m_physicalDevice.get());
		m_swapChain->Init(m_physicalDevice.get(), m_logicalDevice.get(), m_surface,0,0);
		m_renderPass->Init(m_logicalDevice.get(), m_swapChain.get());
		m_swapChain->CreateFramebuffers(m_logicalDevice.get(), m_renderPass.get());
		m_graphicsPipeline->Init("shaders/vert.spv","shaders/frag.spv");
		m_commandSystem->Init(m_physicalDevice->GetGraphicsQueueFamilyIndex());
	}
	catch (const std::runtime_error &e) {
		LOGF_ERROR("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}
    return true;
}

void VulkanRenderer::Cleanup() {
	// TODO: Cleanup Vulkan resources properly
	//vkDestroyDevice(m_device.logicalDevice, nullptr);
	m_instance.reset();
}


bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char *> *checkExtensions) {
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Create a list of VkExtensionProperties using count
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Check if given extensions are in list of available extensions
	for (const auto &checkExtension : *checkExtensions)
	{
		bool hasExtension = false;
		for (const auto &extension : extensions)
		{
			if (strcmp(checkExtension, extension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

void VulkanRenderer::RenderFrame() {
	vkWaitForFences(m_logicalDevice->GetDevice(), 1, m_commandSystem->GetInFlightFenceP(m_currentFrame), VK_TRUE, UINT64_MAX);


	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_logicalDevice->GetDevice(), m_swapChain->GetSwapChain(), UINT64_MAX, m_commandSystem->GetImageAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		//recreateSwapChain();
		return;
	}
	vkResetFences(m_logicalDevice->GetDevice(), 1, m_commandSystem->GetInFlightFenceP(m_currentFrame));

	vkResetCommandBuffer(*m_commandSystem->GetCommandBuffer(m_currentFrame), 0);
	m_commandSystem->recordCommandBuffer(*m_commandSystem->GetCommandBuffer(m_currentFrame), imageIndex);


	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore waitSemaphores[] = { m_commandSystem->GetImageAvailableSemaphore(m_currentFrame) };
	VkSemaphore signalSemaphores[] = { m_commandSystem->GetRenderFinishedSemaphore(m_currentFrame) };
	VkSwapchainKHR swapChains[] = { m_swapChain->GetSwapChain() };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = m_commandSystem->GetCommandBuffer(m_currentFrame);
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkQueueSubmit(m_logicalDevice->GetGraphicsQueue(), 1, &submitInfo, m_commandSystem->GetInFlightFence(m_currentFrame));


	VkPresentInfoKHR presentInfoKHR{};
	presentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfoKHR.swapchainCount = 1;
	presentInfoKHR.pWaitSemaphores = signalSemaphores;
	presentInfoKHR.pSwapchains = swapChains;
	presentInfoKHR.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR( m_logicalDevice-> GetPresentQueue(), &presentInfoKHR);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR  ) {
		//framebufferResized = false;
		//recreateSwapChain();
	}

	m_currentFrame = (m_currentFrame + 1) % GET_CVAR(int, "r_max_frames_in_flight");
}

