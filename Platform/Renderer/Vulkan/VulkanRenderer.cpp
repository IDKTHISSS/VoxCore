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
#include "Core/ECS/Components/UMeshComponent.h"
#include <unordered_map>


struct MeshBuffers {
    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexMemory;
    uint32_t indexCount;
};

std::unordered_map<AActor*, MeshBuffers> m_meshBuffers;


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
	if (!m_instance->Init()) {
		return  false;
	}
	VkSurfaceKHR c_surface;
	SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(window->GetNativeHandle()), m_instance->GetInstance(), nullptr, &c_surface); //TODO: Make CreateSurface in IWindow.
	m_surface = c_surface;
	bool isSuccess =
		   m_physicalDevice->Init(m_surface) &&
		   m_logicalDevice->Init(m_physicalDevice.get()) &&
		   m_swapchain->Init(m_surface) &&
		   m_renderPass->Init();
	if (!isSuccess) {
		LOG_ERROR("Vulkan", "Failed to initialize Vulkan components.");
		return false;
	}
	vulkanShader = new VulkanShader(m_logicalDevice.get());
	vulkanShader->LoadFromFile("shaders/shader.glsl");
	vk::DescriptorSetLayoutBinding cameraLayoutBinding{};
	cameraLayoutBinding.binding = 0;
	cameraLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	cameraLayoutBinding.descriptorCount = 1;
	cameraLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	cameraLayoutBinding.pImmutableSamplers = nullptr;
	vk::DescriptorSetLayoutCreateInfo layoutInfo{{}, 1, &cameraLayoutBinding};
	m_cameraDescriptorSetLayout = m_logicalDevice->GetHandle().createDescriptorSetLayout(layoutInfo);
	m_graphicsPipeline->SetShader(vulkanShader)
					  ->SetPushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, 68) // 64 bytes for mat4 + 4 bytes for float
					  ->SetDescriptorSetLayouts({m_cameraDescriptorSetLayout});
	m_graphicsPipeline->Init();
	m_swapchain->CreateFramebuffers(m_renderPass.get());
	if (!m_commandSystem->Init(m_physicalDevice->GetGraphicsQueueFamilyIndex())) {
		return false;
	}
	return true;
}

void VulkanRenderer::Cleanup() {
	LOG_INFO("Vulkan", "Cleaning up Vulkan resources.");
	m_swapchain.reset();
	m_renderPass.reset();
	m_graphicsPipeline.reset();
	m_commandSystem.reset();
	m_logicalDevice.reset();
	m_physicalDevice.reset();
	m_instance->GetInstance().destroySurfaceKHR(m_surface);
	m_instance.reset();
	LOG_INFO("Vulkan", "Vulkan resources cleaned up.");
}


void VulkanRenderer::RenderFrame() {
	//m_commandSystem->BeginFrame();

	//m_commandSystem->BeginRender(m_swapchain->GetFramebuffer(m_commandSystem->GetCurrentFrame()), m_swapchain->GetSwapExtent());
	//m_renderer->Render();  // отрисовка объектов, тут bind pipeline, меши, и т.д.
	//m_commandSystem->EndRender();

	//m_commandSystem->EndFrame();

}
