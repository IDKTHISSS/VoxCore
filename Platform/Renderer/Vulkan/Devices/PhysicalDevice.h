//
// Created by IDKTHIS on 14.07.2025.
//

#pragma once

#include "Platform/Renderer/Vulkan/Core/VulkanInstance.h"

class PhysicalDevice {
public:
    PhysicalDevice(VulkanInstance* instance);
    ~PhysicalDevice();

    // Initialize the physical device with the given Vulkan instance
    bool Init(vk::SurfaceKHR surface);

    // Get the physical device handle
    vk::PhysicalDevice& GetHandle() { return m_physicalDevice; }

    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;

    // Get the graphics queue family index
    uint32_t& GetGraphicsQueueFamilyIndex() { return m_graphicsQueueFamilyIndex; }
    uint32_t& GetPresentQueueFamilyIndex() { return m_presentQueueFamilyIndex; }

private:
    VulkanInstance* m_vulkanInstance;
    vk::PhysicalDevice m_physicalDevice; // Handle to the physical device
    uint32_t m_graphicsQueueFamilyIndex = -1; // Index of the graphics queue family
    uint32_t m_presentQueueFamilyIndex = -1; // Index of the present queue family
};
