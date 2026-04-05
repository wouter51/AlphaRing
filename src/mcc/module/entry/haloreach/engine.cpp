#include "haloreach.h"

#include "common.h"

// todo: Scheduler
namespace HaloReach::Entry::Engine {
    void Prologue() {
        // open access to main thread resources
        // main thread resources will be copied to the render thread
        static __int64 hModule = (__int64)GetModuleHandleA("haloreach.dll");
        HaloReach::Native::s_nativeInfo.update(hModule);
    }
    void Epilogue() {
    }

    HaloReachEntry(entry, OFFSET_HALOREACH_PF_ENGINE, void, detour) {
        Prologue();
        ((detour_t)entry.m_pOriginal)();
        Epilogue();
    }
}
