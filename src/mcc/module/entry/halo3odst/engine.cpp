#include "halo3odst.h"

#include "common.h"

// todo: Scheduler

namespace Halo3ODST::Entry::Engine {
    void Prologue() {
        // open access to main thread resources
        // main thread resources will be copied to the render thread
        static __int64 hModule = (__int64)GetModuleHandleA("halo3odst.dll");
        Halo3ODST::Native::s_nativeInfo.update(hModule);
    }

    void Epilogue() {
    }

    Halo3ODSTEntry(entry, OFFSET_HALO3ODST_PF_ENGINE, void, detour) {
        Prologue();
        ((detour_t)entry.m_pOriginal)();
        Epilogue();
    }
}
