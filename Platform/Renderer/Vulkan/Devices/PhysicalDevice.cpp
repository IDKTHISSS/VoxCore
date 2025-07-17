//
// Created by IDKTHIS on 14.07.2025.
//

#include "PhysicalDevice.h"

PhysicalDevice::PhysicalDevice(VulkanInstance* instance) :  m_vulkanInstance(instance) {

}

PhysicalDevice::~PhysicalDevice() {
    m_physicalDevice = nullptr;
}

bool PhysicalDevice::Init(vk::SurfaceKHR surface) {
    uint32_t queueIndex = 0;
    auto gpus = m_vulkanInstance->GetInstance().enumeratePhysicalDevices();
    for (auto& gpu : gpus) {
        auto props = gpu.getQueueFamilyProperties();
        for (uint32_t i = 0; i < props.size(); ++i) {
            if (!(props[i].queueFlags & vk::QueueFlagBits::eGraphics) && gpu.getSurfaceSupportKHR(i, surface))
                continue;
            m_physicalDevice = gpu;
            m_graphicsQueueFamilyIndex = i;
            break;
        }
        if (m_graphicsQueueFamilyIndex != -1) {
            break;
        }
    }
    return true;
}
