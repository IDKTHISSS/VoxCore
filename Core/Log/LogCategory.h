#pragma once
#include <string>

struct LogCategory {
    const char* name;

    constexpr LogCategory(const char* n) : name(n) {}
};

#define LOG_CATEGORY_DEFAULT LogCategory("Default")
#define LOG_CATEGORY_RENDER  LogCategory("Render")
#define LOG_CATEGORY_WORLD   LogCategory("World")
#define LOG_CATEGORY_IO      LogCategory("IO")
