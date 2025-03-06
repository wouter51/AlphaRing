#pragma once

#include "../multithreading/event_queue_manager.h"

#include <libmcc/libmcc.h>

struct s_halo3_options {
	bool debug_structure;
	libmcc::real_rgba_color debug_structure_color = libmcc::g_real_rgba_white;
	bool debug_instanced_geometry;
	libmcc::real_rgba_color debug_instanced_geometry_color = libmcc::g_real_rgba_blue;
};

inline s_halo3_options g_halo3_options;

struct s_halo3_test_event : s_event_base {
	void execute() override;
};

struct s_halo3_flying_camera_update_event : s_event_base {
	void execute() override;

	libmcc::e_local_player player;

	bool update_position;
	bool update_facing;
	bool update_roll;

	libmcc::real_point3d position;
	libmcc::real_euler_angles2d facing;
	libmcc::real roll;
};

struct s_halo3_flying_camera_adjust_event : s_event_base {
	void execute() override;

	libmcc::e_local_player player;
	libmcc::real roll;
};

struct s_halo3_player_fov_update_event : s_event_base {
	void execute() override;

	libmcc::e_local_player player;
	libmcc::real fov;
};

struct s_halo3_player_fov_adjust_event : s_event_base {
	void execute() override;

	libmcc::e_local_player player;
	libmcc::real fov;
};
