cmake_minimum_required(VERSION 3.16)
project(VoxCore)

set(CMAKE_CXX_STANDARD 17)

enable_testing()  # Включает поддержку CTest

# === GoogleTest ===
add_subdirectory(libs/googletest)

# === SDL3 ===
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(libs/SDL)

# === Vulkan ===
find_package(Vulkan REQUIRED)

# === GLM ===
set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm")

# === ImGui ===
file(GLOB IMGUI_SOURCES
        libs/imgui/*.cpp
        libs/imgui/backends/imgui_impl_sdl3.cpp
        libs/imgui/backends/imgui_impl_vulkan.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
        libs/imgui
        libs/imgui/backends
        libs/SDL/include
        ${GLM_INCLUDE_DIR}
        ${Vulkan_INCLUDE_DIRS}
)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan)

# === VoxCore Engine ===
file(GLOB_RECURSE ENGINE_SRC
        Application/*.cpp
        Core/**/*.cpp
        Platform/**/*.cpp
)
file(GLOB_RECURSE ENGINE_HDR
        Application/*.h
        Core/**/*.h
        Platform/**/*.h
)

add_library(VoxCore STATIC ${ENGINE_SRC} ${ENGINE_HDR}
        Platform/Renderer/Vulkan/VulkanInstance.cpp
        Platform/Renderer/Vulkan/VulkanInstance.h
        Platform/Renderer/Vulkan/PhysicalDevice.cpp
        Platform/Renderer/Vulkan/PhysicalDevice.h
        Platform/Renderer/Vulkan/LogicalDevice.cpp
        Platform/Renderer/Vulkan/LogicalDevice.h
        Platform/Renderer/Vulkan/VulkanSwapChain.cpp
        Platform/Renderer/Vulkan/VulkanSwapChain.h
        Platform/Renderer/Vulkan/VulkanRenderPass.cpp
        Platform/Renderer/Vulkan/VulkanRenderPass.h
        Platform/Renderer/Vulkan/GraphicsPipeline.cpp
        Platform/Renderer/Vulkan/GraphicsPipeline.h
        Platform/Renderer/Vulkan/CommandSystem.cpp
        Platform/Renderer/Vulkan/CommandSystem.h
)
target_include_directories(VoxCore PUBLIC
        libs/SDL/include
        ${GLM_INCLUDE_DIR}
)

target_link_libraries(VoxCore
        imgui
        SDL3-static
)

# === Unit Tests ===
add_executable(VoxCoreTests

        Tests/Basics/SDL_Window_Tests.cpp

)
target_include_directories(VoxCoreTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}  # <- даёт доступ к Application/, Core/, Platform/
        libs/SDL/include
        ${GLM_INCLUDE_DIR}
)
target_link_libraries(VoxCoreTests
        VoxCore
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(VoxCoreTests)
