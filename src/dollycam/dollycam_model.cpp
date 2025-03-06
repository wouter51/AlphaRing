#include "dollycam_model.h"

#include <nlohmann/json.hpp>

using namespace libmcc;

static constexpr std::array<std::tuple<const char*, real_rgb_color, ImVec2>, k_dollycam_series_count> series_configs {
	std::make_tuple("Position X",	real_rgb_color(1.0f, 0.0f, 0.0f), ImVec2(1.0f, 0.01f)),
	std::make_tuple("Position Y",	real_rgb_color(0.0f, 1.0f, 0.0f), ImVec2(1.0f, 0.01f)),
	std::make_tuple("Position Z",	real_rgb_color(0.0f, 0.0f, 1.0f), ImVec2(1.0f, 0.01f)),
	std::make_tuple("Facing Yaw",	real_rgb_color(1.0f, 1.0f, 0.0f), ImVec2(1.0f, 0.01f)),
	std::make_tuple("Facing Pitch",	real_rgb_color(1.0f, 0.0f, 1.0f), ImVec2(1.0f, 0.01f)),
	std::make_tuple("Roll",			real_rgb_color(0.0f, 1.0f, 1.0f), ImVec2(1.0f, 0.01f)),
	std::make_tuple("Fov",			real_rgb_color(1.0f, 1.0f, 1.0f), ImVec2(1.0f, 1.0f)),
};

c_dollycam_model::c_dollycam_model() {
	m_baked = false;

	for (auto i = 0; i < k_dollycam_series_count; i++) {
		auto& series = m_series[i];
		auto& [name, color, scale] = series_configs[i];

		series.enabled = true;
		series.visible = true;
		series.locked = false;

		series.name = name;

		series.scale = scale;

		libmcc::real_hsv_color hsv(color);

		hsv.value = 1.0f;

		series.selected_color = byte_argb_color(1.0f, static_cast<real_rgb_color>(hsv));

		hsv.value = 0.25f;

		series.unselected_color = byte_argb_color(1.0f, static_cast<real_rgb_color>(hsv));
	}
}

void c_dollycam_model::add(int tick, const s_dollycam_value* value, const s_dollycam_value_attribute* attribute) {
	float ftick = tick;
	int series_index = 0;

	for (auto& series : m_series) {
		if (!attribute->enabled[series_index]) {
			continue;
		}

		auto it = series.keyframes.begin();
		auto end = series.keyframes.end();

		while (true) {
			if (it == end || it->data.x > ftick) {
				s_keyframe keyframe;

				memset(&keyframe, 0, sizeof(s_keyframe));

				keyframe.data.x = ftick;
				keyframe.data.y = value->n[series_index];
				keyframe.interpolation = _keyframe_interpolation_linear;

				series.keyframes.insert(it, std::move(keyframe));

				break;
			} else if (it->data.x == ftick) {

				it->data.y = value->n[series_index];

				break;
			}
			++it;
		}

		++series_index;
	}
}

void c_dollycam_model::interp(int tick, s_dollycam_value* value, s_dollycam_value_attribute* attribute) {
	int series_index = 0;
	float ftick = tick;

	if (m_baked) {
		memcpy(value, &m_baked_values[tick], sizeof(s_dollycam_value));
	}

	for (auto& series : m_series) {
		attribute->enabled[series_index] = series.enabled;

		if (!series.enabled) {
			continue;
		}
		
		if (m_baked) {
			continue;
		}

		auto result = 0.0f;
		auto it = series.keyframes.begin();

		if (it->data.x > ftick) {
			result = it->data.y;
		} else {
			auto end = series.keyframes.end();

			while (it != end) {
				auto next = it; ++next;

				if (it->data.x >= ftick) {
					result = next == end ? it->data.y : it->calculate(ftick, &*next);
					break;
				}

				++it;
			}
		}

		value->n[series_index] = result;
		++series_index;
	}
}

void c_dollycam_model::update(std::list<s_keyframe>& which, std::list<s_keyframe>::iterator& where, e_keyframe_handle_type type, const libmcc::real_vector2d* offset) {
	switch (type) {
	case _keyframe_handle_left:
		where->left_handle.data.x += offset->i;
		where->left_handle.data.y += offset->j;

		where->left_handle.data.x = fmin(where->left_handle.data.x, where->data.x);
		break;
	case _keyframe_handle_middle:
		where->data.x += offset->i;
		where->data.y += offset->j;

		where->left_handle.data.x += offset->i;
		where->left_handle.data.y += offset->j;

		where->right_handle.data.x += offset->i;
		where->right_handle.data.y += offset->j;

		resort_keyframe(which, where);
		break;
	case _keyframe_handle_right:
		where->right_handle.data.x += offset->i;
		where->right_handle.data.y += offset->j;

		where->right_handle.data.x = fmax(where->right_handle.data.x, where->data.x);
		break;
	}
}

void c_dollycam_model::resort_keyframe(std::list<s_keyframe>& which, std::list<s_keyframe>::iterator& keyframe) {
	auto begin = which.begin();
	auto end = which.end();

	auto dst = keyframe;
	auto prev = keyframe; --prev;
	auto next = keyframe; ++next;

	bool need_move = false;

	while (true) {
		// check if the keyframe is larger than the next keyframe
		if (keyframe != end && next != end) {
			do {
				need_move = false;

				if (next != end && keyframe->data.x > next->data.x) {
					need_move = true;
					dst = next;
					++next;
				}

			} while (need_move);

		} else if (keyframe != begin) {

			do {
				need_move = false;

				if (prev != begin && keyframe->data.x < prev->data.x) {
					need_move = true;
					dst = prev;
					--prev;
				}

			} while (need_move);

		}
	}

	if (dst != keyframe) {
		which.splice(dst, which, keyframe);
	}
}

