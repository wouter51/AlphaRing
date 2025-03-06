#include "dollycam_game_update.h"

#include <libmcc/libmcc.h>

using namespace libmcc;
using namespace halo3;

c_dollycam_game_update::c_dollycam_game_update() {
	m_play = false;
	m_model = &g_dollycam_model;
}

void c_dollycam_game_update::execute() {
	if (!m_play) {
		return;
	}

	if (m_model == nullptr) {
		return;
	}

	auto tls = g_thread_local_storage();

	if (!tls->game_globals->options.game_playback()) {
		return;
	}

	auto game_time_globals = tls->game_time_globals;

	auto director_globals = tls->director_globals;

	auto player = _local_player_0;

	if (director_globals->infos[player].camera_mode != _camera_mode_flying) {
		return;
	}

	auto camera = &director_globals->directors[player].director.m_camera.flying_camera;

	s_dollycam_value value;
	s_dollycam_value_attribute attr;

	float* dst[]{
		&camera->m_position.x,
		&camera->m_position.y,
		&camera->m_position.z,
		&camera->m_facing.yaw,
		&camera->m_facing.pitch,
		&camera->m_roll,
		g_player_profile_globals()->at(player).fov()
	};

	m_model->interp(game_time_globals->elapsed_ticks, &value, &attr);

	for (int i = 0; i < k_dollycam_series_count; ++i) {
		if (!attr.enabled[i]) {
			continue;
		}

		*dst[i] = value.n[i];
	}
}
