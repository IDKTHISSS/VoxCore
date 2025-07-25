#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <source_location>

enum class LogLevel {
    Info,
    Warn,
    Error,
    Fatal
};

class Logger {
public:
    struct ChannelConfig {
        std::vector<std::ostream*> outputs;
        bool use_colors = true;
    };

    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void add_output(const std::string& channel, std::ostream& os, bool use_colors = true) {
        std::lock_guard lock(mutex_);
        channels_[channel].outputs.push_back(&os);
        channels_[channel].use_colors = use_colors;
    }

    void add_file_output(const std::string& channel, const std::string& filename) {
        std::lock_guard lock(mutex_);
        auto& files = file_streams_[channel];
        files.emplace_back(filename, std::ios::app);
        if (files.back().is_open()) {
            channels_[channel].outputs.push_back(&files.back());
            channels_[channel].use_colors = false;
        }
    }

    template <typename... Args>
    void log(LogLevel level,
             const std::string& channel,
             const std::source_location& loc,
             std::format_string<Args...> fmt,
             Args&&... args) {
        std::lock_guard lock(mutex_);

        const auto now = std::chrono::system_clock::now();
        const auto now_time = std::chrono::system_clock::to_time_t(now);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::ostringstream timestamp;
        timestamp << std::put_time(std::localtime(&now_time), "%T")
                  << '.' << std::setfill('0') << std::setw(3) << ms.count();

        const std::string level_str = [&] {
            switch(level) {
                case LogLevel::Info:  return "INFO";
                case LogLevel::Warn:  return "WARN";
                case LogLevel::Error: return "ERROR";
                case LogLevel::Fatal: return "FATAL";
                default: return "UNKNOWN";
            }
        }();

        const std::string color_prefix = "\033[";
        const std::string color_suffix = "\033[0m";
        std::string level_color;

        switch(level) {
            case LogLevel::Info:  level_color = "32m"; // Green
            break;
            case LogLevel::Warn:  level_color = "33m"; // Yellow
            break;
            case LogLevel::Error: level_color = "31m"; // Red
            break;
            case LogLevel::Fatal: level_color = "1;31m"; // Bold Red
            break;
            default: level_color = "0m";
        }

        const std::string filename = loc.file_name();
        const std::string short_filename = filename.substr(filename.find_last_of("/\\") + 1);
        const std::string message = std::format(fmt, std::forward<Args>(args)...);

        const std::string log_entry = std::format(
            "{} [{}] [{}] [{}:{}] {}",
            timestamp.str(),
            level_str,
            channel,
            short_filename,
            loc.line(),
            message
        );

        const std::string colored_entry = std::format(
            "{}{}{}{} {}{}",
            color_prefix, level_color,
            level_str, color_suffix,
            log_entry.substr(log_entry.find(']') + 2),
            (level == LogLevel::Fatal ? " <!>" : "")
        );

        auto log_to_channel = [&](const std::string& ch) {
            if (auto it = channels_.find(ch); it != channels_.end()) {
                for (auto& os : it->second.outputs) {
                    if (os && *os) {
                        if (it->second.use_colors) {
                            *os << colored_entry << std::endl;
                        } else {
                            *os << log_entry << std::endl;
                        }
                    }
                }
            }
        };

        log_to_channel(channel);
        if (channel != "*") log_to_channel("*");

        if (level == LogLevel::Fatal) {
            std::exit(EXIT_FAILURE);
        }
    }

private:
    Logger() = default;
    ~Logger() {
        for (auto& [_, files] : file_streams_) {
            for (auto& f : files) {
                if (f.is_open()) f.close();
            }
        }
    }

    std::mutex mutex_;
    std::map<std::string, ChannelConfig> channels_;
    std::map<std::string, std::vector<std::ofstream>> file_streams_;
};

// Макросы с явной передачей source_location
#define LOG_INFO(channel, fmt, ...) \
Logger::instance().log(LogLevel::Info, channel, std::source_location::current(), fmt, ##__VA_ARGS__)

#define LOG_WARN(channel, fmt, ...) \
Logger::instance().log(LogLevel::Warn, channel, std::source_location::current(), fmt, ##__VA_ARGS__)

#define LOG_ERROR(channel, fmt, ...) \
Logger::instance().log(LogLevel::Error, channel, std::source_location::current(), fmt, ##__VA_ARGS__)

#define LOG_FATAL(channel, fmt, ...) \
Logger::instance().log(LogLevel::Fatal, channel, std::source_location::current(), fmt, ##__VA_ARGS__)