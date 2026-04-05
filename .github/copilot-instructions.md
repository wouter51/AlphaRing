# Copilot Instructions for AlphaRing

## Project Overview

AlphaRing is a modding tool for Halo: The Master Chief Collection (MCC). It is compiled as a DLL (`WTSAPI32.dll`) that wraps the real Windows Terminal Services API, allowing it to be loaded by the game via DLL proxy injection. The mod provides splitscreen support for all games, a camera tool (Halo 3), and an object browser (Halo 3).

## Build System

- **CMake** (minimum 3.27), generator: Ninja
- **C++ Standard**: C++17
- **Target architecture**: x64 only (Windows)
- **Toolchain**: MSVC (`msvc_x64_x64`)
- **Configurations**: Debug and Release
- **Output**: A shared library named `WTSAPI32.dll`
- **Current game version target**: `1.3528.0.0` (set via `VERSION` in root CMakeLists.txt)
- **Compile definitions**: `WRAPPER_DLL_NAME`, `GAME_VERSION`, `_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS`, `IMGUI_DEFINE_MATH_OPERATORS`
- **Install**: Copies the DLL to `${MCC_DIR}/mcc/binaries/win64` and resources to `${MCC_DIR}/alpha_ring`
- **Cross-compilation (Linux)**: Devcontainer in `.devcontainer/` using Clang 19 (`clang-cl`) + `lld-link` + `xwin` for Windows SDK/CRT. Toolchain file: `.devcontainer/toolchain-cross-win64.cmake`. Requires `fix-case.sh` for Linux case-sensitivity symlinks.

## Dependencies (all pre-built as static/shared libs under `lib/`)

| Library    | Purpose                              |
|------------|--------------------------------------|
| MinHook    | Function hooking (detour/trampoline) |
| ImGui      | In-game overlay UI (DX11 backend)    |
| Lua        | Scripting engine                     |
| spdlog     | Logging                              |
| tinyxml2   | XML parsing (patch.xml)              |
| nlohmann/json | JSON parsing (header-only)        |

## Project Structure

### `src/` — Core application code

- **`main.cpp`** — DLL entry point (`DllMain`). Spawns initialization on `DLL_PROCESS_ATTACH`.
- **`common.h`** — Precompiled-style header: includes `utils.h`, `Hook.h`, `Log.h`, `Global.h`, `Filesystem.h`.
- **`hook/`** — MinHook wrapper. Provides `Detour()`, `Offset()`, `Patch()` helpers and struct types (`DetourFunction`, `DetourOffset`, `FunctionOffset`, `PatchMCC`, `Detour_t`).
- **`log/`** — spdlog-based logging. Macros: `LOG_INFO`, `LOG_ERROR`, `LOG_WARNING`, `LOG_DEBUG`.
- **`global/`** — Global state singletons using `DefGlobal`/`ImplGlobal` macros. Holds runtime flags (wireframe, imgui visibility, splitscreen config).
- **`filesystem/`** — File I/O helpers for the `alpha_ring/` resource directory.
- **`input/`** — XInput wrapper. Intercepts controller state for menu navigation.
- **`render/`** — Rendering subsystem:
  - `d3d11/` — D3D11 hook (swapchain Present), Graphics state management.
  - `imgui/` — ImGui integration, game-specific UI panels (`game/halo3/`, `game/mcc/`), curve editor.
  - `window/` — Window management.
- **`wrapper/`** — DLL proxy implementation. `ModuleDefinition` class loads the real `WTSAPI32.dll` and forwards exported functions (`WTSRegisterSessionNotification`, etc.).
- **`mcc/`** — MCC engine interaction:
  - `CGameEngine` — Virtual function table for the game engine (pause, restart, events).
  - `CGameManager` — Player profiles, controller mapping, input, game state.
  - `CGameGlobal` — Game identification enum (`Halo1`..`HaloReach`), game state/events.
  - `CDeviceManager` — Device/controller management.
  - `CUserProfile` / `CGamepadMapping` — Player settings.
  - `module/` — Per-game module loading. `CModule` manages entry hooks and patches per game DLL. `entry/` has hook entry points per game. `patch/` has XML-driven patching (`CPatch`, `CPatchSet`).
  - `network/` — Network features (currently commented out / WIP).
  - `splitscreen/` — Splitscreen logic and ImGui UI.

