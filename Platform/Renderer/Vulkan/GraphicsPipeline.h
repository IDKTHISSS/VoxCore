//
// Created by IDKTHIS on 07.07.2025.
//

#pragma once
#include <vector>
#include <string>
#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "VulkanRenderPass.h"


class GraphicsPipeline {
public:
    GraphicsPipeline(LogicalDevice *logicalDevice, PhysicalDevice *physicalDevice, VulkanRenderPass *renderPass);
    ~GraphicsPipeline();
    bool Init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    [[nodiscard]] VkPipeline GetGraphicsPipeline() const { return m_graphicsPipeline; }

private:
    LogicalDevice* m_logicalDevice;
    PhysicalDevice* m_physicalDevice;
    VulkanRenderPass* m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkShaderModule createShaderModule(std::vector<char> code);

    std::vector<char> readFile(std::string filename);
};


