#include "imgui_graph_editor.h"

#include <array>

static constexpr std::array<ImVec2, 3> RATIO {
	ImVec2(2.0f, 0.5f),
	ImVec2(2.5f, 0.4f),
	ImVec2(2.0f, 0.5f),
};

constexpr ImU32 color_white = IM_COL32(255, 255, 255, 255);
constexpr ImU32 color_black = IM_COL32(0, 0, 0, 255);
constexpr ImU32 color_grey = IM_COL32(128, 128, 128, 255);
constexpr ImU32 color_drak = IM_COL32(50, 50, 50, 255);
constexpr ImU32 color_line = IM_COL32(200, 200, 200, 40);
constexpr ImU32 color_blue = IM_COL32(0, 168, 255, 255);

c_imgui_graph_editor::c_imgui_graph_editor() {
	memset(this, 0, sizeof(c_imgui_graph_editor));

	m_position = { 0.0f, 0.0f };
	m_step = { 100.0f, 1.0f };
	m_zoom = { 1.0f, 1.0f };
	m_step = { 100.0f, 1.0f };
	m_step_min = { 1.0f, 0.01f };
	m_step_max = { 100000.0f, 1000.0f };
}

void c_imgui_graph_editor::render() {
	prepare();

	draw_background();

	draw_grid();

	draw_cursor();

	handle_input();
}

c_imgui_graph_editor::s_position c_imgui_graph_editor::calculate_position(const ImVec2& coordinate_position, ImVec2& screen_position) {
	s_position position;

	screen_position.x = r1.Min.x + (coordinate_position.x - m_position.x) * unit.x;
	screen_position.y = r1.Min.y + (coordinate_position.y - m_position.y) * unit.y;

	if (screen_position.x < r1.Min.x) {
		position.horizontal = _horizontal_left;
	} else if (screen_position.x > r1.Max.x) {
		position.horizontal = _horizontal_right;
	} else {
		position.horizontal = _horizontal_center;
	}

	if (screen_position.y < r1.Min.y) {
		position.vertical = _vertical_top;
	} else if (screen_position.y > r1.Max.y) {
		position.vertical = _vertical_bottom;
	} else {
		position.vertical = _vertical_center;
	}

	return position;
}

void c_imgui_graph_editor::prepare() {
	context = ImGui::GetCurrentContext();
	window = ImGui::GetCurrentWindowRead();
	draw_list = ImGui::GetWindowDrawList();
	io = &ImGui::GetIO();

	frame_height = context->FontSize + context->Style.FramePadding.y * 2.0f;

	r0 = ImRect(
		window->ContentRegionRect.Min, 
		{ window->ContentRegionRect.Max.x, window->ContentRegionRect.Min.y + frame_height });

	r0_size = ImVec2(
		r0.Max.x - r0.Min.x, 
		r0.Max.y - r0.Min.y);

	r1 = ImRect(
		{ window->ContentRegionRect.Min.x, r0.Max.y }, 
		window->ContentRegionRect.Max);

	r1_size = ImVec2(
		r1.Max.x - r1.Min.x, 
		r1.Max.y - r1.Min.y);

	grid_size = ImVec2(
		m_zoom.x * frame_height * 5, 
		m_zoom.y * frame_height * 5);

	unit = ImVec2(
		grid_size.x / m_step.x, 
		grid_size.y / m_step.y);
}

void c_imgui_graph_editor::draw_background() {
	draw_list->AddRect(window->ContentRegionRect.Min, window->ContentRegionRect.Max, color_white);
	draw_list->AddRectFilled(r0.Min, r0.Max, color_grey);
	draw_list->AddRectFilled(r1.Min, r1.Max, color_drak);
}

