//
// Created by IDKTHIS on 07.07.2025.
//

#include "VulkanInstance.h"

#include <vector>

#include "../../../../Core/Log/Logger.h"
#include "SDL3/SDL_vulkan.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
VulkanInstance::VulkanInstance() {

    m_appInfo.pApplicationName = "VoxCore Engine base";					// Custom name of the application
    m_appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);		// Custom version of the application
    m_appInfo.pEngineName = "VoxCore";							// Custom engine name
    m_appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);			// Custom engine version
    m_appInfo.apiVersion = VK_API_VERSION_1_3;					// The Vulkan Version

    m_instance = nullptr;
}

VulkanInstance::~VulkanInstance() {
    if (m_instance) {
        m_instance.destroy();
    }
}

bool VulkanInstance::Init() {
    vk::detail::DynamicLoader loader;
    PFN_vkGetInstanceProcAddr getInstanceProcAddr = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(getInstanceProcAddr);
    uint32_t extCount = 0;
    char const* const* exts = SDL_Vulkan_GetInstanceExtensions(&extCount);
    std::vector<const char*> extensions(exts, exts + extCount);
    vk::InstanceCreateInfo instanceInfo({}, &m_appInfo, 0, nullptr, extCount, extensions.data());
    const std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instanceInfo.ppEnabledLayerNames = layers.data();
    m_instance = vk::createInstance(instanceInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);
    LOG_INFO("Vulkan", "Vulkan instance created with {} extensions and {} layers.",
             extensions.size(), layers.size());
    return true;
}
