//
// Created by IDKTHIS on 13.07.2025.
//

#pragma once
#include "vulkan/vulkan_core.h"


struct VulkanShader {
    VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
    VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;
};
