#pragma once

#include <memory>
#include <spdlog/spdlog.h>

spdlog::logger* get_console_logger();

#define LOG_INFO(...)		get_console_logger()->info(__VA_ARGS__)
#define LOG_WARN(...)		get_console_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		get_console_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)	get_console_logger()->critical(__VA_ARGS__)
#define LOG_DEBUG(...)		get_console_logger()->debug(__VA_ARGS__)
#define LOG_TRACE(...)		get_console_logger()->trace(__VA_ARGS__)
