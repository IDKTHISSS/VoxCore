//
// Created by IDKTHIS on 16.07.2025.
//

#include "GraphicsPipeline.h"

#include "glm/glm.hpp"
#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "Platform/Renderer/Vulkan/Devices/PhysicalDevice.h"
#include "Platform/Renderer/Vulkan/RenderPass/VulkanRenderPass.h"
#include "Platform/Renderer/Vulkan/Camera.h"
#include "Platform/Renderer/Vulkan/VulkanRenderer.h"

GraphicsPipeline::GraphicsPipeline(LogicalDevice* device, PhysicalDevice* physicalDevice, VulkanRenderPass* renderPass,
    VulkanSwapChain* swapChain)
    : m_logicalDevice(device), m_physicalDevice(physicalDevice), m_renderPass(renderPass), m_swapChain(swapChain) {}

void GraphicsPipeline::SetShader(VulkanShader* shader) {
    m_shader = shader;
}

void GraphicsPipeline::SetPushConstantRange(vk::ShaderStageFlags stages, uint32_t offset, uint32_t size) {
    m_pushConstantRange = vk::PushConstantRange(stages, offset, size);
}

void GraphicsPipeline::SetDescriptorSetLayouts(const std::vector<vk::DescriptorSetLayout>& layouts) {
    m_descriptorSetLayouts = layouts;
}

bool GraphicsPipeline::Init() {
    if (!m_shader) throw std::runtime_error("Shader is not set for pipeline");

    auto vertModule = m_shader->GetShaderModule("vertex");
    auto fragModule = m_shader->GetShaderModule("fragment");

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertModule, "main"),
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragModule, "main")
    };

    auto bindingDesc = Vertex::getBindingDescription();
    auto attrDesc = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInput({}, 1, &bindingDesc, static_cast<uint32_t>(attrDesc.size()), attrDesc.data());
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly({}, vk::PrimitiveTopology::eTriangleList);
    vk::Viewport viewport(0, 0, (float)m_swapChain->GetSwapExtent().width, (float)m_swapChain->GetSwapExtent().height, 0, 1);
    vk::Rect2D scissor({0, 0}, m_swapChain->GetSwapExtent());
    vk::PipelineViewportStateCreateInfo viewportState({}, 1, &viewport, 1, &scissor);
    vk::PipelineRasterizationStateCreateInfo rasterizer({}, false, false,
                        vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise);
    rasterizer.setLineWidth(1.0f);
    vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1);
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlending({}, false, vk::LogicOp::eCopy, 1, &colorBlendAttachment);

    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.setDepthTestEnable(VK_TRUE);
    depthStencil.setDepthWriteEnable(VK_TRUE);
    depthStencil.setDepthCompareOp(vk::CompareOp::eLess);
    depthStencil.setDepthBoundsTestEnable(VK_FALSE);
    depthStencil.setStencilTestEnable(VK_FALSE);

    vk::PipelineLayoutCreateInfo layoutInfo({},
        static_cast<uint32_t>(m_descriptorSetLayouts.size()), m_descriptorSetLayouts.data(),
        1, &m_pushConstantRange);

    m_pipelineLayout = m_logicalDevice->GetHandle().createPipelineLayout(layoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo({},
        static_cast<uint32_t>(shaderStages.size()), shaderStages.data(),
        &vertexInput, &inputAssembly, nullptr,
        &viewportState, &rasterizer, &multisampling,
        &depthStencil, &colorBlending, nullptr,
        m_pipelineLayout, m_renderPass->GetHandle());

    m_pipeline = m_logicalDevice->GetHandle().createGraphicsPipelines({}, pipelineInfo).value[0];
    return true;
}

void GraphicsPipeline::Bind(vk::CommandBuffer cmdBuffer) const {
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
}

void GraphicsPipeline::PushConstant(vk::CommandBuffer* cmd, vk::ShaderStageFlags stage, uint32_t offset, uint32_t size, const void* data) {
    cmd->pushConstants(m_pipelineLayout, stage, offset, size, data);
}

void GraphicsPipeline::BindDescriptorSet(vk::CommandBuffer cmd, vk::DescriptorSet descriptorSet) {
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}

void GraphicsPipeline::Cleanup() {
    if (m_pipeline) m_logicalDevice->GetHandle().destroyPipeline(m_pipeline);
    if (m_pipelineLayout) m_logicalDevice->GetHandle().destroyPipelineLayout(m_pipelineLayout);
}

BufferResource GraphicsPipeline::CreateBufferResource(vk::DescriptorSetLayout layout, vk::DescriptorPool pool, vk::DeviceSize size,
                                                      vk::BufferUsageFlags usage, uint32_t binding) {
    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);
    vk::Buffer buffer = m_logicalDevice->GetHandle().createBuffer(bufferInfo);

    vk::MemoryRequirements memReq = m_logicalDevice->GetHandle().getBufferMemoryRequirements(buffer);
    uint32_t memTypeIndex = FindMemoryType(memReq.memoryTypeBits,
                                           vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    vk::MemoryAllocateInfo allocInfo(memReq.size, memTypeIndex);
    vk::DeviceMemory memory = m_logicalDevice->GetHandle().allocateMemory(allocInfo);
    m_logicalDevice->GetHandle().bindBufferMemory(buffer, memory, 0);

    vk::DescriptorSetAllocateInfo allocInfoSet(pool, 1, &layout);
    vk::DescriptorSet descriptorSet = m_logicalDevice->GetHandle().allocateDescriptorSets(allocInfoSet).front();

    vk::DescriptorBufferInfo descBufferInfo(buffer, 0, size);
    vk::WriteDescriptorSet write(descriptorSet, binding, 0, 1,
                                 usage == vk::BufferUsageFlagBits::eUniformBuffer ?
                                 vk::DescriptorType::eUniformBuffer : vk::DescriptorType::eStorageBuffer,
                                 nullptr, &descBufferInfo);
    m_logicalDevice->GetHandle().updateDescriptorSets(1, &write, 0, nullptr);

    return { buffer, memory, descBufferInfo, descriptorSet };
}

template<typename T>
void GraphicsPipeline::UpdateBufferData(const BufferResource& resource, const T& data) {
    void* mapped = m_logicalDevice->GetHandle().mapMemory(resource.memory, 0, sizeof(T));
    std::memcpy(mapped, &data, sizeof(T));
    m_logicalDevice->GetHandle().unmapMemory(resource.memory);
}

uint32_t GraphicsPipeline::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    auto memProperties = m_physicalDevice->GetHandle().getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

// Явная инстанциация шаблона для CameraUBO
template void GraphicsPipeline::UpdateBufferData<CameraUBO>(const BufferResource&, const CameraUBO&);
