#include "halo3.h"

namespace Halo3::Entry::Engine {
    Halo3Entry(entry, OFFSET_HALO3_PF_ENGINE, void, detour) {
        // open access to main thread resources
        // main thread resources will be copied to the render thread
        static __int64 hModule = (__int64)GetModuleHandleA("halo3.dll");
        Halo3::Native::s_nativeInfo.update(hModule);
        ((detour_t)entry.m_pOriginal)();
    }
}