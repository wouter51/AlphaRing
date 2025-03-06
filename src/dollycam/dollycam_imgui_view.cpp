#include "dollycam_imgui_view.h"

#include <imgui_internal.h>

#include "../mcc/mcc_manager.h"

using namespace libmcc;

constexpr ImU32 color_white = IM_COL32(255, 255, 255, 255);
constexpr ImU32 color_black = IM_COL32(0, 0, 0, 255);
constexpr ImU32 color_grey = IM_COL32(128, 128, 128, 255);
constexpr ImU32 color_drak = IM_COL32(50, 50, 50, 255);
constexpr ImU32 color_line = IM_COL32(200, 200, 200, 40);
constexpr ImU32 color_blue = IM_COL32(0, 168, 255, 255);

c_dollycam_imgui_view::c_dollycam_imgui_view() {
	m_frame = m_start_frame = m_end_frame = 0.0f;

	m_selected_keyframe = nullptr;
	m_model = &g_dollycam_model;
}

void c_dollycam_imgui_view::render(bool* show) {
	do {
		auto flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;

		if (!ImGui::Begin("Dollycam", show, flags)) {
			break;
		}

		static ImGuiID dockspace_id = ImGui::GetID("Dockspace");
		static ImGuiID left_node_id = 0;
		static ImGuiID middle_node_id = 0;
		static ImGuiID right_node_id = 0;

		flags = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoTabBar;

		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), flags);

		// initialize dockspace
		do {
			static bool initialized = false;

			if (initialized) {
				break;
			}

			ImGui::DockBuilderRemoveNodeChildNodes(dockspace_id);

			ImGui::DockBuilderSplitNode(
				ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.8f, nullptr, &right_node_id), 
				ImGuiDir_Left, 0.25f, &left_node_id, &middle_node_id);

			initialized = true;
		} while (false);

		render_toolbar();

		ImGui::SetNextWindowDockID(left_node_id, ImGuiCond_Always);
		ImGui::Begin("Left View") ? render_left_view(), 1 : 0;
		ImGui::End();

		ImGui::SetNextWindowDockID(middle_node_id, ImGuiCond_Always);
		ImGui::Begin("Main View") ? render_main_view(), 1 : 0;
		ImGui::End();

		ImGui::SetNextWindowDockID(right_node_id, ImGuiCond_Always);
		ImGui::Begin("Right View") ? render_right_view(), 1 : 0;
		ImGui::End();

	} while (false);

	ImGui::End();
}

void c_dollycam_imgui_view::render_toolbar() {
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New")) {
		}

		if (ImGui::MenuItem("Open")) {
		}

		if (ImGui::MenuItem("Save")) {
			nlohmann::json j(*m_model);

			auto dump = j.dump(4);

			mcc_manager()->write_resource("dollycam.json", dump.c_str(), dump.length());
		}

		if (ImGui::MenuItem("Save As")) {

		}
		ImGui::EndMenu();
	}

	auto window = ImGui::GetCurrentWindow();

	auto context = ImGui::GetCurrentContext();

	auto x0 = window->DC.CursorStartPos.x;

	auto x1 = window->ContentRegionRect.Max.x;

	{
		auto button_count = 6;

		auto button_length = context->FontSize + context->Style.FramePadding.y * 2.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 1.0f, 0.0f });

		auto button_group_width = button_count * button_length + context->Style.ItemSpacing.x * (button_count - 1);

		ImGui::SetCursorPosX(fmax(ImGui::GetCursorPosX(), (x1 - x0 - button_group_width) * 0.4f));

		if (ImGui::ArrowButton("Jump to start", ImGuiDir_Left)) {

		}

		if (ImGui::ArrowButton("Jump to previous keyframe", ImGuiDir_Left)) {

		}

		if (ImGui::ArrowButton("Play animation backward", ImGuiDir_Left)) {

		}

		if (ImGui::ArrowButton("Play animation forward", ImGuiDir_Right)) {
		}

		if (ImGui::ArrowButton("Jump to next keyframe", ImGuiDir_Right)) {
		}

		if (ImGui::ArrowButton("Jump to end", ImGuiDir_Right)) {
		}

		ImGui::PopStyleVar();
	}

	{
		auto drag_count = 3;

		auto drag_length = ImGui::CalcTextSize("4294967295").x + context->Style.FramePadding.y * 2.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 1.0f, 0.0f });

		auto drag_group_width = drag_length * drag_count + context->Style.ItemSpacing.x * (drag_count - 1);

		ImGui::SetCursorPosX(fmax(ImGui::GetCursorPosX(), x1 - x0 - drag_group_width));

		ImGui::PushItemWidth(drag_length);

		if (ImGui::DragFloat("##Current Frame", &m_frame, 1.0f, 0.0f, 0.0f, "%.0f")) {
			m_frame = fmax(round(m_frame), 0);
		}

		if (ImGui::DragFloat("##Start Frame", &m_start_frame, 1.0f, 0.0f, 0.0f, "%.0f")) {
			m_start_frame = fmin(fmax(roundf(m_start_frame), 0), m_end_frame);
		}

		if (ImGui::DragFloat("##End Frame", &m_end_frame, 1.0f, 0.0f, 0.0f, "%.0f")) {
			m_end_frame = fmax(roundf(m_end_frame), m_start_frame);
		}

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
	}

	ImGui::EndMenuBar();
}

