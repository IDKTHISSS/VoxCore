//
// Created by IDKTHIS on 02.07.2025.
//

#pragma once

#include <string>
#include <cstdint>

class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool Create(int width, int height, const std::string& title) = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual bool ShouldClose() const = 0;

    virtual void* GetNativeHandle() = 0;
};

