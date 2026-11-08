#include "common.h"

#include "input/Input.h"
#include "hook/Hook.h"
#include "render/Render.h"
#include "mcc/mcc.h"
#include "global/Global.h"

#include <filesystem>
#include <fstream>
#include <string>

// alpha_ring/splitscreen.txt: "players=N" (1-4) starts with splitscreen override on and the overlay hidden.
static void LoadSplitscreenFile() {
    char exe[MAX_PATH];
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::ifstream in(std::filesystem::path(exe).parent_path() / "../../../alpha_ring/splitscreen.txt");
    std::string line;
    while (std::getline(in, line)) {
        int n;
        if (line.rfind("\xEF\xBB\xBF", 0) == 0) line.erase(0, 3); // Notepad BOM
        if (sscanf_s(line.c_str(), " players = %d", &n) == 1 && n >= 1 && n <= 4) {
            auto s = AlphaRing::Global::MCC::Splitscreen();
            s->b_override = true;
            s->player_count = n;
            AlphaRing::Global::Global()->show_imgui = false;
            LOG_INFO("splitscreen.txt: {} players, override on", n);
        }
        if (sscanf_s(line.c_str(), " controllers = %d", &n) == 1 && n == 4) {
            auto s = AlphaRing::Global::MCC::Splitscreen();
            s->b_player0_use_km = false;
            s->b_controller_per_player = true;
            LOG_INFO("splitscreen.txt: 4 controllers, player N uses controller N, keyboard off");
        }
    }
}

static bool Initialize() {
    bool result;

    result = AlphaRing::Log::Init();

    assertm(result, "failed to initialize log");

    result = AlphaRing::Hook::Initialize();

    assertm(result, "failed to initialize hook");

    LoadSplitscreenFile();

    //LOG_INFO("Initialized AlphaRing.");
    
    result = AlphaRing::Filesystem::Init();

    assertm(result, "failed to initialize filesystem");

	//LOG_INFO("Initialized filesystem.");

    result = AlphaRing::Input::Init();

    assertm(result, "failed to initialize input");

	//LOG_INFO("Initialized input.");

    result = AlphaRing::Render::Initialize();

    assertm(result, "failed to initialize render");

	//LOG_INFO("Initialized render.");

    result = MCC::Initialize();

    assertm(result, "failed to initialize mcc");

	//LOG_INFO("Initialized mcc.");

    //LOG_INFO("Game Version[{}]: {}", AlphaRing::Hook::IsWS() ? "Windows Store" : "Steam", GAME_VERSION);

    return true;
}

static bool Shutdown() {
    LOG_INFO("Shutting down");

    AlphaRing::Filesystem::Shutdown();
    AlphaRing::Input::Shutdown();
    AlphaRing::Hook::Shutdown();
    AlphaRing::Log::Shutdown();

    return true;
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Initialize, nullptr, 0, nullptr);
    } else if (reason == DLL_PROCESS_DETACH) {
        if (reserved == nullptr)
            return Shutdown();
    }

    return true;
}