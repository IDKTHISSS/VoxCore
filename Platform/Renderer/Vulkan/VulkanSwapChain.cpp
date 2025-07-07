//
// Created by IDKTHIS on 07.07.2025.
//

#include "VulkanSwapChain.h"

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanRenderPass.h"
#include "../../../Core/Log/Logger.h"


VulkanSwapChain::VulkanSwapChain() {
}

VulkanSwapChain::~VulkanSwapChain() {
}

bool VulkanSwapChain::Init(PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkSurfaceKHR surface,
                           uint32_t width, uint32_t height) {
    uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->GetDevice(), surface, &presentModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->GetDevice(), surface, &presentModeCount, presentModes.data());

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (VkPresentModeKHR avalibleMode : presentModes)
	{
		if (avalibleMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = avalibleMode;
			break;
		}
	}

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->GetDevice(), surface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> swapChainFormats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->GetDevice(), surface, &formatCount, swapChainFormats.data());


	VkSurfaceFormatKHR surfaceFormat = swapChainFormats.data()[0];
	for (VkSurfaceFormatKHR avalibleFormat : swapChainFormats)
	{
		if (avalibleFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && avalibleFormat.format == VK_FORMAT_B8G8R8A8_SRGB) {
			surfaceFormat = avalibleFormat;
			break;
		}
	}

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->GetDevice(), surface, &capabilities);
	VkExtent2D extent = capabilities.currentExtent;
	uint32_t imageCount = capabilities.minImageCount + 1;


	VkSwapchainCreateInfoKHR swapChainCreateInfo{};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surface;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.imageArrayLayers = 1;

	uint32_t queueFamiliyIndices[] = { physicalDevice->GetGraphicsQueueFamilyIndex(), physicalDevice->GetPresentQueueFamilyIndex() };
	if (physicalDevice->GetGraphicsQueueFamilyIndex() != physicalDevice->GetPresentQueueFamilyIndex()) {

		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamiliyIndices;
	}
	else {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.imageExtent = extent;
	swapChainCreateInfo.minImageCount = imageCount;
	swapChainCreateInfo.preTransform = capabilities.currentTransform;
	swapChainCreateInfo.imageFormat = surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;

	VkResult res = vkCreateSwapchainKHR(logicalDevice->GetDevice(), &swapChainCreateInfo, nullptr, &m_swapChain);
	if (res != VK_SUCCESS) {
		LOGF_ERROR("Failed to create swapchain! VkResult = %d", res);
		return false;
	}

	uint32_t swapChainImageCount;
	vkGetSwapchainImagesKHR(logicalDevice->GetDevice(), m_swapChain, &swapChainImageCount, nullptr);
	m_swapChainImages.resize(swapChainImageCount);
	vkGetSwapchainImagesKHR(logicalDevice->GetDevice(), m_swapChain, &swapChainImageCount, m_swapChainImages.data());

	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
	CreateImageView(logicalDevice);

	return true;
}

bool VulkanSwapChain::CreateImageView(LogicalDevice* logicalDevice) {
	m_swapChainImageViews.resize(m_swapChainImages.size());

	for (size_t i = 0; i < m_swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_swapChainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = m_swapChainImageFormat;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		vkCreateImageView(logicalDevice->GetDevice(), &imageViewCreateInfo, nullptr, &m_swapChainImageViews[i]);
	}
	return true;
}

bool VulkanSwapChain::CreateFramebuffers(LogicalDevice *logicalDevice, VulkanRenderPass* renderPass) {
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());
	for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { m_swapChainImageViews[i] };
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass->GetRenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = m_swapChainExtent.width;
		framebufferCreateInfo.height = m_swapChainExtent.height;
		framebufferCreateInfo.layers = 1;
		vkCreateFramebuffer(logicalDevice->GetDevice(), &framebufferCreateInfo, nullptr, &m_swapChainFramebuffers[i]);
	}
	return true;
}
