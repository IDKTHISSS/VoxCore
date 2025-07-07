//
// Created by IDKTHIS on 07.07.2025.
//

#pragma once
#include <vulkan/vulkan.h>

typedef struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    uint32_t computeFamily;
    uint32_t transferFamily;
} QueueFamilyIndices;

class PhysicalDevice;

class LogicalDevice {
public:
    LogicalDevice();
    ~LogicalDevice();
    bool Init(PhysicalDevice* physicalDevice);
    [[nodiscard]] VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
    [[nodiscard]] VkQueue GetPresentQueue() const { return m_presentQueue; }
    [[nodiscard]] VkDevice GetDevice() const { return m_device; }
private:
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkDevice m_device;
};
