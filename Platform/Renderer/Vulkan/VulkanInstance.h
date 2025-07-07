//
// Created by IDKTHIS on 07.07.2025.
//

#pragma once
#include <vulkan/vulkan.h>


class VulkanInstance {
public:
    VulkanInstance();
    ~VulkanInstance();

    bool Init();
    [[nodiscard]] VkInstance GetInstance() const { return m_instance; }

private:
    VkApplicationInfo m_appInfo;
    VkInstance m_instance;
};
