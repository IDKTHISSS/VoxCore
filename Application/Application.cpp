//
// Created by IDKTHIS on 02.07.2025.
//

// Engine/Core/Application/Application.cpp

#include "Application.h"
#include "../Platform/Window/SDL3Window.h"
#include "../Platform/Renderer/VulkanRenderer.h"
#include <chrono>
#include <utility>

#include "../Core/CVar/CVar.h"
#include "../Core/Log/Logger.h"

using namespace Engine;

CONVAR("w_title", "VoxCore base", "Game window title", CVAR_RUNTIME_ONLY);
CONVAR("w_size_width", 1920, "Game window width", CVAR_ARCHIVE);
CONVAR("w_size_height", 1080, "Game window height", CVAR_ARCHIVE);

Application::Application() = default;

Application::Application(std::string title, int width, int height) {
    SET_CVAR("w_title", title);
    SET_CVAR("w_size_width", width);
    SET_CVAR("w_size_height", height);
}

Application::~Application() = default;

void Application::Run() {
    Core::Log::Logger::Get().EnableColor(false);
    LOG_INFO("Initializing engine.");
    Init();
    LOG_INFO("Initialization successful.");
    LOG_INFO("Creating window.");
    window = std::make_unique<SDL3Window>();
    if (!window->Create(GET_CVAR(int, "w_size_width"), GET_CVAR(int, "w_size_height"), GET_CVAR(std::string, "w_title"))) {
        LOG_ERROR("Failed to create window.");
        return;
    }
    LOG_INFO("Creating Vulkan renderer.");
    renderer = std::make_unique<VulkanRenderer>();
    if (!renderer->Init(window.get())) {
        LOG_ERROR("Failed to initialize Vulkan renderer.");
        return;
    }
    auto lastTime = std::chrono::high_resolution_clock::now();
    LOG_INFO("Starting main loop.");
    while (!window->ShouldClose()) {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        window->PollEvents();
        Update(dt);
        // renderer->RenderFrame(); // Placeholder for future rendering
        window->SwapBuffers();
    }
    Shutdown();
}

// Initializes application resources
void Application::Init() {
    // To be overridden in derived classes
}

// Updates application state (every frame)
void Application::Update(float dt) {
    // To be overridden in derived classes
}

// Shuts down the application and releases resources
void Application::Shutdown() {
    // To be overridden in derived classes
}
