//
// Created by IDKTHIS on 07.07.2025.
//

#include "PhysicalDevice.h"

#include <vector>


PhysicalDevice::PhysicalDevice(VulkanInstance* instance) :
            m_VulkanInstance(instance),
            m_device(VK_NULL_HANDLE)
{
}

PhysicalDevice::~PhysicalDevice() {
}

bool PhysicalDevice::Init(VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VulkanInstance->GetInstance(), &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VulkanInstance->GetInstance(), &deviceCount, devices.data());
    if (devices.empty()) {
        return false;
    }
    m_device = devices[0];
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyCount, queueFamilies.data());

    m_graphicsQueueFamilyIndex = 0;
    m_presentQueueFamilyIndex = 0;

    for (size_t i = 0; i < queueFamilies.size(); i++)
    {
        VkQueueFamilyProperties queueFamily = queueFamilies[i];

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_device, i, surface, &presentSupport);
        if (presentSupport) {
           m_presentQueueFamilyIndex = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_graphicsQueueFamilyIndex = i;
            break;
        }

    }
    return true;
}
