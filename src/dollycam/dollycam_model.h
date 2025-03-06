#pragma once

#include "series.h"

#include <libmcc/libmcc.h>

#include <nlohmann/json.hpp>

enum e_dollycam_series {
	_dollycam_series_position_x,
	_dollycam_series_position_y,
	_dollycam_series_position_z,
	_dollycam_series_facing_yaw,
	_dollycam_series_facing_pitch,
	_dollycam_series_roll,
	_dollycam_series_fov,

	k_dollycam_series_count,
	k_dollycam_series_none = -1
};

union s_dollycam_value_attribute {
	bool enabled[k_dollycam_series_count];
	unsigned long long n;
};

union s_dollycam_value {
	s_dollycam_value() {}

	struct {
		libmcc::real_point3d position;
		libmcc::real_euler_angles2d facing;
		libmcc::real roll;
		libmcc::real fov;
	};

	libmcc::real n[k_dollycam_series_count];
};

namespace nlohmann {
	template<>
	struct adl_serializer<ImVec2> {
		static void to_json(json& j, const ImVec2& value) {
			j = json{ value.x, value.y };
		}
		static void from_json(const json& j, ImVec2& value) {
			value.x = j.at(0).get<float>();
			value.y = j.at(1).get<float>();
		}
	};

	template<>
	struct adl_serializer<libmcc::real_point3d> {
		static void to_json(json& j, const libmcc::real_point3d& value) {
			j = json{ value.x, value.y, value.z };
		}
		static void from_json(const json& j, libmcc::real_point3d& value) {
			value.x = j.at(0).get<libmcc::real>();
			value.y = j.at(1).get<libmcc::real>();
			value.z = j.at(2).get<libmcc::real>();
		}
	};

	template<>
	struct adl_serializer<libmcc::real_euler_angles2d> {
		static void to_json(json& j, const libmcc::real_euler_angles2d& value) {
			j = json{ value.yaw, value.pitch };
		}
		static void from_json(const json& j, libmcc::real_euler_angles2d& value) {
			value.yaw = j.at(0).get<libmcc::angle>();
			value.pitch = j.at(1).get<libmcc::angle>();
		}
	};
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(s_keyframe_handle, data)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(s_keyframe, interpolation, data, left_handle, right_handle)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(s_series, enabled, visible, locked, selected_color, unselected_color, scale, name, keyframes)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(s_dollycam_value, position, facing, roll, fov);

class c_dollycam_model {
public:
	c_dollycam_model();

	// data access
	inline std::array<s_series, k_dollycam_series_count>& series() { return m_series; }

	void add(int tick, const s_dollycam_value* value, const s_dollycam_value_attribute* attribute);
	void update(std::list<s_keyframe>& which, std::list<s_keyframe>::iterator& where, e_keyframe_handle_type type, const libmcc::real_vector2d* offset);
	void interp(int tick, s_dollycam_value* value, s_dollycam_value_attribute* attribute);

	void resort_keyframe(std::list<s_keyframe>& which, std::list<s_keyframe>::iterator& keyframe);

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(c_dollycam_model, m_baked, m_baked_values, m_series)

private:
	bool m_baked;
	std::vector<s_dollycam_value> m_baked_values;
	std::array<s_series, k_dollycam_series_count> m_series;

};

inline c_dollycam_model g_dollycam_model;
