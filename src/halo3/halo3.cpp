#include "halo3.h"

using namespace libmcc;
using namespace halo3;

void s_halo3_flying_camera_update_event::execute() {
    auto tls = g_thread_local_storage();

    auto director = tls->director_globals;

    auto camera = &director->directors[player].director.m_camera.camera;

    if (camera->get_type() != _camera_mode_flying) {
        return;
    }

    auto flying_camera = static_cast<c_flying_camera*>(camera);

    if (update_position) {
		flying_camera->set_position(&position);
    }

	if (update_facing) {
        flying_camera->m_facing = facing;
	}

	if (update_roll) {
		flying_camera->set_roll(roll);
	}
}

void s_halo3_flying_camera_adjust_event::execute() {
    auto tls = g_thread_local_storage();

    auto director = tls->director_globals;

    auto camera = &director->directors[player].director.m_camera.camera;

    if (camera->get_type() != _camera_mode_flying) {
        return;
    }

    auto flying_camera = static_cast<c_flying_camera*>(camera);

    flying_camera->set_roll(flying_camera->m_roll + roll);
}

void s_halo3_player_fov_update_event::execute() {
    auto profile = g_player_profile_globals();

    if (fov < 1.0f) {
        fov = 1.0f;
    } else if (fov > 150.0f) {
        fov = 150.0f;
    }

    *profile->at(player).fov() = fov;
}

void s_halo3_player_fov_adjust_event::execute() {
    auto profile = g_player_profile_globals();

    fov += *profile->at(player).fov();

    if (fov < 1.0f) {
        fov = 1.0f;
    } else if (fov > 150.0f) {
        fov = 150.0f;
    }

    *profile->at(player).fov() = fov;
}

void s_halo3_test_event::execute() {
	auto tls = g_thread_local_storage();

    auto engine = tls->game_engine_globals;

    auto map_variant = &engine->map_variant;

    auto position = tls->g_observer_globals->observers[0].result.focus_point;

    for (int i = 0; i < map_variant->m_number_of_variant_objects; ++i) {
        auto object = map_variant->m_variant_objects + i;

        object->position = position;
    }
}
