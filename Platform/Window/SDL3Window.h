//
// Created by IDKTHIS on 02.07.2025.
//

#ifndef SDL3WINDOW_H
#define SDL3WINDOW_H
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

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;
};



#endif //SDL3WINDOW_H
