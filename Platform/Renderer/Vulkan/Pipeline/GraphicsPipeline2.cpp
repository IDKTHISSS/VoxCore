//
// Created by IDKTHIS on 15.07.2025.
//

#include "GraphicsPipeline2.h"

#include <fstream>

#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "Platform/Renderer/Vulkan/RenderPass/VulkanRenderPass.h"
#include "Platform/Renderer/Vulkan/Swapchain/VulkanSwapChain.h"


GraphicsPipeline2::GraphicsPipeline2(VulkanInstance *instance, LogicalDevice *logicalDevice, VulkanSwapChain *swapChain,
                                     VulkanRenderPass *renderPass) : m_vulkanInstance(instance), m_logicalDevice(logicalDevice), m_swapChain(swapChain), m_renderPass(renderPass) {
}

bool GraphicsPipeline2::Init() {
    auto vertCode = readFile("shaders/vert.spv");
    auto fragCode = readFile("shaders/frag.spv");
    vk::ShaderModule vertModule = m_logicalDevice->GetHandle().createShaderModule({ {}, vertCode.size(), (uint32_t*)vertCode.data() });
    vk::ShaderModule fragModule = m_logicalDevice->GetHandle().createShaderModule({ {}, fragCode.size(), (uint32_t*)fragCode.data() });

    vk::PipelineShaderStageCreateInfo stages[] = {
        {{}, vk::ShaderStageFlagBits::eVertex, vertModule, "main"},
        {{}, vk::ShaderStageFlagBits::eFragment, fragModule, "main"}
    };

    vk::PipelineVertexInputStateCreateInfo vertexInput;
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly({}, vk::PrimitiveTopology::eTriangleList);
    vk::Viewport viewport(0, 0, (float)m_swapChain->GetSwapExtent().width, (float)m_swapChain->GetSwapExtent().height, 0, 1);
    vk::Rect2D scissor({0, 0}, m_swapChain->GetSwapExtent());
    vk::PipelineViewportStateCreateInfo viewportState({}, 1, &viewport, 1, &scissor);
    vk::PipelineRasterizationStateCreateInfo rasterizer({}, false, false, vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise);
    vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1);
    vk::PipelineColorBlendAttachmentState colorBlendAttachment(false);
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlending({}, false, vk::LogicOp::eCopy, 1, &colorBlendAttachment);

    // Push constant range for a float time
    vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eFragment, 0, sizeof(float));

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 1, &pushConstantRange);
    m_pipelineLayout = m_logicalDevice->GetHandle().createPipelineLayout(pipelineLayoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo({}, 2, stages, &vertexInput,
        &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling,
        nullptr, &colorBlending, nullptr, m_pipelineLayout, m_renderPass->GetHandle());
    m_pipeline = m_logicalDevice->GetHandle().createGraphicsPipelines({}, pipelineInfo).value[0];
    return true;
}

std::vector<char> GraphicsPipeline2::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open file: " + filename);
    size_t size = (size_t)file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    return buffer;
}
