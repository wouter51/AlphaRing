#pragma once

#include "dollycam_model.h"

class c_dollycam_game_update {
public:
	c_dollycam_game_update();
	void execute();
	inline void set_play(bool play) { m_play = play; }

private:
	bool m_play;
	c_dollycam_model* m_model;

};