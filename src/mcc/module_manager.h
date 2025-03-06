#pragma once

#include "../common.h"

#include <libmcc/libmcc.h>

struct s_module_vftable {
	s_module_vftable(size_t offset, const void** src, void** original, size_t size)
		: offset(offset), src(src), original(original), size(size) {}

	size_t offset;
	const void** src;
	void** original;
	size_t size;
};

struct s_module_patch {
	s_module_patch(size_t offset, const char* patch)
		: offset(offset), patch(patch), enabled(true) {
	}

	bool enabled;
	void* target;
	const char* patch;
	size_t offset;
	size_t size;
};

struct s_module_hook {
	s_module_hook(size_t offset, void* detour, void** original)
		: offset(offset), detour(detour), original(original), enabled(true) {
	}

	bool enabled;
	void* target;
	void* detour;
	void** original;
	size_t offset;
};

struct s_module {
	HMODULE hModule;
	std::vector<s_module_patch> patches;
	std::vector<s_module_hook> hooks;
	std::vector< s_module_vftable> vftables;

	int initialize(HMODULE hModule);
	int shutdown();
};

class c_module_manager : public i_manager {
public:
	int initialize() override;
	int shutdown() override;

	s_module* get(libmcc::e_module module) {
		return &m_modules[module];
	}

private:
	s_module m_modules[libmcc::k_game_count];
};

inline ::c_module_manager g_module_manager;
inline ::c_module_manager* module_manager() {
	return &g_module_manager;
}
