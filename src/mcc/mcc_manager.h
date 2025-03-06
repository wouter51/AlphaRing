#pragma once

#include "../common.h"

#include <libmcc/libmcc.h>

#define BASE_DIR TOSTRING(PROJECT_NAME)

class c_mcc_manager : public i_manager {
public:
	int initialize() override;
	int shutdown() override;

	HANDLE attach(HMODULE hModule);
	void detach();

	int initialize_base_dir();
	std::unique_ptr<std::vector<char>> read_resource(const char* resource);
	bool write_resource(const char* resource, const char* data, size_t size);

	int allocate_console();
	int free_console();

	inline libmcc::e_module get_mcc_module() const { return m_mcc_module; }
	inline libmcc::e_module get_game_module() const { return m_game_module; }
	inline libmcc::e_game_globals_state get_game_state() const { return m_game_state; }
	inline libmcc::e_game_mode get_game_mode() const { return m_game_mode; }
	inline bool get_is_theater() const { return m_is_theater; }

	inline void set_game_module(libmcc::e_module module) { m_game_module = module; }
	inline void set_game_state(libmcc::e_game_globals_state state) { m_game_state = state; }
	inline void set_game_mode(libmcc::e_game_mode mode) { m_game_mode = mode; }
	inline void set_is_theater(bool is_theater) { m_is_theater = is_theater; }

private:
	bool m_initialized;
	bool m_is_theater;

	HMODULE m_hModule;
	s_file_version m_version;
	libmcc::e_module m_mcc_module;
	libmcc::e_module m_game_module;
	libmcc::e_game_globals_state m_game_state;
	libmcc::e_game_mode m_game_mode;

	std::wstring m_cwd; // mcc is broken, so we need to create one for ourselves
};

inline c_mcc_manager g_mcc_manager;

inline c_mcc_manager* mcc_manager() {
	return &g_mcc_manager;
}