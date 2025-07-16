//
// Created by IDKTHIS on 16.07.2025.
//

#pragma once
#include "IPipeline.h"
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
    ~GraphicsPipeline() override = default;

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

private:
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

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
