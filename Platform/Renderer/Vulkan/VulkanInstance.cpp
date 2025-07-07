//
// Created by IDKTHIS on 07.07.2025.
//

#include "VulkanInstance.h"

#include <vector>

#include "../../../Core/Log/Logger.h"
#include "SDL3/SDL_vulkan.h"

VulkanInstance::VulkanInstance() {
    m_appInfo = {};
    m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    m_appInfo.pApplicationName = "VoxCore Engine base";					// Custom name of the application
    m_appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);		// Custom version of the application
    m_appInfo.pEngineName = "VoxCore";							// Custom engine name
    m_appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);			// Custom engine version
    m_appInfo.apiVersion = VK_API_VERSION_1_0;					// The Vulkan Version

    m_instance = nullptr;
}

VulkanInstance::~VulkanInstance() {
    vkDestroyInstance(m_instance, nullptr);
}

bool VulkanInstance::Init() {
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &m_appInfo;
    uint32_t sdlExtensionCout = 0;
    char const * const * sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCout);
    std::vector<const char*> extensions(sdlExtensions, sdlExtensions + sdlExtensionCout);
    createInfo.enabledExtensionCount = sdlExtensionCout;
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkResult res = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (res != VK_SUCCESS) {
        LOGF_ERROR("Failed to create Vulkan instance! Error code: %d", res);
        return false;
    }
    return true;
}
