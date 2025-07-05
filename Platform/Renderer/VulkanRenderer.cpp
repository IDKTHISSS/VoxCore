//
// Created by IDKTHIS on 02.07.2025.
//

#include "VulkanRenderer.h"
#include <vulkan/vulkan.h>
#include <cassert>

VulkanRenderer::VulkanRenderer() = default;
VulkanRenderer::~VulkanRenderer() = default;

bool VulkanRenderer::Init(IWindow *window) {
    // Placeholder: Vulkan instance creation
    // In the future, use window->GetNativeHandle() to create Vulkan surface
    assert(window != nullptr);
    // Example: retrieve native handle
    void* nativeHandle = window->GetNativeHandle();
    // Vulkan initialization logic will go here
    return true;
}
