#pragma once

#include <libmcc/libmcc.h>

enum e_mcc_type {
	_mcc_type_steam,
	_mcc_type_winstore,

	k_mcc_type_count,
};

enum e_mcc_offset {
	_mcc_offset_game_manager_vftable,
	_mcc_offset_game_globals_states,
	_mcc_offset_game_globals,
	_mcc_offset_set_player_input,

	k_mcc_offset_count,
};

template<typename T>
struct s_mcc_offset {
	constexpr s_mcc_offset(e_mcc_offset offset) : offset(offset) {}
	e_mcc_offset offset;
};

struct s_mcc_offset_map {
	s_mcc_offset<libmcc::i_game_manager_vftable*> game_manager_vftable = _mcc_offset_game_manager_vftable;
	s_mcc_offset<libmcc::s_game_globals_states> game_globals_states = _mcc_offset_game_globals_states;
	s_mcc_offset<libmcc::s_game_globals*> game_globals = _mcc_offset_game_globals;
	s_mcc_offset<void> set_player_input = _mcc_offset_set_player_input;
};

constexpr s_mcc_offset_map g_mcc_offset_map;

constexpr uintptr_t g_mcc_offset_table[k_mcc_type_count][k_mcc_offset_count]
{{
	libmcc::mcc::s_data_offset_table::game_manager.first,
	libmcc::mcc::s_data_offset_table::game_globals_states.first,
	libmcc::mcc::s_data_offset_table::p_game_globals.first,
	libmcc::mcc::s_function_offset_table::set_player_gamepad.first,
}, {
	libmcc::mccwinstore::s_data_offset_table::game_manager.first,
	libmcc::mccwinstore::s_data_offset_table::game_globals_states.first,
	libmcc::mccwinstore::s_data_offset_table::p_game_globals.first,
	libmcc::mccwinstore::s_function_offset_table::set_player_gamepad.first
}};
