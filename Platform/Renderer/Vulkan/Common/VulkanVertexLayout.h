//
// Created by IDKTHIS on 25.07.2025.
//

#pragma once
#include "Platform/Renderer/Common/IVertexLayout.h"
#include "Core/Common/Vertex.h"
#include <vulkan/vulkan.hpp>

class VulkanVertexLayout : public IVertexLayout {
public:
    vk::VertexInputBindingDescription bindingDesc;
    std::vector<vk::VertexInputAttributeDescription> attributeDescs;

    VulkanVertexLayout() {
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(Vertex);
        bindingDesc.inputRate = vk::VertexInputRate::eVertex;

        attributeDescs.resize(2);
        attributeDescs[0] = { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position) };
        attributeDescs[1] = { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color) };
    }

    void* getNativeBindingDescription() const override {
        return (void*)&bindingDesc;
    }

    void* getNativeAttributeDescriptions() const override {
        return (void*)attributeDescs.data();
    }
};
