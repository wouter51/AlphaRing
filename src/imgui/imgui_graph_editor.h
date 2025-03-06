#pragma once

#include <imgui.h>
#include <imgui_internal.h>

class c_imgui_graph_editor {
public:
	c_imgui_graph_editor();

	enum e_horizontal : char {
		_horizontal_left,
		_horizontal_center,
		_horizontal_right
	} horizontal;

	enum e_vertical : char {
		_vertical_top,
		_vertical_center,
		_vertical_bottom
	} vertical;

	union s_position {
		struct {
			e_horizontal horizontal;
			e_vertical vertical;
		};

		int n;

		constexpr s_position() {}
		constexpr s_position(e_horizontal h, e_vertical v) : horizontal(h), vertical(v) {}
	};

	void render();

	s_position calculate_position(const ImVec2& coordinate_position, ImVec2& screen_position);

protected:
	void prepare();
	void draw_background();
	void draw_grid();
	void draw_cursor();
	void handle_input();

protected:
	ImVec2 m_step_min;
	ImVec2 m_step_max;
	ImVec2 m_position;
	ImVec2 m_zoom;
	ImVec2 m_step;
	ImVec2ih m_ratio;
	ImVec2 m_cursor;

protected:
	char buffer[1024];
	ImDrawList* draw_list;
	ImVec2 unit;
	ImRect r0;
	ImVec2 r0_size;
	ImRect r1;
	ImVec2 r1_size;
	float frame_height;
	ImVec2 grid_size;
	ImVec2 origin;
	ImVec2 position;
	ImVec2 zoom_position;
	ImGuiWindow* window;
	ImGuiContext* context;
	ImGuiIO* io;

};

