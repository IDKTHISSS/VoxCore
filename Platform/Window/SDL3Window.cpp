//
// Created by IDKTHIS on 02.07.2025.
//

#include "SDL3Window.h"
#include <SDL3/SDL.h>

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
        LOGF_ERROR("Failed to initialize SDL - %s", SDL_GetError());
        return false;
    }
    m_window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN);
    return m_window != nullptr;
}

void SDL3Window::PollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_shouldClose = true;
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
