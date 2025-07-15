//
// Created by IDKTHIS on 13.07.2025.
//

#pragma once
#include "vulkan/vulkan_core.h"

class IPipeline {
public:
    virtual ~IPipeline() = default;
    virtual bool Init() = 0;
    virtual void Bind(VkCommandBuffer cmdBuffer) const = 0;

    [[nodiscard]] virtual VkPipeline GetHandle() const = 0;
    [[nodiscard]] virtual VkPipelineLayout GetLayout() const = 0;
};
