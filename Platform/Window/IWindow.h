//
// Created by IDKTHIS on 02.07.2025.
//

#pragma once

#include <string>
#include <cstdint>
#include <array>

struct MouseState {
    int x = 0, y = 0; // позиция
    int dx = 0, dy = 0; // дельта за кадр
    bool buttons[8] = {false}; // до 8 кнопок мыши
};

class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool Create(int width, int height, const std::string& title) = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual bool ShouldClose() const = 0;

    virtual void* GetNativeHandle() = 0;

    // --- Новый API для ввода ---
    virtual const std::array<bool, 512>& GetKeyBuffer() const = 0;
    virtual const MouseState& GetMouseState() const = 0;
    virtual void SetRelativeMouseMode(bool enable) = 0;
    virtual bool IsRelativeMouseMode() const = 0;
    virtual void SetTitle(const std::string& title) = 0;
};