void c_dollycam_imgui_view::render_left_view() {
	s_id id;

	for (auto& series : m_model->series()) {
		ImGui::Text(series.name.c_str());

		ImGui::SameLine();

		id.index = 0;
		ImGui::PushID(id);
		ImGui::Checkbox("V", &series.visible);
		ImGui::PopID();

		ImGui::SameLine();

		id.index = 1;
		ImGui::PushID(id);
		ImGui::Checkbox("E", &series.enabled);
		ImGui::PopID();

		ImGui::SameLine();

		id.index = 2;
		ImGui::PushID(id);
		ImGui::Checkbox("L", &series.locked);
		ImGui::PopID();

		++id.series;
	}
}

void c_dollycam_imgui_view::render_main_view() {
	c_imgui_graph_editor::render();

	for (m_current_id.n = 0; m_current_id.series < k_dollycam_series_count; ++m_current_id.series) {
		auto series = &m_model->series().at(m_current_id.series);

		if (!series->visible) {
			continue;
		}

		draw_series(series);
	}
}

void c_dollycam_imgui_view::render_right_view() {
	if (m_selected_series == nullptr || m_selected_keyframe == nullptr) {
		return;
	}

	auto& scale = m_selected_series->scale;

	s_keyframe keyframe = *m_selected_keyframe;

	if (ImGui::DragFloat("Frame", &keyframe.data.x, scale.x)) {

	}

	if (ImGui::DragFloat("Value", &keyframe.data.y, scale.y)) {

	}

	if (ImGui::DragFloat("Left Handle Frame", &keyframe.left_handle.data.x, scale.x)) {

	}

	if (ImGui::DragFloat("Left Handle Value", &keyframe.left_handle.data.y, scale.y)) {

	}

	if (ImGui::DragFloat("Right Handle Frame", &keyframe.right_handle.data.x, scale.x)) {

	}

	if (ImGui::DragFloat("Right Handle Value", &keyframe.right_handle.data.y, scale.y)) {

	}
}

void c_dollycam_imgui_view::draw_series(const s_series* series) {
	auto it = series->keyframes.begin();
	auto end = series->keyframes.end();
	auto color = m_selected_series == series ? series->selected_color : series->unselected_color;

	if (it != end) {
		draw_keyframe(series, nullptr, &*it, color);

		auto next = it; ++next;

		while (next != end) {
			draw_keyframe(series, &*it, &*next, color);

			++it;
			++next;
		}

		draw_keyframe(series, &*it, nullptr, color);
	} else {
		draw_keyframe(series, nullptr, nullptr, series->unselected_color);
	}
}

