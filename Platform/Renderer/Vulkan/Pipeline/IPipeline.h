//
// Created by IDKTHIS on 13.07.2025.
//
#pragma once
#include "vulkan/vulkan.hpp"

class IPipeline {
public:
    virtual ~IPipeline() = default;

    virtual bool Init() = 0;
    virtual void Bind(void* data) const = 0;
    virtual void Cleanup() = 0;
};
