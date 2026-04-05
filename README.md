## Alpha Ring
A Modding Tool for MCC

[![Build status](https://ci.appveyor.com/api/projects/status/o3qbtc7jirw81xmb?svg=true)](https://ci.appveyor.com/project/WinterSquire/alpharing)
[![](https://dcbadge.limes.pink/api/server/https://discord.gg/TUyAnCrpuz)](https://discord.gg/TUyAnCrpuz)

### Showcase

| | |
|--|--|
| Camera Tool (H3) <br> ![Camera](https://github.com/WinterSquire/AlphaRing/assets/135317392/d359b2e8-5302-430f-be0d-bc065e63f546) | Object Browser (H3) <br> ![Object](https://github.com/WinterSquire/AlphaRing/assets/135317392/0bce1af7-354f-4d9d-92f7-eb2d46d8ae37) |
| 8 Players Campaign <br> ![Splitscreen 8 players](https://github.com/WinterSquire/AlphaRing/assets/135317392/7d9f4281-892a-47e2-8e0c-845a965e5d11) | Splitscreen With [Mod](https://steamcommunity.com/sharedfiles/filedetails/?id=3153235187) (By [Priception](https://steamcommunity.com/id/priception)) <br> ![H4](https://github.com/WinterSquire/AlphaRing/assets/135317392/5359868c-c5db-4300-9805-84c61b0bd8ee) |

### Features
* Splitscreen (all games)
* Camera Tool (H3)
* Object Browser (H3)

### Building from Source

#### Windows (MSVC)
Requires CMake 3.27+, Ninja, and MSVC (x64). Visual Studio or the CMakeSettings.json configs work out of the box:
```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

#### Linux / Dev Container (cross-compilation)
A devcontainer is provided in `.devcontainer/` for cross-compiling the Windows x64 DLL from Linux using Clang 19 + lld-link + xwin (Windows SDK/CRT).

**VS Code**: Open the repo and select "Reopen in Container" when prompted.

**CLI (Docker)**:
```bash
docker build -t alpharing-dev .devcontainer/
docker run --rm -v "$(pwd)":/workspace -w /workspace alpharing-dev bash -c '
  sh .devcontainer/fix-case.sh
  cmake -B build -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=.devcontainer/toolchain-cross-win64.cmake \
    -DCMAKE_BUILD_TYPE=Release
  cmake --build build
'
```

Output: `build/WTSAPI32.dll` (PE32+ x86-64).

### Installation
Make sure you have the latest [Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe) installed.

Download the latest stable build from the [Releases](https://github.com/WinterSquire/AlphaRing/releases) page.

Place the DLL into the "Halo The Master Chief Collection\mcc\binaries\win64" directory and launch the game with EAC off.

For Running on Steam Deck/Linux, add the following command in the Steam Game Launch Options:
``` 
WINEDLLOVERRIDES="WTSAPI32=n,b" %command%
```

Tested with GE-Proton 10-34

### Logging
Logging is disabled by default. To enable it, set the `ALPHARING_LOG` environment variable to `1`. Logs are written to `AlphaRing.log` next to the DLL.

**Windows** (Command Prompt, before launching the game):
```
set ALPHARING_LOG=1
```

**Linux/Steam Deck** (Steam Game Launch Options):
```
WINEDLLOVERRIDES="WTSAPI32=n,b" ALPHARING_LOG=1 %command%
```

### Usage
Toggle menu: `F4` or `Controller Back` + `Controller Start`

To navigate using Controller use the `Right Stick` to move the mouse and `RB` to click.

When the menu is open, game input is disabled.

#### Linux/Steam Deck Notes
On Linux (Proton), start each mission **without** enabling splitscreen or adding extra players first. Once you are loaded into the mission, open the menu and enable splitscreen/add players on the fly.

> **Note:** This workaround does not apply to Halo 1 (CE), which does not support adding players mid-mission.

### Bugs Report
Submit it in the [Issues](https://github.com/WinterSquire/AlphaRing/issues) page.

### Credits
- [Assembly](https://github.com/XboxChaos/Assembly) for the tag group research.
- [Blender](https://github.com/blender/blender) for the bezier curve calculation.
- [Priception](https://github.com/Priception) for adding UI controller support and helping with the interface and crash issue.
