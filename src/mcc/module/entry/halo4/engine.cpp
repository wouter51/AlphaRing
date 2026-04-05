#include "halo4.h"

#include "common.h"

// todo: Scheduler
namespace Halo4::Entry::Engine {
    void Prologue() {
        // open access to main thread resources
        // main thread resources will be copied to the render thread
        static __int64 hModule = (__int64)GetModuleHandleA("halo4.dll");
        Halo4::Native::s_nativeInfo.update(hModule);
    }

    void Epilogue() {
    }

    Halo4Entry(entry, OFFSET_HALO4_PF_ENGINE, void, detour) {
        Prologue();
        ((detour_t)entry.m_pOriginal)();
        Epilogue();
    }
}