#include "player_manager.h"

#include "../mcc/mcc_manager.h"

using namespace libmcc;

#include <cstring>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/xchar.h>

int c_player_manager::initialize() {
	// Initialize XUID
	initialize_xuid();

	// Initialize profiles
	m_profiles = new s_player_profiles[k_local_player_count];

	if (load_profile()) {
		initialize_profile();
	}

	return 0;
}

int c_player_manager::shutdown() {
	save_profile();

	delete[] m_profiles;

	return 0;
}

int c_player_manager::initialize_xuid() {
	union {
		GUID guid;
		XUID xuid;
		struct { uint64_t part1, part2; };
	} id;

	auto hr = CoCreateGuid(&id.guid);

	ASSERT_HR(hr, "Failed to create guid!");

	id.xuid = id.part1 ^ id.part2;

	for (int i = 0; i < k_local_player_count; i++) {
		m_xuids[i] = id.xuid + i;

		if (i == 0) {
			m_input_devices[0] = _player_input_device_km;
		} else {
			m_input_devices[i] = static_cast<e_player_input_device>(i - 1);
		}
	}

	return 0;
}

int c_player_manager::initialize_profile() {
	memset(m_profiles, 0, k_player_profiles_size);

	for (int i = 0; i < k_local_player_count; i++) {
		auto profile = m_profiles + i;

		auto name = fmt::format(L"Player {}", i + 1);

		auto tag = fmt::format(L"P{}", i + 1);

		memcpy(profile->name, name.c_str(), name.size() * sizeof(wchar_t));

		for (int j = 0; j < k_game_count; j++) {
			auto game_profile = profile->game_profile + j;

			memcpy(game_profile->profile.service_tag, tag.c_str(), tag.size() * sizeof(wchar_t));
		}
	}

	return 0;
}

int c_player_manager::load_profile() {
	auto profile_file = mcc_manager()->read_resource(PROFILE_FILE_NAME);

	if (profile_file == nullptr || profile_file->size() != k_player_profiles_size) {
		return -1;
	}

	memcpy(m_profiles, profile_file->data(), k_player_profiles_size);

	return 0;
}

int c_player_manager::save_profile() {
	return mcc_manager()->write_resource(
		PROFILE_FILE_NAME,
		reinterpret_cast<const char*>(m_profiles),
		k_player_profiles_size);
}
