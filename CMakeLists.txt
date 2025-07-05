cmake_minimum_required(VERSION 3.16)
project(VoxCore)

set(CMAKE_CXX_STANDARD 17)

# Отключаем ненужные подсистемы SDL для Windows
set(SDL_STATIC ON CACHE BOOL "" FORCE)

# Vulkan
find_package(Vulkan REQUIRED)

# SDL3
add_subdirectory(libs/SDL)

# GLM
set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm")

# ImGui
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

# Движок
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

add_library(VoxCore STATIC ${ENGINE_SRC} ${ENGINE_HDR})
target_include_directories(VoxCore PUBLIC
        libs/SDL/include
        ${GLM_INCLUDE_DIR}
)
target_link_libraries(VoxCore
        imgui
        SDL3-static
)
