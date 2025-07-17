#pragma once
#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include "LogLevel.h"
#include "LogCategory.h"

namespace Engine::Core::Log {

    using LogCallback = std::function<void(LogLevel, const LogCategory&, const std::string&)>;

    class Logger {
    public:
        static Logger& Get();

        void Log(LogLevel level, const LogCategory& category,
                 const std::string& message,
                 const char* file = nullptr, int line = 0, const char* function = nullptr);

        void Logf(LogLevel level, const LogCategory& category,
                  const char* file, int line, const char* function,
                  const char* fmt, ...);

        void Subscribe(LogCallback callback);
        void SetMinimumLevel(LogLevel level);
        void EnableColor(bool enable);
        void EnableTimestamp(bool enable);

    private:
        Logger();
        LogLevel minLevel;
        bool useColor;
        bool showTimestamp;

        std::vector<LogCallback> subscribers;
        std::mutex mutex;
    };
}

#define LOG_RAW(level, category, msg) \
    ::Engine::Core::Log::Logger::Get().Log(level, category, msg, __FILE__, __LINE__, __FUNCTION__)

#define LOGF_RAW(level, category, fmt, ...) \
    ::Engine::Core::Log::Logger::Get().Logf(level, category, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(msg)  LOG_RAW(LogLevel::Debug, LOG_CATEGORY_DEFAULT, msg)
#define LOG_INFO(msg)   LOG_RAW(LogLevel::Info,  LOG_CATEGORY_DEFAULT, msg)
#define LOG_WARN(msg)   LOG_RAW(LogLevel::Warn,  LOG_CATEGORY_DEFAULT, msg)
#define LOG_ERROR(msg)  LOG_RAW(LogLevel::Error, LOG_CATEGORY_DEFAULT, msg)

#define LOGF_DEBUG(fmt, ...) LOGF_RAW(LogLevel::Debug, LOG_CATEGORY_DEFAULT, fmt, ##__VA_ARGS__)
#define LOGF_INFO(fmt, ...)  LOGF_RAW(LogLevel::Info,  LOG_CATEGORY_DEFAULT, fmt, ##__VA_ARGS__)
#define LOGF_WARN(fmt, ...)  LOGF_RAW(LogLevel::Warn,  LOG_CATEGORY_DEFAULT, fmt, ##__VA_ARGS__)
#define LOGF_ERROR(fmt, ...) LOGF_RAW(LogLevel::Error, LOG_CATEGORY_DEFAULT, fmt, ##__VA_ARGS__)
