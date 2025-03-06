#include "dollycam_game_view.h"

#include <queue>
#include <algorithm>

#include <libmcc/libmcc.h>

using namespace libmcc;
using namespace halo3;

c_dollycam_game_view::c_dollycam_game_view() {
	m_visibility = false;
	m_model = &g_dollycam_model;
}

void c_dollycam_game_view::render() {
	if (!m_visibility) {
		return;
	}

	if (m_model == nullptr) {
		return;
	}

	std::priority_queue<float> ticks;
	std::list<s_keyframe>::const_iterator it[3];
	std::list<s_keyframe>::const_iterator next[3];
	std::list<s_keyframe>::const_iterator end[3];

	for (int i = 0; i <= _dollycam_series_position_z; ++i) {
		auto keyframes = &m_model->series().at(i).keyframes;

		keyframes->begin();

		if (keyframes->size() <= 1) {
			return;
		}

		for (auto& keyframe : *keyframes) {
			ticks.push(keyframe.data.x);
		}

		it[i] = keyframes->begin();
		next[i] = it[i]; ++next[i];
		end[i] = keyframes->end();
	}

	float tick_begin = ticks.top(); ticks.pop();
	float tick_end = ticks.top(); ticks.pop();

	real_point3d positions[2];

	while (!ticks.empty()) {
		int p0 = -1;
		int p1 = 0;
		c_render_debug_line_drawer drawer;
		
		for (int l = 0; l <= m_level; ++l) {
			auto tick = tick_begin + (tick_end - tick_begin) * l / m_level;

			for (int i = 0; i <= _dollycam_series_position_z; ++i) {
				if (next[i] != end[i] && next[i]->data.x < tick_end) {
					++it[i];
					++next[i];
				}

				if (next[i] != end[i] && it[i]->data.x < tick_end) {
					positions[p1].n[i] = it[i]->calculate(tick, &*next[i]);
				} else {
					positions[p1].n[i] = it[i]->data.y;
				}

				if (p0 != -1) {
					drawer.add_line_3d(positions + p0, positions + p1);
				}

				p0 = p1;
				p1 = (p1 + 1) % 2;
			}
		}

		tick_begin = tick_end;
		tick_end = ticks.top();
		ticks.pop();
	}
}
