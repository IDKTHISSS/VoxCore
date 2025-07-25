//
// Created by IDKTHIS on 02.07.2025.
//

#include "SDL3Window.h"
#include <SDL3/SDL.h>
#include <algorithm>

#include "../../Core/Log/Logger.h"

SDL3Window::SDL3Window() = default;

SDL3Window::~SDL3Window() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
}

bool SDL3Window::Create(int width, int height, const std::string &title) {
    int SDLInitStatus = SDL_Init(SDL_INIT_VIDEO);
    if (SDLInitStatus != 1) {
        LOG_FATAL("Window", "Failed to initialize SDL - {}", SDL_GetError());
        return false;
    }
    m_window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN);
    return m_window != nullptr;
}

void SDL3Window::PollEvents() {
    // Сброс дельты мыши
    m_mouseState.dx = 0;
    m_mouseState.dy = 0;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_shouldClose = true;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode < 512) {
            m_keyBuffer[event.key.scancode] = true;
        }
        if (event.type == SDL_EVENT_KEY_UP && event.key.scancode < 512) {
            m_keyBuffer[event.key.scancode] = false;
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button < 8) {
            m_mouseState.buttons[event.button.button] = true;
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button < 8) {
            m_mouseState.buttons[event.button.button] = false;
        }
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            m_mouseState.dx += event.motion.xrel;
            m_mouseState.dy += event.motion.yrel;
            m_mouseState.x = event.motion.x;
            m_mouseState.y = event.motion.y;
        }
    }
}

void SDL3Window::SwapBuffers() {
    // No-op for Vulkan, handled by renderer
}

bool SDL3Window::ShouldClose() const {
    return m_shouldClose;
}

void *SDL3Window::GetNativeHandle() {
    return static_cast<void*>(m_window);
}

void SDL3Window::SetRelativeMouseMode(bool enable) {
    SDL_SetWindowMouseGrab(m_window, enable);
    enable ? SDL_HideCursor() : SDL_ShowCursor();
    SDL_SetWindowRelativeMouseMode(m_window, enable);
    m_relativeMouseMode = enable;
}

bool SDL3Window::IsRelativeMouseMode() const {
    return m_relativeMouseMode;
}

void SDL3Window::SetTitle(const std::string& title) {
    if (m_window) {
        SDL_SetWindowTitle(m_window, title.c_str());
    }
}
