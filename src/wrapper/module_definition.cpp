#include "module_definition.h"

#include <filesystem>

// Use OutputDebugStringA instead of MessageBoxA for Wine/Proton compatibility
// MessageBoxA blocks and can freeze the game on Linux
static void LogError(const char* message) {
    OutputDebugStringA("[AlphaRing Error] ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

ModuleDefinition::ModuleDefinition(const char *moduleName, std::initializer_list<const char*> funcs) {
    // Get System Directory
    wchar_t systemPath[MAX_PATH];
    if (!GetSystemDirectoryW(systemPath, MAX_PATH)) {
        LogError("Unable to load system directory");
        ExitProcess(1);
    }

    // Load DLL
    std::filesystem::path path(systemPath);
    path.append(moduleName);
    if ((m_hModule = LoadLibraryW(path.c_str())) == nullptr) {
        LogError((std::string("Unable to load dll: ") + moduleName).c_str());
        ExitProcess(1);
    }

    // Find Function
    void *func_ptr;
    for (auto func: funcs) {
        if ((func_ptr = GetProcAddress(m_hModule, func)) == nullptr) {
            LogError((std::string("Unable to load function: ") + func).c_str());
            ExitProcess(1);
        }

        m_funcs.push_back(func_ptr);
    }
}

void *ModuleDefinition::GetFunc(int index) {
    if (index < 0 || index >= (int)m_funcs.size()) return nullptr;
    return m_funcs[index];
}
