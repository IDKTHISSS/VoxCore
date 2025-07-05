#include "Logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdarg>

namespace Engine::Core::Log {

    Logger::Logger() : minLevel(LogLevel::Debug), useColor(true), showTimestamp(true) {}

    Logger& Logger::Get() {
        static Logger instance;
        return instance;
    }

    void Logger::SetMinimumLevel(LogLevel level) { minLevel = level; }
    void Logger::EnableColor(bool enable) { useColor = enable; }
    void Logger::EnableTimestamp(bool enable) { showTimestamp = enable; }

    void Logger::Subscribe(LogCallback callback) {
        std::lock_guard<std::mutex> lock(mutex);
        subscribers.push_back(callback);
    }

    static const char* ToStr(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
            default:              return "UNKNOWN";
        }
    }

    static const char* ToColor(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "\033[90m"; // Gray
            case LogLevel::Info:  return "\033[37m"; // White
            case LogLevel::Warn:  return "\033[33m"; // Yellow
            case LogLevel::Error: return "\033[31m"; // Red
            default:              return "\033[0m";
        }
    }

    void Logger::Log(LogLevel level, const LogCategory& category,
                     const std::string& message, const char* file, int line, const char* function) {
        if (level < minLevel) return;

        std::ostringstream output;

        if (showTimestamp) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
            localtime_s(&tm, &time);
            output << "[" << std::put_time(&tm, "%H:%M:%S") << "] ";
        }

        output << "[" << ToStr(level) << "] ";
        output << "[" << category.name << "] ";
        if (file) {
            output << "[" << file << ":" << line << "] ";
        }
        if (function) {
            output << "[" << function << "] ";
        }

        output << message;

        std::string finalMsg = output.str();

        if (useColor) {
            std::cout << ToColor(level) << finalMsg << "\033[0m" << std::endl;
        } else {
            std::cout << finalMsg << std::endl;
        }

        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& sub : subscribers) {
            sub(level, category, finalMsg);
        }
    }

    void Logger::Logf(LogLevel level, const LogCategory& category,
                      const char* file, int line, const char* function,
                      const char* fmt, ...) {
        char buffer[2048];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        Log(level, category, buffer, file, line, function);
    }
}
