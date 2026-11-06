#pragma once

#include <Windows.h>
#include <Xinput.h>

namespace AlphaRing {
    namespace Input {
        bool Init();
        bool Shutdown();
        bool Update();
        // Safe for per-frame polling: skips disconnected slots (cached
        // connected mask) to avoid XInputGetState's stall on empty slots,
        // and always zeroes *pState when returning false.
        bool GetXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
        // Full rescan of the controller slots on the next poll; called on
        // WM_DEVICECHANGE so hot-plug is detected at once.
        void RequestPadRescan();
        void SetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
    };
}