### `lib/game/` — Game-specific reverse-engineered code

- **`inc/`** — Headers per game: `halo1.h`, `halo2.h`, `halo3.h`, `halo4.h`, `haloreach.h`, `halo3odst.h`, `groundhog.h` (Halo 2 Anniversary).
- **`inc/<version>/`** — Version-specific offset headers (e.g., `offset_halo3.h`, `offset_mcc.h`) for versions `1.3385.0.0`, `1.3495.0.0`, `1.3528.0.0`.
- **`src/`** — Native function wrappers per game (e.g., `halo3/` has subdirs for `ai`, `camera`, `game`, `objects`, `physics`, `render`, `simulation`, `units`, etc.).
- **`src/ICNative.h`** — Macros: `DefNative`, `DefPtr`, `DefPPtr` for defining native game function/pointer accessors using module base + offset.

### `lib/utils/` — Shared utility code

- `FileVersion` — Parse/compare game EXE version strings.
- `String` — String utilities.
- `ThreadLocalStorage` — TLS for per-module base address storage.

### `res/` — Runtime resources (installed to `alpha_ring/`)

- `init.lua` — Lua initialization script.
- `patch.xml` — XML-defined memory patches per module/version.

## Coding Conventions

- **Namespaces**: `AlphaRing::Hook`, `AlphaRing::Render`, `AlphaRing::Input`, `AlphaRing::Log`, `AlphaRing::Filesystem`, `AlphaRing::Global`, `MCC`, `MCC::Module`, `MCC::Splitscreen`, `MCC::Network`.
- **Naming**: PascalCase for classes/structs (`CGameEngine`, `CModule`), PascalCase for namespaces, camelCase/snake_case for functions and members. Prefix `C` for major classes.
- **Hook pattern**: Use `AlphaRing::Hook::Detour()` with initializer lists of `DetourOffset` or `Detour_t`. Use `AlphaRing::Hook::Offset()` for resolving function/data pointers.
- **Offsets**: Steam and Windows Store offsets are maintained in parallel (`offset_steam`, `offset_ws`).
- **Game detection**: Based on `CGameGlobal::eGame` enum. Module DLLs are loaded dynamically.
- **Assertions**: `assertm(expr, msg)` macro used extensively during initialization.
- **Globals**: Use `DefGlobal(Name)` / `ImplGlobal(Name)` pattern for singleton global state.
- **Native calls**: Use `DefNative`, `DefPtr`, `DefPPtr` macros from `ICNative.h` to access game memory at known offsets.

## Supported Games

| Enum Value | Game            | Module DLL   |
|-----------|-----------------|--------------|
| 0         | Halo 1 (CE)     | halo1.dll    |
| 1         | Halo 2          | halo2.dll    |
| 2         | Halo 3          | halo3.dll    |
| 3         | Halo 4          | halo4.dll    |
| 4         | Groundhog (H2A) | groundhog.dll|
| 5         | Halo 3 ODST     | halo3odst.dll|
| 6         | Halo Reach      | haloreach.dll|

## DLL Injection Mechanism

The project builds as `WTSAPI32.dll`. When placed in the game directory, Windows loads it instead of the system DLL. The wrapper code (`src/wrapper/`) loads the real system `WTSAPI32.dll` and forwards all original exports while simultaneously running the mod's initialization code on `DLL_PROCESS_ATTACH`.

## Key Technical Notes

- All memory offsets are relative to the game executable's base address.
- Two sets of offsets exist: one for Steam, one for Windows Store. Only Steam is actively tested.
- The mod hooks D3D11 `Present` to inject ImGui rendering.
- XInput is intercepted to allow controller-based menu navigation.
- Patches can be defined in `patch.xml` and applied at runtime via the `CPatch`/`CPatchSet` system.
- Lua scripting support is available via the embedded Lua runtime.

## Copilot Workflow Preferences

- Always save memory/knowledge files within the repository (e.g., `.github/memories.md`) rather than only in Copilot's internal memory system.
