#include "logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/callback_sink.h>

spdlog::logger* get_console_logger() {
	static std::shared_ptr<spdlog::logger> console_logger = spdlog::stdout_color_mt("console");
	return console_logger.get();
}