void c_imgui_graph_editor::draw_grid() {
	ImVec2 position, zoom_position(r1.Min);

	if (m_position.x < 0.001f) {
		auto offset = fmodf(m_position.x, m_step.x);
		position.x = m_position.x - offset;
		zoom_position.x = r1.Min.x - offset * unit.x;
	} else if (m_position.x > 0.001f) {
		auto offset = fmodf(m_step.x - fmodf(m_position.x, m_step.x), m_step.x);
		position.x = m_position.x + offset;
		zoom_position.x = r1.Min.x + offset * unit.x;
	}

	if (m_position.y < 0.001f) {
		auto offset = fmodf(m_position.y, m_step.y);
		position.y = m_position.y - offset;
		zoom_position.y = r1.Min.y - offset * unit.y;
	} else if (m_position.y > 0.001f) {
		auto offset = fmodf(m_step.y - fmodf(m_position.y, m_step.y), m_step.y);
		position.y = m_position.y + offset;
		zoom_position.y = r1.Min.y + offset * unit.y;
	}

	while (zoom_position.x < r1.Max.x) {
		sprintf(buffer, "%.0f", position.x);

		draw_list->AddText({ zoom_position.x, r0.Min.y }, color_white, buffer);
		draw_list->AddLine({ zoom_position.x, r1.Min.y }, { zoom_position.x, r1.Max.y }, color_line);

		position.x += m_step.x;
		zoom_position.x += grid_size.x;
	}

	while (zoom_position.y < r1.Max.y) {
		sprintf(buffer, "%.2f", position.y);

		draw_list->AddText({ r1.Min.x , zoom_position.y }, color_white, buffer);
		draw_list->AddLine({ r1.Min.x, zoom_position.y }, { r1.Max.x, zoom_position.y }, color_line);

		position.y += m_step.y;
		zoom_position.y += grid_size.y;
	}
}

void c_imgui_graph_editor::draw_cursor() {
	auto offset = (m_cursor.x - m_position.x) * unit.x;

	if (offset >= 0) {
		if (offset <= r0_size.x) {
			sprintf(buffer, "%.0f", m_cursor.x);

			draw_list->AddRectFilled(
				{ r0.Min.x + offset - frame_height, r0.Min.y },
				{ r0.Min.x + offset + frame_height, r0.Min.y + frame_height },
				color_blue);

			draw_list->AddText(
				{ r0.Min.x + offset - frame_height, r0.Min.y },
				color_white,
				buffer);

			draw_list->AddLine(
				{ r0.Min.x + offset, r1.Min.y },
				{ r0.Min.x + offset, r1.Max.y },
				color_blue);
		}
	}
}

void c_imgui_graph_editor::handle_input() {
	ImGui::SetCursorScreenPos(r0.Min);
	ImGui::InvisibleButton("r0", r0_size, ImGuiButtonFlags_MouseButtonLeft);

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
		m_cursor.x += io->MouseDelta.x * m_step.x * 0.01f;
	}

	ImGui::SetCursorScreenPos(r1.Min);
	ImGui::InvisibleButton("r1", r1_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

	if (ImGui::IsItemActive() &&
		ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f)) {
		m_position.x -= io->MouseDelta.x * m_step.x * 0.01f;
		m_position.y -= io->MouseDelta.y * m_step.y * 0.01f;
	}

	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {

		}

		auto handle_zoom = [](float zoom, float* mz, float* ms, float msmax, float msmin, short* mr) {
			if (zoom < 0 && *ms < msmax) {
				*mz += zoom;
				auto& ratio = RATIO[*mr];

				if (*mz < ratio.y) {
					*mz = 1.0f;
					*ms *= ratio.x;
					*mr = (*mr + 1 + RATIO.size()) % RATIO.size();
				}
			} else if (zoom > 0 && *ms > msmin) {
				*mz += zoom;

				auto last = (*mr - 1 + RATIO.size()) % RATIO.size();
				auto& ratio = RATIO[last];

				if (*mz > ratio.x) {
					*mz = 1.0f;
					*ms *= ratio.y;
					*mr = last;
				}
			}
			};

		auto zoom = io->MouseWheel * 0.1f;

		handle_zoom(zoom, &m_zoom.x, &m_step.x, m_step_max.x, m_step_min.x, &m_ratio.x);
		handle_zoom(zoom, &m_zoom.y, &m_step.y, m_step_max.y, m_step_min.y, &m_ratio.y);
	}
}
