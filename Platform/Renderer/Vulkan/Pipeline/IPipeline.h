//
// Created by IDKTHIS on 13.07.2025.
//
#pragma once
#include "vulkan/vulkan.hpp"

class IPipeline {
public:
    virtual ~IPipeline() = default;

    virtual bool Init() = 0;
    virtual void Bind(vk::CommandBuffer cmdBuffer) const = 0;
    virtual void Cleanup() = 0;

    virtual void PushConstant(vk::CommandBuffer* cmd, vk::ShaderStageFlags stage, uint32_t offset, uint32_t size, const void* data) = 0;
    virtual void BindDescriptorSet(vk::CommandBuffer cmd, vk::DescriptorSet descriptorSet) = 0;

    [[nodiscard]] virtual vk::Pipeline GetHandle() const = 0;
    [[nodiscard]] virtual vk::PipelineLayout GetLayout() const = 0;
};
