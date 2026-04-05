#include "groundhog.h"

#include "common.h"
#include <queue>
#include <mutex>

namespace GroundHog::Entry::World {
    WinMutex tasks_mutex;
    std::queue<std::function<void()>> tasks;

    void AddTask(const std::function<void()>& func) {
        std::lock_guard<WinMutex> lock(tasks_mutex);
        tasks.push(func);
    }

    void ExecuteTask() {
        std::function<void()> func = nullptr;

        { std::lock_guard<WinMutex> lock(tasks_mutex);
        if (!tasks.empty()) { func = tasks.front();tasks.pop();}
        }

        if (func != nullptr) func();
    }

    void Prologue() {
        ExecuteTask();
    }

    void Epilogue() {

    }

    GroundHogEntry(entry, OFFSET_GROUNDHOG_PF_WORLD, void, detour) {
        Prologue();
        ((detour_t)entry.m_pOriginal)();
        Epilogue();
    }
}
