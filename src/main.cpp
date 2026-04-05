#include "common.h"

#include "input/Input.h"
#include "hook/Hook.h"
#include "render/Render.h"
#include "mcc/mcc.h"

static bool Initialize() {
    bool result;

    AlphaRing::Log::Early("Initialize: starting...");

    result = AlphaRing::Log::Init();

    if (!result) {
        AlphaRing::Log::Early("Initialize: FAILED to init log");
        return false;
    }

    AlphaRing::Log::Early("Initialize: log init done, testing LOG_INFO...");
    LOG_INFO("Log initialized.");
    AlphaRing::Log::Early("Initialize: LOG_INFO works");

    AlphaRing::Log::Early("Initialize: calling Hook::Initialize...");
    result = AlphaRing::Hook::Initialize();

    if (!result) {
        AlphaRing::Log::Early("Initialize: FAILED Hook::Initialize");
        LOG_ERROR("Failed to initialize hook");
        return false;
    }

    AlphaRing::Log::Early("Initialize: Hook done");
    
    result = AlphaRing::Filesystem::Init();

    if (!result) {
        AlphaRing::Log::Early("Initialize: FAILED Filesystem::Init");
        return false;
    }

    AlphaRing::Log::Early("Initialize: Filesystem done");

    result = AlphaRing::Input::Init();

    if (!result) {
        AlphaRing::Log::Early("Initialize: FAILED Input::Init");
        LOG_ERROR("Failed to initialize input");
        return false;
    }

    AlphaRing::Log::Early("Initialize: Input done");

    result = AlphaRing::Render::Initialize();

    if (!result) {
        AlphaRing::Log::Early("Initialize: FAILED Render::Initialize");
        LOG_ERROR("Failed to initialize render");
        return false;
    }

    AlphaRing::Log::Early("Initialize: Render done");

    result = MCC::Initialize();

    if (!result) {
        AlphaRing::Log::Early("Initialize: FAILED MCC::Initialize");
        LOG_ERROR("Failed to initialize MCC");
        return false;
    }

    AlphaRing::Log::Early("Initialize: ALL DONE");
    LOG_INFO("AlphaRing fully initialized.");

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
        AlphaRing::Log::Early("DllMain: DLL_PROCESS_ATTACH");
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Initialize, nullptr, 0, nullptr);
    } else if (reason == DLL_PROCESS_DETACH) {
        AlphaRing::Log::Early("DllMain: DLL_PROCESS_DETACH");
        if (reserved == nullptr)
            return Shutdown();
    }

    return true;
}