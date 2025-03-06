#pragma once

#include "dollycam_model.h"

class c_dollycam_game_view {
public:
	c_dollycam_game_view();

	void render();

	inline void set_visibility(bool visibility) { m_visibility = visibility; }

private:
	bool m_visibility;
	int m_level = 10;
	c_dollycam_model* m_model;
};

inline c_dollycam_game_view g_dollycam_game_view;
