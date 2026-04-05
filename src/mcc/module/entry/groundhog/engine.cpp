#include "groundhog.h"

#include "common.h"

// todo: Scheduler
namespace GroundHog::Entry::Engine {
    void Prologue() {
        // open access to main thread resources
        // main thread resources will be copied to the render thread
        static __int64 hModule = (__int64)GetModuleHandleA("groundhog.dll");
        GroundHog::Native::s_nativeInfo.update(hModule);
    }

    void Epilogue() {
    }

    GroundHogEntry(entry, OFFSET_GROUNDHOG_PF_ENGINE, void, detour) {
        Prologue();
        ((detour_t)entry.m_pOriginal)();
        Epilogue();
    }
}