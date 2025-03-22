#pragma once

#include <libmcc/libmcc.h>

extern void* (__fastcall* set_player_gamepad_original)(void* a1, int a2);
extern void* __fastcall set_player_gamepad(void* a1, int a2);

extern libmcc::s_game_globals_states g_game_globals_states;
inline libmcc::s_game_globals_states g_game_globals_states_original;

extern libmcc::i_game_manager_vftable g_game_manager_vftable;
inline libmcc::i_game_manager_vftable g_game_manager_vftable_original;
