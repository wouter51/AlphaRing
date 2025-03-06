#pragma once

#include <imgui.h>

struct s_keyframe_handle {
	ImVec2 data;
};

enum e_keyframe_handle_type {
	_keyframe_handle_left,
	_keyframe_handle_middle,
	_keyframe_handle_right,
	k_keyframe_handle_count,
};

enum e_keyframe_interpolation {
	_keyframe_interpolation_constant,
	_keyframe_interpolation_linear,
	_keyframe_interpolation_bezier,
	k_keyframe_interpolation_count,
};

struct s_keyframe {
	s_keyframe();

	float calculate(float frame, const s_keyframe* other) const;
	float calculateT(float t, const s_keyframe* other) const;

	e_keyframe_interpolation interpolation;
	ImVec2 data;
	s_keyframe_handle left_handle;
	s_keyframe_handle right_handle;
};

