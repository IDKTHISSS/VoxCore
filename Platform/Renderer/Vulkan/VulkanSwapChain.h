//
// Created by IDKTHIS on 07.07.2025.
//

#pragma once
#include <vector>

#include "vulkan/vulkan.h"


class VulkanRenderPass;
class LogicalDevice;
class PhysicalDevice;

class VulkanSwapChain {
public:
    VulkanSwapChain();
    ~VulkanSwapChain();
    bool Init(PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height);
    bool CreateFramebuffers(LogicalDevice* logicalDevice, VulkanRenderPass* renderPass);
    [[nodiscard]] VkFormat GetImageFormat() const { return m_swapChainImageFormat; }
    [[nodiscard]] VkSwapchainKHR GetSwapChain() const { return m_swapChain; }
    [[nodiscard]] const std::vector<VkImageView>& GetSwapChainImageViews() const { return m_swapChainImageViews; }
    [[nodiscard]] const VkFramebuffer* GetSwapChainFramebuffer(uint32_t index) { return &m_swapChainFramebuffers[index]; }
    [[nodiscard]] VkExtent2D GetSwapChainExtent() const { return m_swapChainExtent; }

private:
    bool CreateImageView(LogicalDevice* logicalDevice);
    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;
};
