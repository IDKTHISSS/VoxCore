//
// Created by IDKTHIS on 02.07.2025.
//

#include "VulkanRenderer.h"

#include <cassert>
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>

#include "Core/CVar/CVar.h"
#include "Core/Log/Logger.h"
#include "Commands/VulkanCommandSystem.h"
#include "Devices/LogicalDevice.h"
#include "Devices/PhysicalDevice.h"
#include "Pipeline/GraphicsPipeline.h"
#include "RenderPass/VulkanRenderPass.h"
#include "Swapchain/VulkanSwapChain.h"


VulkanRenderer::VulkanRenderer() {
	m_instance = std::make_unique<VulkanInstance>();
	m_physicalDevice = std::make_unique<PhysicalDevice>(m_instance.get());
	m_logicalDevice = std::make_unique<LogicalDevice>(m_instance.get());
	m_swapchain = std::make_unique<VulkanSwapChain>(m_instance.get(), m_physicalDevice.get(), m_logicalDevice.get());
	m_renderPass = std::make_unique<VulkanRenderPass>(m_instance.get(), m_logicalDevice.get(), m_swapchain.get());
	m_graphicsPipeline = std::make_unique<GraphicsPipeline>( m_logicalDevice.get(), m_physicalDevice.get(), m_renderPass.get(), m_swapchain.get());
	m_commandSystem = std::make_unique<VulkanCommandSystem>(m_instance.get(), m_logicalDevice.get(), m_renderPass.get(), m_swapchain.get(), m_graphicsPipeline.get());
};
VulkanRenderer::~VulkanRenderer() = default;
VulkanShader* vulkanShader = nullptr;;
bool VulkanRenderer::Init(IWindow *window) {
    assert(window != nullptr);
	try {
		m_instance->Init();
		VkSurfaceKHR c_surface;
		SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(window->GetNativeHandle()), m_instance->GetInstance(), nullptr, &c_surface);
		m_surface = c_surface;
		m_physicalDevice->Init(m_surface);
		m_logicalDevice->Init(m_physicalDevice.get());
		m_swapchain->Init(m_surface);
		m_renderPass->Init();
		vulkanShader = new VulkanShader(m_logicalDevice.get());
		vulkanShader->LoadFromFile("shaders/shader.glsl");
		m_graphicsPipeline->SetShader(vulkanShader);
		m_graphicsPipeline->SetPushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(float));

		m_graphicsPipeline->Init();
		m_swapchain->CreateFramebuffers(m_renderPass.get());
		m_commandSystem->Init(m_physicalDevice->GetGraphicsQueueFamilyIndex());

		m_imageAvailable = m_logicalDevice->GetHandle().createSemaphore({});
		m_renderFinished = m_logicalDevice->GetHandle().createSemaphore({});

		m_startTime = std::chrono::high_resolution_clock::now();
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
	uint32_t imageIndex = m_logicalDevice->GetHandle().acquireNextImageKHR(m_swapchain->GetHandle(), UINT64_MAX, m_imageAvailable).value;

	// Reset and record command buffer
	auto cmdBuffer = m_commandSystem->GetCommandBuffer(imageIndex);
	cmdBuffer->reset({});
	cmdBuffer->begin({ vk::CommandBufferUsageFlagBits::eSimultaneousUse });

	vk::ClearValue clearColor = vk::ClearColorValue(std::array<float, 4>{ 0.1f, 0.1f, 1.0f, 1.0f });
	vk::RenderPassBeginInfo renderPassBeginInfo(
		m_renderPass->GetHandle(),
		m_swapchain->GetFramebuffers()[imageIndex],
		{ {0, 0}, m_swapchain->GetSwapExtent() },
		1, &clearColor
	);

	cmdBuffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
	cmdBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline->GetHandle());

	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = currentTime - m_startTime;
	float time = elapsed.count();
	m_graphicsPipeline->PushConstant(cmdBuffer,  vk::ShaderStageFlagBits::eVertex, 0, sizeof(float), &time);


	cmdBuffer->draw(8526, 1, 0, 0);
	cmdBuffer->endRenderPass();
	cmdBuffer->end();

	// Вставляем нужный флаг ожидания
	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	vk::SubmitInfo submitInfo(
		1, &m_imageAvailable,
		&waitStage,
		1, &(*cmdBuffer),
		1, &m_renderFinished
	);

	m_logicalDevice->GetGraphicsQueue().submit(submitInfo);

	vk::PresentInfoKHR presentInfo(
		1, &m_renderFinished,
		1, &m_swapchain->GetHandle(),
		&imageIndex
	);
	m_logicalDevice->GetGraphicsQueue().presentKHR(presentInfo);

	m_logicalDevice->GetGraphicsQueue().waitIdle();
	m_currentFrame = (m_currentFrame + 1) % 2;
}


