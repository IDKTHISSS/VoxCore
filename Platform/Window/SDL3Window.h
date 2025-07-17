//
// Created by IDKTHIS on 02.07.2025.
//

#ifndef SDL3WINDOW_H
#define SDL3WINDOW_H
#include <array>
#include <string>

#include "IWindow.h"
#include "SDL3/SDL_video.h"


class SDL3Window : public IWindow {
public:
    SDL3Window();
    ~SDL3Window() override;

    bool Create(int width, int height, const std::string& title) override;
    void PollEvents() override;
    void SwapBuffers() override;
    bool ShouldClose() const override;
    void* GetNativeHandle() override;
    const std::array<bool, 512>& GetKeyBuffer() const override { return m_keyBuffer; }
    const MouseState& GetMouseState() const override { return m_mouseState; }
    void SetRelativeMouseMode(bool enable) override;
    bool IsRelativeMouseMode() const override;
    void SetTitle(const std::string& title) override;

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;
    std::array<bool, 512> m_keyBuffer{};
    MouseState m_mouseState;
    bool m_relativeMouseMode = false;
};



#endif //SDL3WINDOW_H
