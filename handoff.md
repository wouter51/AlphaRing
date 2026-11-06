# AlphaRing Project Handoff

## Project Overview

**AlphaRing** is a C++ DLL-based modding tool for Halo: The Master Chief Collection (MCC). It's a fork of `wouter51/AlphaRing` maintained by `thejackbitt` with a Xbox 360-style menu.

**Repository**: https://github.com/thejackbitt/AlphaRing

**Current Release**: v1.2.1 (commit `bdad7eb`)

### Tech Stack
- C++17 (not C++20 as previously noted)
- CMake build system
- Visual Studio 2022 Build Tools
- DirectX 11 hooking for UI overlay
- ImGui for the in-game interface
- XInput for controller handling
- nlohmann/json for settings serialization

### Dependencies (in `lib/` directory)
- `lib/imgui` - Dear ImGui UI framework (precompiled)
- `lib/minhook` - Function hooking library
- `lib/spdlog` - Logging
- `lib/json` - nlohmann/json for serialization
- `lib/lua` - Lua scripting
- `lib/tinyxml2` - XML parsing
- `lib/game` - Game offsets and structures
- `lib/utils` - Utility functions

---

## Actual Project Structure

```
AlphaRing/
├── CMakeLists.txt
├── build/                    # Build output directory
│   └── Release/
│       └── WTSAPI32.dll      # The compiled mod DLL
├── src/
│   ├── main.cpp              # DLL entry point
│   ├── common.h              # Common includes and macros
│   ├── filesystem/           # File system utilities
│   ├── global/               # Global state management
│   ├── hook/                 # Low-level function hooking
│   ├── input/                # XInput controller handling
│   │   ├── Input.h
│   │   └── Input.cpp         # XInput wrapper functions
│   ├── log/                  # Logging utilities
│   ├── mcc/                  # MCC game integration
│   │   ├── CDeviceManager.*  # Input device management
│   │   ├── CGameEngine.*     # Game engine interface
│   │   ├── CGameGlobal.*     # Global game state
│   │   ├── CGameManager.*    # Player/profile management
│   │   ├── CGamepadMapping.* # Gamepad button-to-action mapping
│   │   ├── CInputDevice.*    # Input device abstraction
│   │   ├── CUserProfile.*    # User profile settings
│   │   ├── mcc.*             # MCC state detection
│   │   ├── module/           # Game-specific modules (Halo 1-4, Reach, ODST)
│   │   ├── network/          # Network functionality
│   │   └── splitscreen/      # Splitscreen functionality
│   │       ├── Splitscreen.h
│   │       └── Splitscreen.cpp  # Splitscreen UI and logic
│   ├── render/               # Rendering subsystem
│   │   ├── d3d11/            # DirectX 11 hooking
│   │   ├── imgui/            # ImGui integration
│   │   │   ├── game/
│   │   │   │   ├── halo3/    # Halo 3 specific UI
│   │   │   │   └── mcc/      # MCC main menu UI
│   │   │   │       └── CMCCContext.cpp  # Main ImGui menu
│   │   │   └── curve_editor/ # Animation curve editor
│   │   └── window/           # Window management
│   └── wrapper/              # DLL wrapper (WTSAPI32)
└── lib/                      # Precompiled libraries
```

**Build Output**: DLL named `WTSAPI32.dll` in `build/Release/`

**Installation**: Copy to `%MCC_DIR%/mcc/binaries/win64/`

---

## Working Features (Confirmed)

- **Splitscreen support** (1-4 players) - works in all Halo games
- **Controller-friendly menu** - works, but can be improved
- **Wireframe rendering** - works
- **Controller-to-player binding** - IMPLEMENTED (see below)
- **Button-to-action gamepad mapping** - IMPLEMENTED with bind feature
- **Configurable menu binding** - works

---

## IMPLEMENTED: Input-Based Controller Binding

### Feature 1: Controller-to-Player Slot Binding (Splitscreen)

**File**: `src/mcc/splitscreen/Splitscreen.cpp`

**How it works**:
1. In the Splitscreen window, each player tab shows an "Input" dropdown
2. Next to the dropdown is a "Bind" button
3. Click "Bind" → shows yellow text "Press any button on controller..."
4. Press any button/trigger on a controller
5. System auto-detects which controller (0-3) and assigns it to that player
6. Click "Cancel" to abort binding

**Implementation details**:

```cpp
// Detect which controller (0-3) has any button/trigger pressed, returns -1 if none
static int DetectActiveController() {
    for (int i = 0; i < 4; i++) {
        XINPUT_STATE state;
        if (AlphaRing::Input::GetXInputGetState(i, &state)) {
            if (state.Gamepad.wButtons != 0 ||
                state.Gamepad.bLeftTrigger > 30 ||
                state.Gamepad.bRightTrigger > 30) {
                return i;
            }
        }
    }
    return -1;
}

// Binding state: which player slot is waiting for controller input (-1 = none)
static int s_binding_player = -1;
```

The UI in `ProfileContext()` checks `s_binding_player` and either shows the binding prompt or the normal dropdown + bind button.

### Feature 2: Button-to-Action Gamepad Mapping

