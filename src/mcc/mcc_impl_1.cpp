#include "mcc_impl.h"

#include "./mcc_manager.h"
#include "./player_manager.h"
#include "../d3d11/d3d11_manager.h"
#include "../imgui/imgui_manager.h"

using namespace libmcc;

// game manager
static void __fastcall signal_end_frame(i_game_manager* This, IDXGISwapChain* pSwapChain, UINT* flags);
static s_player_profile* __fastcall local_user_get_profile(i_game_manager* This, XUID xuid);
static bool __fastcall input_update(i_game_manager* This, e_local_player player, s_input_state* state);
static void __fastcall input_set_rumble(i_game_manager* This, e_local_player player, s_rumble_state* state);
static bool __fastcall local_user_get_player(i_game_manager* This, XUID* xuid, wchar_t* name, uint32_t size, e_local_player player);
static s_gamepad_mapping* __fastcall local_user_get_gamepad_mapping(i_game_manager* This, XUID xuid);

libmcc::i_game_manager_vftable g_game_manager_vftable {
	.signal_end_frame = signal_end_frame,
	.local_user_get_profile = local_user_get_profile,
	.input_update = input_update,
	.input_set_rumble = input_set_rumble,
	.local_user_get_player = local_user_get_player,
	.local_user_get_gamepad_mapping = local_user_get_gamepad_mapping
};

static void __fastcall signal_end_frame(i_game_manager* This, IDXGISwapChain* pSwapChain, UINT* flags) {
	g_game_manager_vftable_original.signal_end_frame(This, pSwapChain, flags);

	static auto p_game_data = *mcc::g_game_data();

	if (!d3d11_manager()->check_swap_chain(pSwapChain) && !imgui_manager()->begin_frame()) {
		c_imgui_render::main();

		auto title = p_game_data->game_title;

		if (title >= _module_halo1 && title <= _module_haloreach) {
			c_imgui_render::game_render[title]();
		}

		imgui_manager()->end_frame();

		imgui_manager()->set_game_render(true);
	}
}

static s_player_profile* __fastcall local_user_get_profile(i_game_manager* This, XUID xuid) {
	static s_player_profile result;
	s_xdk_user user;

	mcc::get_user_by_xuid(nullptr, &user, xuid);
	
	if (user.unknown_0 == nullptr) {
		c_critical_section cs(_critical_section_player);

		for (int i = 0; i < player_manager()->get_local_player_count(); ++i) {
			if (player_manager()->get_xuid(i) == xuid) {
				static auto p_game_data = *libmcc::mcc::g_game_data();

				memcpy(
					&result,
					&player_manager()->get_profile(i)->game_profile[p_game_data->game_title].profile,
					sizeof(s_player_profile));

				return &result;
			}
		}
	}

	return mcc::get_user_profile(nullptr, &user);
}

static bool __fastcall input_update(i_game_manager* This, e_local_player player, s_input_state* state) {
	if (player == 0) {
		return g_game_manager_vftable_original.input_update(This, player, state);
	} else {
		memset(state, 0, sizeof(s_input_state));

		return true;
	}
}

static void __fastcall input_set_rumble(i_game_manager* This, e_local_player player, s_rumble_state* state) {
	if (player == 0) {
		return g_game_manager_vftable_original.input_set_rumble(This, player, state);
	} else {
		return;
	}
}

static bool __fastcall local_user_get_player(i_game_manager* This, XUID* xuid, wchar_t* name, uint32_t size, e_local_player player) {
	if (player == 0) {
		return g_game_manager_vftable_original.local_user_get_player(This, xuid, name, size, player);
	} else {
		c_critical_section cs(_critical_section_player);

		if (player <= 0 || player >= player_manager()->get_local_player_count()) {
			return false;
		}

		if (xuid) {
			*xuid = player_manager()->get_xuid(player);
		}

		if (name && size) {
			memcpy(name, player_manager()->get_profile(player)->name, size);
		}

		return true;
	}
}

static s_gamepad_mapping* __fastcall local_user_get_gamepad_mapping(i_game_manager* This, XUID xuid) {
	static s_gamepad_mapping result;
	s_xdk_user user;

	mcc::get_user_by_xuid(nullptr, &user, xuid);

	if (user.unknown_0 == nullptr) {
		c_critical_section cs(_critical_section_player);

		for (int i = 0; i < player_manager()->get_local_player_count(); ++i) {
			if (player_manager()->get_xuid(i) == xuid) {
				static auto p_game_data = *libmcc::mcc::g_game_data();

				memcpy(
					&result,
					&player_manager()->get_profile(i)->game_profile[p_game_data->game_title].gamepad_mapping,
					sizeof(s_gamepad_mapping));

				return &result;
			}
		}
	}

	return g_game_manager_vftable_original.local_user_get_gamepad_mapping(This, xuid);
}
