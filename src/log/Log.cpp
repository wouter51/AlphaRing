#include "Log.h"

namespace AlphaRing::Log {
    static FILE* g_logFile = nullptr;
    static CRITICAL_SECTION g_logCS;
    static bool g_csInit = false;
    static bool g_enabled = false;

    static bool IsEnabled() {
        static int cached = -1;
        if (cached == -1) {
            char buf[16] = {};
            DWORD len = GetEnvironmentVariableA("ALPHARING_LOG", buf, sizeof(buf));
            cached = (len > 0 && buf[0] != '0') ? 1 : 0;
        }
        return cached == 1;
    }

    // Returns path to "AlphaRing.log" next to the DLL
    static const char* GetLogPath() {
        static char path[MAX_PATH] = {};
        if (path[0] == '\0') {
            HMODULE hSelf = nullptr;
            GetModuleHandleExA(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)&GetLogPath,
                &hSelf);
            if (hSelf && GetModuleFileNameA(hSelf, path, MAX_PATH)) {
                char* last = strrchr(path, '\\');
                if (!last) last = strrchr(path, '/');
                if (last) *(last + 1) = '\0';
                strcat(path, "AlphaRing.log");
            } else {
                strcpy(path, "AlphaRing.log");
            }
        }
        return path;
    }

    static void WriteTimestamp(FILE* f) {
        time_t now = time(nullptr);
        struct tm t;
        localtime_s(&t, &now);
        fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] ",
            t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec);
    }

    void Early(const char* fmt, ...) {
        if (!IsEnabled()) return;

        FILE* f = fopen(GetLogPath(), "a");
        if (!f) return;

        WriteTimestamp(f);

        va_list args;
        va_start(args, fmt);
        vfprintf(f, fmt, args);
        va_end(args);

        fprintf(f, "\n");
        fclose(f);
    }

    void Write(const char* level, const char* msg) {
        if (!IsEnabled()) return;

        if (!g_csInit) {
            // Fallback before Init()
            Early("[%s] %s", level, msg);
            return;
        }

        EnterCriticalSection(&g_logCS);
        if (g_logFile) {
            WriteTimestamp(g_logFile);
            fprintf(g_logFile, "[%s] %s\n", level, msg);
            fflush(g_logFile);
        }
        LeaveCriticalSection(&g_logCS);
    }

    bool Init() {
        g_enabled = IsEnabled();
        if (!g_enabled) return true;

        Early("=== AlphaRing Log Start ===");

        InitializeCriticalSection(&g_logCS);
        g_csInit = true;

        g_logFile = fopen(GetLogPath(), "a");
        if (!g_logFile) {
            Early("ERROR: failed to open persistent log file");
            return false;
        }

        return true;
    }

    bool Shutdown() {
        Write("INFO", "Shutting down logger.");

        if (g_csInit) {
            EnterCriticalSection(&g_logCS);
            if (g_logFile) {
                fclose(g_logFile);
                g_logFile = nullptr;
            }
            LeaveCriticalSection(&g_logCS);
            DeleteCriticalSection(&g_logCS);
            g_csInit = false;
        }

        return true;
    }
}
