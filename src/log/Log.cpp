#include "Log.h"

#include "spdlog.h"
#include "sinks/basic_file_sink.h"

#include <filesystem>

#include "common.h"

namespace AlphaRing::Log {
    std::shared_ptr<spdlog::logger> default_logger;

    bool Init() {
        AllocConsole(); // fails harmlessly if the process already has a console

        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        // Also log to <game>/alpha_ring/alpha_ring.log so the output survives a crash and can be sent/read remotely.
        std::vector<spdlog::sink_ptr> sinks{std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>()};
        try {
            char exe[MAX_PATH];
            GetModuleFileNameA(nullptr, exe, MAX_PATH);
            auto log = std::filesystem::path(exe).parent_path() / "../../../alpha_ring/alpha_ring.log";
            sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log.string(), true));
        } catch (...) {}
        default_logger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
        default_logger->flush_on(spdlog::level::trace);

        spdlog::register_logger(default_logger);

        return true;
    }

    bool Shutdown() {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();

        return true;
    }
}
