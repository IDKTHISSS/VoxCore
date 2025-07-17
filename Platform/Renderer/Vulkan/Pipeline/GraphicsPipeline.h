//
// Created by IDKTHIS on 16.07.2025.
//

#pragma once
#include "IPipeline.h"
#include "glm/vec3.hpp"
#include "Platform/Renderer/Vulkan/Core/VulkanShader.h"
#include "Platform/Renderer/Vulkan/Swapchain/VulkanSwapChain.h"


class PhysicalDevice;
class LogicalDevice;
class VulkanRenderPass;

struct BufferResource {
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vk::DescriptorBufferInfo bufferInfo;
    vk::DescriptorSet descriptorSet;
};


class GraphicsPipeline : public IPipeline {
public:
    GraphicsPipeline(LogicalDevice* device, PhysicalDevice* physicalDevice, VulkanRenderPass* renderPass, VulkanSwapChain* swapChain);
    ~GraphicsPipeline() override {
        GraphicsPipeline::Cleanup();
    };

    bool Init() override;
    void Bind(vk::CommandBuffer cmdBuffer) const override;
    void Cleanup() override;

    void PushConstant(vk::CommandBuffer* cmd, vk::ShaderStageFlags stage, uint32_t offset, uint32_t size, const void* data) override;
    void BindDescriptorSet(vk::CommandBuffer cmd, vk::DescriptorSet descriptorSet) override;

    vk::Pipeline GetHandle() const override { return m_pipeline; }
    vk::PipelineLayout GetLayout() const override { return m_pipelineLayout; }

    void SetShader(VulkanShader* shader);
    void SetPushConstantRange(vk::ShaderStageFlags stages, uint32_t offset, uint32_t size);
    void SetDescriptorSetLayouts(const std::vector<vk::DescriptorSetLayout>& layouts);

    BufferResource CreateBufferResource(vk::DescriptorSetLayout layout, vk::DescriptorPool pool, vk::DeviceSize size,
                                        vk::BufferUsageFlags usage, uint32_t binding);
    template<typename T>
    void UpdateBufferData(const BufferResource& resource, const T& data);
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

private:

    LogicalDevice* m_logicalDevice;
    PhysicalDevice* m_physicalDevice;
    VulkanRenderPass* m_renderPass;
    VulkanSwapChain* m_swapChain;
    VulkanShader* m_shader = nullptr;

    vk::Pipeline m_pipeline;
    vk::PipelineLayout m_pipelineLayout;

    vk::PushConstantRange m_pushConstantRange{};
    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(Vertex);
        bindingDesc.inputRate = vk::VertexInputRate::eVertex;
        return bindingDesc;
    }
    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 2> attr{};
        attr[0].binding = 0;
        attr[0].location = 0;
        attr[0].format = vk::Format::eR32G32B32Sfloat;
        attr[0].offset = offsetof(Vertex, position);
        attr[1].binding = 0;
        attr[1].location = 1;
        attr[1].format = vk::Format::eR32G32B32Sfloat;
        attr[1].offset = offsetof(Vertex, color);
        return attr;
    }
};
