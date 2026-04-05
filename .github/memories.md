# AlphaRing Repository Knowledge

## What It Is
- Modding tool for Halo: Master Chief Collection (MCC)
- DLL proxy injection via `WTSAPI32.dll` — wraps real Windows Terminal Services API
- Provides: splitscreen (all games), camera tool (H3), object browser (H3)
- Created by WinterSquire, updated by xTrxplex

## Build
- CMake 3.27+, Ninja generator, MSVC x64, C++17
- Output: `WTSAPI32.dll` (shared library)
- Current target version: `1.3528.0.0` (set in root CMakeLists.txt `VERSION` var)
- Install dir: `${MCC_DIR}/mcc/binaries/win64` (DLL) + `${MCC_DIR}/alpha_ring` (resources)
- Two configs: Debug & Release — libs in `lib/<name>/lib/debug|release/`
- Definitions: `WRAPPER_DLL_NAME`, `GAME_VERSION`, `_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS`, `IMGUI_DEFINE_MATH_OPERATORS`

## Dependencies (pre-built under lib/)
- MinHook: function hooking
- ImGui: in-game overlay UI (DX11)
- Lua: scripting engine
- spdlog: logging
- tinyxml2: XML parsing (patch.xml)
- nlohmann/json: JSON (header-only)

## Key Architecture
- `src/main.cpp`: DllMain → DLL_PROCESS_ATTACH → CreateThread(Initialize)
- `src/common.h`: precompiled header — utils, Hook, Log, Global, Filesystem + assertm macro
- `src/hook/`: MinHook wrapper — Detour(), Offset(), Patch() with structs DetourOffset, Detour_t, FunctionOffset, PatchMCC
- `src/log/`: spdlog — LOG_INFO, LOG_ERROR, LOG_WARNING, LOG_DEBUG macros
- `src/global/`: DefGlobal/ImplGlobal pattern for singleton state (wireframe, imgui, splitscreen config)
- `src/filesystem/`: File I/O for alpha_ring/ resource dir
- `src/input/`: XInput wrapper for controller interception
- `src/render/`: D3D11 swapchain Present hook, ImGui integration, window management
  - `d3d11/`: D3d11.h/cpp, Graphics.h/cpp, Hook.cpp
  - `imgui/`: ImGui.h/cpp, ICContext.h, curve_editor/, game/ (halo3, mcc UI panels)
  - `window/`: Window.h/cpp
- `src/wrapper/`: ModuleDefinition loads real WTSAPI32.dll, forwards 5 exports
- `src/mcc/`: Engine interaction
  - CGameEngine: vtable for engine (pause/restart/events)
  - CGameManager: player profiles, controllers, input, game state
  - CGameGlobal: game enum (Halo1-HaloReach), state/events
  - CDeviceManager, CUserProfile, CGamepadMapping
  - `module/`: CModule — per-game DLL loading, entry hooks, XML patches
    - `entry/`: per-game hook entry points (halo1-haloreach subdirs)
    - `patch/`: CPatch, CPatchSet — XML-driven runtime patching
  - `network/`: WIP, currently commented out
  - `splitscreen/`: logic + ImGui UI

## Game Library (lib/game/)
- `inc/`: Per-game headers (halo1.h-haloreach.h, groundhog.h)
- `inc/<version>/`: Offset headers per version (1.3385.0.0, 1.3495.0.0, 1.3528.0.0)
  - Each version has: offset_halo1.h through offset_mcc.h
- `src/`: Native function wrappers per game
  - halo3/ has deepest coverage: ai, camera, game, interface, main, networking, objects, physics, rasterizer, render, simulation, units
- `src/ICNative.h`: DefNative, DefPtr, DefPPtr macros for game memory access

## Utils (lib/utils/)
- FileVersion: parse/compare game EXE version strings
- String: string utilities
- ThreadLocalStorage: TLS for per-module base address

## Resources (res/ → installed to alpha_ring/)
- init.lua: Lua init script (sets alpha_ring['show_menu'] = true)
- patch.xml: XML memory patches per module/version (e.g., halo3.dll patches)

## Coding Conventions
- Namespaces: AlphaRing::{Hook,Render,Input,Log,Filesystem,Global}, MCC, MCC::{Module,Splitscreen,Network}
- PascalCase classes/structs with C prefix (CGameEngine, CModule)
- PascalCase namespaces, camelCase/snake_case functions/members
- Hook pattern: AlphaRing::Hook::Detour({...}) with initializer lists
- Dual offsets: Steam + Windows Store (only Steam actively tested)
- Game detection: CGameGlobal::eGame enum
- assertm(expr, msg) for init-time assertions

## Supported Games
| Enum | Game         | DLL           |
|------|-------------|---------------|
| 0    | Halo 1 (CE) | halo1.dll     |
| 1    | Halo 2      | halo2.dll     |
| 2    | Halo 3      | halo3.dll     |
| 3    | Halo 4      | halo4.dll     |
| 4    | H2A (Groundhog) | groundhog.dll |
| 5    | Halo 3 ODST | halo3odst.dll |
| 6    | Halo Reach  | haloreach.dll |

## Distribution Detection (Hook.cpp)
- Steam: MCC-Win64-Shipping.exe
- Windows Store: MCCWinStore-Win64-Shipping.exe
- Validates file version matches GAME_VERSION at startup

## User Controls
- Toggle menu: F4 or Back+Start on controller
- Controller nav: Right Stick = mouse, RB = click
- Game input disabled when menu open

## Steam Deck / Linux
- Launch option: `WINEDLLOVERRIDES="WTSAPI32=n,b" %command%`

## Dev Container (Cross-Compilation from Linux)
- `.devcontainer/` provides a Docker-based cross-compilation environment
- Stack: Ubuntu 24.04 + Clang 19 (from LLVM apt repo) + lld-link + xwin 0.6.5
- xwin downloads Windows SDK + MSVC CRT headers/libs into /xwin
- Toolchain file: `.devcontainer/toolchain-cross-win64.cmake` — uses clang-cl targeting x86_64-pc-windows-msvc
- xwin only ships release CRT — forces `CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL` (/MD) for all build types
- Case-sensitivity symlinks required on Linux (NTFS is case-insensitive):
  - `lib/*/lib/Release` → `release`, `lib/*/lib/Debug` → `debug`
  - `lib/minhook/lib/*/libMinhook.x64.lib` → `libMinHook.x64.lib` (CMakeLists.txt uses wrong casing)
  - `src/render/D3d11` → `d3d11`, `src/render/Window` → `window` (includes use PascalCase dirs)
  - `/xwin/sdk/include/ucrt/String.h` → `string.h` (game code includes `"String.h"` resolved case-insensitively)
- `.devcontainer/fix-case.sh` creates all workspace-level symlinks in postCreateCommand
- Build: `cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=.devcontainer/toolchain-cross-win64.cmake -DCMAKE_BUILD_TYPE=Release && cmake --build build`
- Produces valid PE32+ x86-64 DLL (~1.2 MB)
