#pragma once

#include <fmt/bundled/format.h>
#include <Windows.h>
#include <cstdio>
#include <cstdarg>
#include <ctime>

namespace AlphaRing::Log {
    // Early file log - works before Init() (global constructors, DllMain)
    void Early(const char* fmt, ...);

    // Formatted file log - works after Init()
    void Write(const char* level, const char* msg);

    template<typename... Args>
    void Info(fmt::format_string<Args...> fmt, Args&&... args) {
        Write("INFO", fmt::format(fmt, std::forward<Args>(args)...).c_str());
    }

    template<typename... Args>
    void Error(fmt::format_string<Args...> fmt, Args&&... args) {
        Write("ERROR", fmt::format(fmt, std::forward<Args>(args)...).c_str());
    }

    template<typename... Args>
    void Warn(fmt::format_string<Args...> fmt, Args&&... args) {
        Write("WARN", fmt::format(fmt, std::forward<Args>(args)...).c_str());
    }

    template<typename... Args>
    void Debug(fmt::format_string<Args...> fmt, Args&&... args) {
        Write("DEBUG", fmt::format(fmt, std::forward<Args>(args)...).c_str());
    }

    bool Init();
    bool Shutdown();
}

#define LOG_INFO(...) AlphaRing::Log::Info(__VA_ARGS__)
#define LOG_ERROR(...) AlphaRing::Log::Error(__VA_ARGS__)
#define LOG_WARNING(...) AlphaRing::Log::Warn(__VA_ARGS__)
#define LOG_DEBUG(...) AlphaRing::Log::Debug(__VA_ARGS__)