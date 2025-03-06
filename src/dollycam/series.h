#pragma once

#include <list>
#include <string>
#include <vector>

#include "keyframe.h"

struct s_series {
	bool enabled;
	bool visible;
	bool locked;
	ImU32 selected_color;
	ImU32 unselected_color;
	ImVec2 scale;
	std::string name;
	std::list<s_keyframe> keyframes;
};
