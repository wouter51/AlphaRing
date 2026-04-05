#pragma once

#include <utils.h>
#include <functional>

#include "./hook/Hook.h"
#include "./log/Log.h"
#include "./global/Global.h"
#include "./filesystem/Filesystem.h"

#undef NDEBUG
#include <assert.h>

#define assertm(exp, msg) assert(((void)msg, (exp)))

// Drop-in replacement for std::mutex using Win32 CRITICAL_SECTION.
// Works reliably under Wine/Proton where std::mutex can deadlock
// when used from DLLs loaded via proxy injection.
struct WinMutex {
    CRITICAL_SECTION cs;
    WinMutex() { InitializeCriticalSection(&cs); }
    ~WinMutex() { DeleteCriticalSection(&cs); }
    void lock() { EnterCriticalSection(&cs); }
    void unlock() { LeaveCriticalSection(&cs); }
    WinMutex(const WinMutex&) = delete;
    WinMutex& operator=(const WinMutex&) = delete;
};
