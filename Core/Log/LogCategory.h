#pragma once
#include <string_view>

struct LogCategory {
    std::string_view name;

    constexpr LogCategory(std::string_view n) : name(n) {}
};

#define LOG_CATEGORY_DEFAULT LogCategory("Default")
#define LOG_CATEGORY_RENDER  LogCategory("Render")
#define LOG_CATEGORY_SHADER  LogCategory("Shader")
#define LOG_CATEGORY_WORLD   LogCategory("World")
#define LOG_CATEGORY_IO      LogCategory("IO")
