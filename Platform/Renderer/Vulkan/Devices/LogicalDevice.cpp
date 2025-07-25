//
// Created by IDKTHIS on 14.07.2025.
//

#include "LogicalDevice.h"

#include "PhysicalDevice.h"
#include "Core/Log/Logger.h"

LogicalDevice::LogicalDevice(VulkanInstance *instance) : m_vulkanInstance(instance) {
    // Constructor implementation
}

LogicalDevice::~LogicalDevice() {
    if (m_logicalDevice) {
        m_logicalDevice.destroy();
    }
}

bool LogicalDevice::Init(PhysicalDevice* physicalDevice) {
    float priority = 1.0f;
    vk::DeviceQueueCreateInfo queueInfo({}, physicalDevice->GetGraphicsQueueFamilyIndex(), 1, &priority);
    vk::DeviceCreateInfo deviceInfo(
        {}, 1, &queueInfo,
        0, nullptr,
        static_cast<uint32_t>(m_deviceExtensions.size()), m_deviceExtensions.data()
    );
    m_logicalDevice = physicalDevice->GetHandle().createDevice(deviceInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_logicalDevice);
    m_graphicsQueue = m_logicalDevice.getQueue(physicalDevice->GetGraphicsQueueFamilyIndex(), 0);
    LOG_INFO("Vulkan", "Logical device created with {} extensions.", m_deviceExtensions.size());
    return true;
}
