//
// Created by IDKTHIS on 07.07.2025.
//

#pragma once
#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanInstance.h"

class PhysicalDevice {
public:
    explicit PhysicalDevice(VulkanInstance* instance);
    ~PhysicalDevice();
    bool Init(VkSurfaceKHR surface);
    [[nodiscard]] uint32_t GetGraphicsQueueFamilyIndex() const { return m_graphicsQueueFamilyIndex; }
    [[nodiscard]] uint32_t GetPresentQueueFamilyIndex() const { return m_presentQueueFamilyIndex; }
    [[nodiscard]] VkPhysicalDevice GetDevice() const { return m_device; }
private:
    VkPhysicalDevice m_device;
    VulkanInstance* m_VulkanInstance;
    uint32_t m_graphicsQueueFamilyIndex;
    uint32_t m_presentQueueFamilyIndex;
};
