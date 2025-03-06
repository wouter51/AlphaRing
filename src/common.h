#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <string>
#include <memory>
#include <filesystem>

#include "./main/main.h"
#include "./main/manager.h"
#include "./main/logging.h"
#include "./main/assertion.h"
#include "./main/win32.h"

#include "./multithreading/event_queue_manager.h"
#include "./multithreading/critical_section_manager.h"

#include "./hook/hook_manager.h"
#include "./hook/vftable_manager.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