**File**: `src/mcc/CGamepadMapping.cpp`

**How it works**:
1. In the Splitscreen window → expand "Gamepad Mapping" section
2. Select which controller to listen to (Controller 1-4 dropdown at top)
3. For each action (Jump, Melee, Reload, etc.), there's a dropdown + "Bind" button
4. Click "Bind" → shows "Press button..." prompt
5. Press a button on the selected controller
6. That button is assigned to that action

**Implementation details**:

```cpp
// Detect which button is currently pressed on a controller
static int DetectPressedButton(int controllerIndex) {
    XINPUT_STATE state;
    if (!AlphaRing::Input::GetXInputGetState(controllerIndex, &state))
        return -1;

    // Check triggers first
    if (state.Gamepad.bLeftTrigger > 30) return CGamepadMapping::LeftTrigger;
    if (state.Gamepad.bRightTrigger > 30) return CGamepadMapping::RightTrigger;

    // Check buttons
    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) return CGamepadMapping::DpadUp;
    // ... etc for all buttons

    return -1;
}
```

---

## XInput Integration

**File**: `src/input/Input.cpp`

```cpp
namespace AlphaRing::Input {
    bool Init();  // Loads xinput1_3.dll, xinput1_4.dll, or xinput9_1_0.dll
    bool GetXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
    void SetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
    bool Update();  // Handles Start+Back toggle for menu, mouse simulation with right stick
}
```

The `Update()` function also handles:
- Start + Back combo to toggle the ImGui menu
- Right stick to move mouse cursor when menu is open
- Right shoulder button for mouse click when menu is open

---

## Build Instructions

### Prerequisites
- Visual Studio 2022 Build Tools (installed at `C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools`)
- CMake 3.27+

### Build Commands

**First time setup** (if `build/` doesn't exist):
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
```

**Build the DLL**:
```bash
cd build
"C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/MSBuild/Current/Bin/MSBuild.exe" WTSAPI32.vcxproj -p:Configuration=Release -p:Platform=x64
```

**Output**: `build/Release/WTSAPI32.dll`

### Install
Copy `WTSAPI32.dll` to your MCC installation:
```
<Steam>/steamapps/common/Halo The Master Chief Collection/mcc/binaries/win64/
```

---

## Experiment Log: 6-Player Splitscreen

**Status**: FAILED - Game engine limitation

**What was tried**:
- Changed profile arrays from `[4]` to `[6]`
- Updated all loops from `< 4` to `< 6`
- Changed UI limit from `<= 4` to `<= 6`

**Why it failed**:
The Halo engine's `c_splitscreen_config` structure (in `lib/game/src/halo3/render/views/split_screen_config.h`) has hardcoded arrays:
```cpp
struct c_splitscreen_config {
    // ...
    } m_view_bounds[4];      // Only 4 view bounds
} m_config_table[4];         // Only 4 config tables
```

The game engine itself only has screen layouts for 1-4 players. There's no way to add more without modifying the game executable itself.

**Reverted**: All changes reverted back to 4-player maximum.

---

## Key Files Reference

| File | Purpose |
|------|---------|
| `src/mcc/splitscreen/Splitscreen.cpp` | Splitscreen UI, controller-to-player binding |
| `src/mcc/CGamepadMapping.cpp` | Button-to-action mapping with bind feature |
| `src/mcc/CGameManager.cpp` | Player profile container, XUID management |
| `src/mcc/settings/Settings.cpp` | JSON save/load for all settings |
| `src/input/Input.cpp` | XInput wrapper, menu toggle |
| `src/render/imgui/game/mcc/CMCCContext.cpp` | Main ImGui menu structure |
| `CMakeLists.txt` | Build configuration |

---

## Current Git State

**Branch**: Detached HEAD at `bdad7eb`

**Modified files** (not committed):
- `src/mcc/splitscreen/Splitscreen.cpp` - Controller bind feature added
- `src/mcc/CGamepadMapping.cpp` - Already had button bind feature

**To commit these changes**:
```bash
git add src/mcc/splitscreen/Splitscreen.cpp
git commit -m "feat: add controller-to-player bind feature in splitscreen"
```

---

## Notes for Future Development

1. **Thread safety**: The codebase uses critical sections for thread-safe access. Follow existing patterns.

2. **ImGui patterns**: UI code uses `ImGui::BeginDisabled()`/`EndDisabled()` for conditional enabling.

3. **XInput limitation**: Only 4 controllers (0-3) are supported by XInput. Players 5+ would need to share controllers or use keyboard.

4. **Settings auto-save**: Splitscreen options auto-save. Profile changes require manual "Save Profile" click.

5. **Game detection**: Use `MCC::IsInGame()` to check if player is in a game session.

6. **Build warning**: `LNK4098: defaultlib 'LIBCMT' conflicts` - This is harmless and can be ignored.

---

## Summary

**What's working**:
- 4-player splitscreen with input-based controller binding
- Gamepad button-to-action mapping with bind feature
- Full profile save/load to JSON
- All original AlphaRing features

**What's NOT possible**:
- More than 4 players (game engine limitation)
- More than 4 controllers (XInput limitation)
