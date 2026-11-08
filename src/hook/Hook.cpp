#include "Hook.h"

#include "common.h"

#include "utils.h"
#include "MinHook.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace AlphaRing::Hook {
    enum eDistro {
        Steam,
        WindowsStore,
        None
    };

    static eDistro distro;
    static __int64 hModule;
    static FileVersion version;

    // Game Pass (Windows Store) offsets drift with every MCC update. alpha_ring/offsets_ws.txt
    // (next to patch.xml) remaps them without a rebuild:  version=1.xxxx.0.0  and  0xOLD=0xNEW  lines.
    static std::unordered_map<__int64, __int64> ws_override;
    static std::string ws_version;

    static void LoadWSOverrides() {
        char exe[MAX_PATH];
        GetModuleFileNameA(nullptr, exe, MAX_PATH);
        auto path = std::filesystem::path(exe).parent_path() / "../../../alpha_ring/offsets_ws.txt";
        std::ifstream in(path);
        if (!in) { LOG_INFO("offsets_ws.txt not found ({})", path.string()); return; }
        auto trim = [](std::string s) {
            if (s.rfind("\xEF\xBB\xBF", 0) == 0) s.erase(0, 3); // Notepad BOM
            auto b = s.find_first_not_of(" \t\r\n"), e = s.find_last_not_of(" \t\r\n");
            return b == std::string::npos ? std::string() : s.substr(b, e - b + 1);
        };
        std::string line;
        while (std::getline(in, line)) {
            line = trim(line);
            auto eq = line.find('=');
            if (line.empty() || line[0] == '#' || eq == std::string::npos) continue;
            auto key = trim(line.substr(0, eq)), val = trim(line.substr(eq + 1));
            if (key == "version") {
                int a, b, c, d;
                if (sscanf_s(val.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) == 4) ws_version = val;
                else LOG_WARNING("offsets_ws.txt: bad version '{}'", val);
                continue;
            }
            try { ws_override[std::stoll(key, nullptr, 16)] = std::stoll(val, nullptr, 16); } catch (...) {}
        }
        LOG_INFO("offsets_ws.txt: {} overrides, version '{}'", ws_override.size(), ws_version);
    }

    static __int64 WS(__int64 offset) {
        auto it = ws_override.find(offset);
        return it == ws_override.end() ? offset : it->second;
    }

    bool IsWS() {
        return distro == WindowsStore;
    }

    bool Initialize() {
        bool result;
        char buffer[1024];

        result = MH_Initialize() == MH_OK;

        assertm(result, "failed to initialize minhook");

		LOG_INFO("Initializing AlphaRing...");
        LOG_INFO("Created by WinterSquire, updated by xTrxplex\n");
		LOG_WARNING(" == This version only supports the steam version of the game ==");

        if ((hModule = (__int64)GetModuleHandleA("MCC-Win64-Shipping.exe")) != 0) {
            distro = Steam;
        } else if ((hModule = (__int64)GetModuleHandleA("MCCWinStore-Win64-Shipping.exe")) != 0) {
			distro = WindowsStore;
            LoadWSOverrides();
        } else {
            distro = None;
        }

        assertm(distro != None, "failed to get distro type");

        LOG_INFO("Game Version[{}]: {}", IsWS() ? "Windows Store" : "Steam", GAME_VERSION);

        if ((version = FileVersion(hModule)) != FileVersion::fromString(GAME_VERSION)) {
            if (distro == WindowsStore)
            {
				if (version == FileVersion::fromString("1.3498.0.0") ||
                    (!ws_version.empty() && version != FileVersion::fromString("") &&
                     version == FileVersion::fromString(ws_version.c_str())))
					return true;
            }

            if (distro == WindowsStore) {
                // Store offsets survived Steam 1.3495 -> 1.3528 unchanged; let the player try an unlisted build.
                sprintf(buffer, "Game Pass MCC version %s is not verified (known: 1.3498.0.0).\n"
                                "Try anyway? If it crashes, add version=%s and fixed offsets to alpha_ring\\offsets_ws.txt.",
                        version.toString().c_str(), version.toString().c_str());
                if (MessageBoxA(nullptr, buffer, "AlphaRing", MB_YESNO | MB_ICONWARNING) == IDYES) {
                    LOG_WARNING("Unverified Game Pass version {}", version.toString());
                    return true;
                }
                return false;
            }
            sprintf(buffer, "Version mismatch [%s]:%s", GAME_VERSION, version.toString().c_str());
            MessageBoxA(nullptr, buffer, "Error", MB_OK);
            return false;
        }



        return true;
    }

    bool Shutdown() {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();

        return true;
    }

    bool Detour(const std::initializer_list<Detour_t> &hooks) {
        for (auto &hook : hooks) {
            if (MH_CreateHook(hook.pTarget, hook.detour, hook.ppOriginal) != MH_OK ||
                MH_EnableHook(hook.pTarget) != MH_OK)
                return false;
        }
        return true;
    }

    bool Detour(const std::initializer_list<DetourOffset>& hooks) {
        void* pTarget;

        for (auto &hook : hooks) {
            if ((pTarget = (LPVOID) (hModule + (IsWS() ? WS(hook.offset_ws) : hook.offset_steam))),
                    MH_CreateHook(pTarget, hook.detour, hook.ppOriginal) != MH_OK ||
                    MH_EnableHook(pTarget) != MH_OK)
                return false;
        }
        return true;
    }

    bool Detour(const char *module_name, const std::initializer_list<DetourFunction>& hooks) {
        auto hModule = GetModuleHandleA(module_name);

        if (hModule == nullptr)
            return false;

        for (auto &hook : hooks) {
            auto pTarget = GetProcAddress(hModule, hook.function_name);

            if (pTarget == nullptr)
                return false;

            if (MH_CreateHook(pTarget, hook.detour, hook.ppOriginal) != MH_OK ||
                MH_EnableHook(pTarget) != MH_OK)
                return false;
        }

        return true;
    }

    void Offset(const std::initializer_list<FunctionOffset> &offsets) {
        int patch_count = 0;
        for (auto &offset : offsets) {
            if (offset.ppFunction == nullptr) continue;
            *offset.ppFunction = (void*)(hModule + (IsWS() ? WS(offset.offset_ws) : offset.offset_steam));
        }
    }

    bool Patch(const char *module_name, const std::initializer_list<PatchFunction> &patches) {
        auto hModule = GetModuleHandleA(module_name);

        if (hModule == nullptr)
            return false;

        for (auto &patch : patches) {
            auto pTarget = (LPVOID)GetProcAddress(hModule, patch.function_name);

            if (pTarget == nullptr)
                return false;

            DWORD dwOldProtect;
            VirtualProtect(pTarget, patch.size, PAGE_EXECUTE_READWRITE, &dwOldProtect);
            memcpy(pTarget, patch.patch, patch.size);
            VirtualProtect(pTarget, patch.size, dwOldProtect, &dwOldProtect);
        }

        return true;
    }

    bool Patch(const std::initializer_list<PatchMCC> &patches) {
        for (auto &patch : patches) {
            auto pTarget = (LPVOID)(hModule + (IsWS() ? WS(patch.offset_ws) : patch.offset_steam));

            DWORD dwOldProtect;
            VirtualProtect(pTarget, patch.size, PAGE_EXECUTE_READWRITE, &dwOldProtect);
            memcpy(pTarget, patch.patch, patch.size);
            VirtualProtect(pTarget, patch.size, dwOldProtect, &dwOldProtect);
        }

        return true;
    }
}
