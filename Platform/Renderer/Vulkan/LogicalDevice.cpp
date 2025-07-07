//
// Created by IDKTHIS on 07.07.2025.
//

#include "LogicalDevice.h"

#include <set>

#include "../../../Core/Log/Logger.h"
#include "PhysicalDevice.h"


LogicalDevice::LogicalDevice() {
}

LogicalDevice::~LogicalDevice() {
}

bool LogicalDevice::Init(PhysicalDevice* physicalDevice) {
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		physicalDevice->GetGraphicsQueueFamilyIndex(),
		physicalDevice->GetPresentQueueFamilyIndex()
	};

	float queuePriority = 1.0f;
	for (const uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	VkResult result = vkCreateDevice(physicalDevice->GetDevice(), &deviceCreateInfo, nullptr, &m_device);
	if (result != VK_SUCCESS) {
		//std::cerr << "Failed to create logical device! VkResult: " << result << std::endl;
		return false;
	}

	vkGetDeviceQueue(m_device, physicalDevice->GetGraphicsQueueFamilyIndex(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, physicalDevice->GetPresentQueueFamilyIndex(), 0, &m_presentQueue);

	return true;
}

