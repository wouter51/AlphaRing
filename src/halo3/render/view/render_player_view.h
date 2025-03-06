#pragma once

#include <libmcc/libmcc.h>

namespace halo3 {
	template<typename T>
	struct c_view_vftable {
		void(__fastcall* render)(T* view);
		int(__fastcall* render_setup)(T* view);
		int(__fastcall* compute_visibility)(T* view);
		int(__fastcall* render_submit_visibility)(T* view);
	};

	extern c_view_vftable<libmcc::halo3::c_player_view> g_player_view_vftable;
	inline c_view_vftable<libmcc::halo3::c_player_view> g_player_view_vftable_original;
}
