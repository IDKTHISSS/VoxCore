//
// Created by IDKTHIS on 25.07.2025.
//

#pragma once

class IVertexLayout {
public:
    virtual ~IVertexLayout() = default;

    virtual void* getNativeBindingDescription() const = 0;
    virtual void* getNativeAttributeDescriptions() const = 0;
};