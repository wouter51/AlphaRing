#pragma once

#include "../common.h"

#include <libmcc/libmcc.h>
#include <libmcc/game/players.h>

#define PROFILE_FILE_NAME "profile.dat"

struct s_player_profiles {
	wchar_t name[0x20];
	struct s_game_profile {
		libmcc::s_player_profile profile;
		libmcc::s_gamepad_mapping gamepad_mapping;
	} game_profile[libmcc::k_game_count];
};

class c_player_manager : i_manager {
public:
	int initialize() override;
	int shutdown() override;

	int initialize_xuid();
	int initialize_profile();
	int load_profile();
	int save_profile();

	inline libmcc::XUID get_xuid(int player) { return m_xuids[player]; }
	inline s_player_profiles* get_profile(int player) {return m_profiles + player;}

	inline int get_local_player_count() { return m_local_player_count; }
	inline void set_local_player_count(int count) { m_local_player_count = count; }

private:
	int m_local_player_count = 1;
	libmcc::XUID m_xuids[libmcc::k_game_count];
	s_player_profiles* m_profiles;

	static constexpr size_t k_player_profiles_size = sizeof(s_player_profiles[libmcc::k_local_player_count]);
};

inline c_player_manager g_player_manager;

inline c_player_manager* player_manager() {
	return &g_player_manager;
}
