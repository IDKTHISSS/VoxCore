//
// Created by IDKTHIS on 02.07.2025.
//

#pragma once
#include "IRenderer.h"
#include <memory>
#include "../Window/IWindow.h"

class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    bool Init(IWindow* window) override;
    // Add more Vulkan-specific methods as needed

private:
    // Vulkan instance, device, etc. (placeholders for now)
    void* m_instance = nullptr;
    void* m_device = nullptr;
    void* m_surface = nullptr;
};
