//
// Created by IDKTHIS on 05.07.2025.
//
#include <thread>
#include <gtest/gtest.h>
#include "../../Platform/Window/SDL3/SDL3Window.h"

TEST(SDLWindowTest, CanCreateWindow) {
    std::unique_ptr<IWindow> window = std::make_unique<SDL3Window>();
    window->Create(800, 600, "Test Window");
    std::this_thread::sleep_for(std::chrono::seconds(15));
    EXPECT_TRUE(window->GetNativeHandle() != nullptr) << "Window handle should not be null after creation";
}