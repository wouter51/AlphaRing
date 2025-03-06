#pragma once

#include "../common.h"
#include "./imgui_render.h"

#include <d3d11.h>

class c_imgui_manager : public i_manager {
public:
	int initialize() override;
	int shutdown() override;
	bool initialized() { return m_initialized; }

	int begin_frame();
	int end_frame();

	inline void set_game_render(bool value) { m_game_render = value; }
	inline bool get_game_render() { return m_game_render; }

	int create_view();
	int destroy_view();

	bool get_visibility();
	void set_visibility(bool visibility);

	bool want_capture_cursor();
	void reset_cursor_position();

private:
	bool m_initialized = false;
	bool m_game_render = false;
	bool m_visibility = true;
	ID3D11RenderTargetView* m_imgui_view;
};

inline c_imgui_manager g_imgui_manager;

inline c_imgui_manager* imgui_manager() {
	return &g_imgui_manager;
}
