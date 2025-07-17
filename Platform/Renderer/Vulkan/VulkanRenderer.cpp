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
#include "Camera.h"
#include "Core/ECS/BaseClasses/UWorld.h"
#include "Core/ECS/Components/UTransformComponent.h"


VulkanRenderer::VulkanRenderer() {
	m_instance = std::make_unique<VulkanInstance>();
	m_physicalDevice = std::make_unique<PhysicalDevice>(m_instance.get());
	m_logicalDevice = std::make_unique<LogicalDevice>(m_instance.get());
	m_swapchain = std::make_unique<VulkanSwapChain>(m_instance.get(), m_physicalDevice.get(), m_logicalDevice.get());
	m_renderPass = std::make_unique<VulkanRenderPass>(m_instance.get(), m_logicalDevice.get(), m_swapchain.get());
	m_graphicsPipeline = std::make_unique<GraphicsPipeline>( m_logicalDevice.get(), m_physicalDevice.get(), m_renderPass.get(), m_swapchain.get());
	m_commandSystem = std::make_unique<VulkanCommandSystem>(m_instance.get(), m_logicalDevice.get(), m_renderPass.get(), m_swapchain.get(), m_graphicsPipeline.get());
	// Инициализация камеры: позиция, yaw, pitch, fov, aspect, near, far
	m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 1.0f), 180.0f, 0.0f, 60.0f, 16.0f/9.0f, 0.1f, 100.0f);
};
VulkanRenderer::~VulkanRenderer() = default;
VulkanShader* vulkanShader = nullptr;;
bool VulkanRenderer::Init(IWindow *window, UWorld* world) {
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

		// --- Uniform buffer и дескрипторы для камеры ---
		// 1. Создать layout ДО Init pipeline
		vk::DescriptorSetLayoutBinding cameraLayoutBinding{};
		cameraLayoutBinding.binding = 0;
		cameraLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
		cameraLayoutBinding.descriptorCount = 1;
		cameraLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
		cameraLayoutBinding.pImmutableSamplers = nullptr;
		vk::DescriptorSetLayoutCreateInfo layoutInfo{{}, 1, &cameraLayoutBinding};
		m_cameraDescriptorSetLayout = m_logicalDevice->GetHandle().createDescriptorSetLayout(layoutInfo);
		m_graphicsPipeline->SetDescriptorSetLayouts({m_cameraDescriptorSetLayout});

		// Теперь pipeline знает о layout'е uniform buffer!
		m_graphicsPipeline->Init();

		m_swapchain->CreateFramebuffers(m_renderPass.get());
		m_commandSystem->Init(m_physicalDevice->GetGraphicsQueueFamilyIndex());

		// 2. Создать pool
		vk::DescriptorPoolSize poolSize{vk::DescriptorType::eUniformBuffer, 1};
		vk::DescriptorPoolCreateInfo poolInfo{{}, 1, 1, &poolSize};
		m_cameraDescriptorPool = m_logicalDevice->GetHandle().createDescriptorPool(poolInfo);
		// 3. Создать uniform buffer
		m_cameraBuffer = m_graphicsPipeline->CreateBufferResource(
			m_cameraDescriptorSetLayout, m_cameraDescriptorPool, sizeof(CameraUBO),
			vk::BufferUsageFlagBits::eUniformBuffer, 0);
		// 4. Сохранить дескрипторный набор
		m_cameraDescriptorSet = m_cameraBuffer.descriptorSet;

		m_startTime = std::chrono::high_resolution_clock::now();
    	m_world = world;
	}
	catch (const std::runtime_error &e) {
		LOGF_ERROR("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}
    return true;
}

void VulkanRenderer::Cleanup() {
	if (m_commandSystem) {
		m_commandSystem->Cleanup();  // это ДОЛЖНО освободить все CommandBuffer и CommandPool
		m_commandSystem.reset();     // или delete, если raw ptr
	}

	if (m_graphicsPipeline) {
		m_graphicsPipeline.reset();
	}

	if (vulkanShader) {
		delete vulkanShader;
		vulkanShader = nullptr;
	}

	if (m_swapchain) {
		m_swapchain.reset(); // framebuffers, image views, swapchain
	}

	if (m_renderPass) {
		m_renderPass.reset(); // vk::RenderPass
	}
	if (m_logicalDevice) {
		m_logicalDevice.reset(); // destroyDevice called here
	}

	if (m_physicalDevice) {
		m_physicalDevice.reset();
	}

	if (m_surface) {
		m_instance->GetInstance().destroySurfaceKHR(m_surface);
		m_surface = nullptr;
	}

	if (m_instance) {
		m_instance.reset();
	}
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
    vk::Semaphore imageAvailableSemaphore = m_commandSystem->GetImageAvailableSemaphore();
    vk::Semaphore renderFinishedSemaphore = m_commandSystem->GetRenderFinishedSemaphore();
    uint32_t imageIndex = m_logicalDevice->GetHandle().acquireNextImageKHR(m_swapchain->GetHandle(), UINT64_MAX, imageAvailableSemaphore).value;

    auto cmdBuffer = m_commandSystem->GetCommandBuffer(imageIndex);
    cmdBuffer->reset({});
    cmdBuffer->begin({ vk::CommandBufferUsageFlagBits::eSimultaneousUse });

    vk::ClearValue clearColor = vk::ClearColorValue(std::array<float, 4>{ 0.1f, 0.1f, 0.1f, 0.01f });
    vk::ClearValue clearDepth;
    clearDepth.depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
    std::array<vk::ClearValue, 2> clearValues = { clearColor, clearDepth };
    vk::RenderPassBeginInfo renderPassBeginInfo(
        m_renderPass->GetHandle(),
        m_swapchain->GetFramebuffers()[imageIndex],
        { {0, 0}, m_swapchain->GetSwapExtent() },
        static_cast<uint32_t>(clearValues.size()), clearValues.data()
    );

    cmdBuffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    cmdBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline->GetHandle());

    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - m_startTime;
    float time = elapsed.count();

    glm::mat4 view = m_camera->GetViewMatrix();
    glm::mat4 proj = m_camera->GetProjectionMatrix();
    CameraUBO ubo{view, proj};
    m_graphicsPipeline->UpdateBufferData(m_cameraBuffer, ubo);
    m_graphicsPipeline->BindDescriptorSet(*cmdBuffer, m_cameraDescriptorSet);

    if (m_world) {
        auto actors = m_world->GetActors();
        for (auto& actor : actors) {
            auto transform = actor->GetComponent<UTransformComponent>();
            glm::mat4 model = glm::mat4(1.0f);
            if (transform) {
                model = transform->GetTransformMatrix();
            }
            struct PushConstants {
                glm::mat4 model;
                float time;
            } pushConsts{model, time};
            m_graphicsPipeline->PushConstant(cmdBuffer, vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstants), &pushConsts);
            cmdBuffer->draw(36, 1, 0, 0);
        }
    }

    cmdBuffer->endRenderPass();
    cmdBuffer->end();

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo(
        1, &imageAvailableSemaphore,
        &waitStage,
        1, &(*cmdBuffer),
        1, &renderFinishedSemaphore
    );

    m_logicalDevice->GetGraphicsQueue().submit(submitInfo);

    vk::PresentInfoKHR presentInfo(
        1, &renderFinishedSemaphore,
        1, &m_swapchain->GetHandle(),
        &imageIndex
    );
    m_logicalDevice->GetGraphicsQueue().presentKHR(presentInfo);

    m_logicalDevice->GetGraphicsQueue().waitIdle();
    m_currentFrame = (m_currentFrame + 1) % 2;
}
