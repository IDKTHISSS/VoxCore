//
// Created by IDKTHIS on 02.07.2025.
//

#pragma once
#include "IRenderer.h"
#include <memory>
#include <vector>

#include "../Window/IWindow.h"
#include <vulkan/vulkan.h>

// Indices (locations) of Queue Families (if they exist at all) TEMP HERE
struct QueueFamilyIndices {
    int graphicsFamily = -1;			// Location of Graphics Queue Family

    // Check if queue families are valid
    [[nodiscard]] bool isValid() const
    {
        return graphicsFamily >= 0;
    }
};

class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    bool Init(IWindow* window) override;
    // Add more Vulkan-specific methods as needed

private:
    // Vulkan instance, device, etc. (placeholders for now)
    VkInstance m_instance = nullptr;
    VkDevice m_device = nullptr;
    void* m_surface = nullptr;

    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;
    VkQueue graphicsQueue;


    // Vulkan Functions
    // - Create Functions
    void createInstance();
    void createLogicalDevice();
    void Cleanup() override;
    // - Get Functions
    void getPhysicalDevice();

    // - Support Functions
    // -- Checker Functions
    bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtensions);
    bool checkDeviceSuitable(VkPhysicalDevice device);

    // -- Getter Functions
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};