void c_dollycam_imgui_view::draw_keyframe(const s_series* series, const s_keyframe* k0, const s_keyframe* k1, ImU32 color, float thickness) {
	enum {
		_k0,
		_k0_right_handle,
		_k1,
		_k1_left_handle,
		k_count
	};

	constexpr s_position center = { _horizontal_center, _vertical_center };

	struct s_location {
		s_location() {}

		ImVec2 coordinate;
		s_position position;
	} positions[k_count];

	// if keyframes are null draw a line
	if (k0 == nullptr && k1 == nullptr) {
		ImVec2 origin = { 0.0f, 0.0f }, screen_origin;

		auto pos = calculate_position(origin, screen_origin);

		if (pos.horizontal != _horizontal_center) {
			return;
		}

		positions[_k0].coordinate = ImVec2(r1.Min.x, screen_origin.y);
		positions[_k1].coordinate = ImVec2(r1.Max.x, screen_origin.y);

		draw_list->AddLine(positions[_k0].coordinate, positions[_k1].coordinate, color, thickness);

		return;
	}

	// calculate positions
	if (k0 != nullptr) {
		positions[_k0].position = calculate_position(k0->data, positions[_k0].coordinate);
		positions[_k0_right_handle].position = calculate_position(k0->right_handle.data, positions[_k0_right_handle].coordinate);
	}

	if (k1 != nullptr) {
		positions[_k1].position = calculate_position(k1->data, positions[_k1].coordinate);
		positions[_k1_left_handle].position = calculate_position(k1->left_handle.data, positions[_k1_left_handle].coordinate);
	}

	// draw keyframes and handles
	if (k0 == nullptr) {
		if (positions[_k1].position.horizontal != _horizontal_left) {
			positions[_k0].coordinate = ImVec2(r1.Min.x, positions[_k1].coordinate.y);
			draw_list->AddLine(positions[_k0].coordinate, positions[_k1].coordinate, color, thickness);
		}

		if (positions[_k1].position.n & center.n) {
			draw_list->AddCircleFilled(positions[_k1].coordinate, thickness, color);

		}
	} else if (k1 == nullptr) {
		if (positions[_k0].position.horizontal != _horizontal_right) {
			positions[_k1].coordinate = ImVec2(r1.Max.x, positions[_k0].coordinate.y);
			draw_list->AddLine(positions[_k0].coordinate, positions[_k1].coordinate, color, thickness);
		}

		if (positions[_k0].position.n & center.n) {
			draw_list->AddCircleFilled(positions[_k0].coordinate, thickness, color);
		}
	} else {
		if ((positions[_k0].position.n & center.n) ||
			(positions[_k1].position.n & center.n) ||
			!(positions[_k0].position.n & positions[_k1].position.n)) {
			draw_list->AddLine(positions[_k0].coordinate, positions[_k1].coordinate, color, thickness);
		}

		if ((positions[_k0].position.n & center.n) ||
			(positions[_k0_right_handle].position.n & center.n) ||
			!(positions[_k0].position.n & positions[_k0_right_handle].position.n)) {
			draw_list->AddCircleFilled(positions[_k0].coordinate, thickness, color);

			if (k0->interpolation == _keyframe_interpolation_bezier) {
				draw_list->AddLine(positions[_k0].coordinate, positions[_k0_right_handle].coordinate, color_black);
				draw_list->AddCircle(positions[_k0_right_handle].coordinate, thickness, color_black);
			}
		}

		if ((positions[_k1].position.n & center.n) ||
			(positions[_k1_left_handle].position.n & center.n) ||
			!(positions[_k1].position.n & positions[_k1_left_handle].position.n)) {
			draw_list->AddCircleFilled(positions[_k1].coordinate, thickness, color);

			if (k0->interpolation == _keyframe_interpolation_bezier) {
				draw_list->AddLine(positions[_k1].coordinate, positions[_k1_left_handle].coordinate, color_black);
				draw_list->AddCircle(positions[_k1_left_handle].coordinate, thickness, color_black);
			}
		}
	}
}
