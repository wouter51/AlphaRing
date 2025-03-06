#include "keyframe.h"

s_keyframe::s_keyframe() {
}

float s_keyframe::calculate(float frame, const s_keyframe* other) const {
	return calculateT((frame - data.x) / (other->data.x - data.x), other);
}

float s_keyframe::calculateT(float t, const s_keyframe* other) const {
	if (t <= 0.001f) {
		return data.y;
	}

	if (t >= 0.999f) {
		return other->data.y;
	}

	switch (interpolation) {
	case _keyframe_interpolation_constant: {
		return data.y;
	}
	case _keyframe_interpolation_linear: {
		return data.y + (other->data.y - data.y) * t;
	}
	case _keyframe_interpolation_bezier: {
		float u = 1.0f - t;
		float w1 = u * u * u;
		float w2 = 3 * u * u * t;
		float w3 = 3 * u * t * t;
		float w4 = t * t * t;
		return w1 * data.y + w2 * right_handle.data.y + w3 * other->left_handle.data.y + w4 * other->data.y;
	}
	default: {
		return 0.0f;
	}
	}
}
