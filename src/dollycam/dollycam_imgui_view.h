#pragma once

#include "dollycam_model.h"

#include "../imgui/imgui_graph_editor.h"

class c_dollycam_imgui_view : public c_imgui_graph_editor {
public:
	enum e_type {
		_type_frame_left_handle,
		_type_frame,
		_type_frame_right_handle,

		k_type_count
	};

	union s_id {
		s_id() : n(0) {}

		operator int() const { return n; }

		int n;
		struct {
			unsigned char series : 3;
			int index : 27;
			unsigned char type : 2;
		};
	};

public:
	c_dollycam_imgui_view();

	void render(bool* show);

private:
	void render_toolbar();
	void render_left_view();
	void render_main_view();
	void render_right_view();

	void draw_series(const s_series* series);
	void draw_keyframe(const s_series* series, const s_keyframe* k0, const s_keyframe* k1, ImU32 color, float thickness = 3.0f);

	const int k_maximum_keyframes = (1 << (32 - 3 - 2));

	float m_frame;
	float m_start_frame;
	float m_end_frame;

	c_dollycam_model* m_model;

	s_series* m_selected_series;
	s_keyframe* m_selected_keyframe;

	s_id m_current_id;

};

inline c_dollycam_imgui_view g_dollycam_imgui_view;
