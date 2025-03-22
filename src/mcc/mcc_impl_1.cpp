#include "mcc_impl.h"

#include "./mcc_manager.h"
#include "./player_manager.h"
#include "../d3d11/d3d11_manager.h"
#include "../imgui/imgui_manager.h"

using namespace libmcc;

void* (__fastcall* set_player_gamepad_original)(void* a1, int a2);

void* __fastcall set_player_gamepad(void* a1, int a2) {
	if (player_manager()->get_local_player_count() == 1) {
		return set_player_gamepad_original(a1, a2);
	} else {
		return nullptr;
	}
}

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

	if (!d3d11_manager()->check_swap_chain(pSwapChain) && !imgui_manager()->begin_frame()) {
		c_imgui_render::main();

		imgui_manager()->end_frame();

		imgui_manager()->set_game_render(true);
	}
}

static s_player_profile* __fastcall local_user_get_profile(i_game_manager* This, XUID xuid) {
	s_xdk_user user;
	static s_player_profile result;

	mcc_manager()->get_user_by_xuid(nullptr, &user, xuid);
	
	if (user.unknown_0 != nullptr) {
		auto profile = g_game_manager_vftable_original.local_user_get_profile(This, xuid);

		memcpy(&result, profile, sizeof(result));
	}

	return &result;
}

typedef DWORD (*t_XInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState);

t_XInputGetState get_xinput_get_state_function() {
	constexpr const wchar_t* xinput_module_names[]{
		L"xinput1_4.dll",
		L"xinput1_3.dll",
		L"xinput9_1_0.dll",
		L"xinput1_2.dll",
		L"xinput1_1.dll",
		L"xinput1_0.dll"
	};

	static HMODULE xinput = nullptr;
	static t_XInputGetState xinput_get_state = nullptr;

	if (xinput == nullptr) {
		for (int i = 0; i < _countof(xinput_module_names); ++i) {
			xinput = LoadLibraryW(xinput_module_names[i]);
			if (xinput) {
				break;
			}
		}

		if (xinput == nullptr) {
			return nullptr;
		}
	}

	if (xinput_get_state == nullptr) {
		xinput_get_state = reinterpret_cast<t_XInputGetState>(GetProcAddress(xinput, "XInputGetState"));
	}

	return xinput_get_state;
}

static bool __fastcall input_update(i_game_manager* This, e_local_player player, s_input_state* state) {
	auto input_device = player_manager()->get_input_device(player);

	auto use_km = !(player != _local_player_0 || input_device != _player_input_device_km);

	if (use_km) {
		state->is_km = true;

		return g_game_manager_vftable_original.input_update(This, player, state);
	} else {
		XINPUT_STATE xinput_state;

		memset(state, 0, sizeof(s_input_state));

		auto func = get_xinput_get_state_function();

		if (!func || func(input_device, &xinput_state) != ERROR_SUCCESS) {
			return true;
		}

		state->gamepad = s_gamepad_state(xinput_state);
	}

	return true;
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
	s_xdk_user user;
	static s_gamepad_mapping result;

	mcc_manager()->get_user_by_xuid(nullptr, &user, xuid);

	if (user.unknown_0 != nullptr) {
		memcpy(
			&result, 
			g_game_manager_vftable_original.local_user_get_gamepad_mapping(This, xuid), 
			sizeof(result));
	}

	return &result;
}
