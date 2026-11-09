# AlphaRing — Xbox Game Pass (PC) build

Local 4-player splitscreen for **Halo: The Master Chief Collection on Xbox Game Pass / Microsoft Store (PC)**,
MCC version **1.3528.0.0**. Use only with the official **"Halo: MCC Anti-Cheat Disabled (Mods and Limited
Services)"** launch — offline/modded mode, no online play.

Fork of [wouter51/AlphaRing](https://github.com/wouter51/AlphaRing) (MCC 1.3528 update), itself a fork of
[WinterSquire/AlphaRing](https://github.com/WinterSquire/AlphaRing). MIT licence; all credit to them.

### Tested (Game Pass MCC 1.3528.0.0, Windows 11, 2026-11-08)
| Mode | Result |
|--|--|
| Halo CE — Slayer, Blood Gulch, 4 players | 4-way split |
| Halo 3 — campaign (Sierra 117), 4 players | 4-way split |
| Halo CE — campaign | 2-way split only (known limitation) |
| 4 controllers, one per player | works |

Not tested: Steam (the Steam code path is unchanged), Halo 2 / ODST / Reach / Halo 4 splitscreen.

### What this fork changes
- **Game Pass 1.3528 offsets** for the MCC executable (the old Store table targeted 1.3498).
- **`alpha_ring/splitscreen.txt`** — `players=4` starts 4-player splitscreen without opening the overlay;
  `controllers=4` gives player N controller N and turns keyboard/mouse off.
- **`alpha_ring/offsets_ws.txt`** — remap Game Pass offsets after a future MCC update without rebuilding
  (`0xOLD=0xNEW` lines; `version=` to accept a new build).
- **`alpha_ring/alpha_ring.log`** — the console log is also written to a file.
- Crash on quit with the overlay open — fixed.
- **Build on a Mac:** `tools/mac-build/build.sh` cross-compiles the DLL with clang-cl + xwin.

### Install (Game Pass)
1. Xbox app → Halo: The Master Chief Collection → **⋯ → Manage → Files → Browse**. That opens the folder that
   contains `mcc` (e.g. `...\XboxGames\Halo- The Master Chief Collection\Content`).
2. Copy `WTSAPI32.dll` into `mcc\binaries\win64\`.
3. Copy the `alpha_ring` folder next to the `mcc` folder.
4. Start menu → **"Halo: MCC Anti-Cheat Disabled (Mods and Limited Services)"**.
5. **F4** opens the overlay (or use `splitscreen.txt`). Start a game; each player gets a screen.

Uninstall: delete `WTSAPI32.dll`. If Windows reports `VCRUNTIME140_1.dll` missing, install the
[Microsoft Visual C++ Redistributable (x64)](https://aka.ms/vs/17/release/vc_redist.x64.exe).

---


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

### Installation
Make sure you have the latest [Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe) installed.

Download the latest stable build from the [Releases](https://github.com/WinterSquire/AlphaRing/releases) page.

Place the DLL into the "Halo The Master Chief Collection\mcc\binaries\win64" directory and launch the game with EAC off.

For Running on Steam Deck/Linux, add the following command in the Steam Game Launch Options:
``` 
WINEDLLOVERRIDES="WTSAPI32=n,b" %command%
```

### Usage
Toggle menu: `F4` or `Controller Back` + `Controller Start`

To navigate using Controller use the `Right Stick` to move the mouse and `RB` to click.

When the menu is open, game input is disabled.

### Bugs Report
Submit it in the [Issues](https://github.com/WinterSquire/AlphaRing/issues) page.

### Credits
- [Assembly](https://github.com/XboxChaos/Assembly) for the tag group research.
- [Blender](https://github.com/blender/blender) for the bezier curve calculation.
- [Priception](https://github.com/Priception) for adding UI controller support and helping with the interface and crash issue.
