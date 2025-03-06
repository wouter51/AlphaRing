#pragma once

#include <libmcc/libmcc.h>

// todo: use original

extern libmcc::s_game_globals_states g_game_globals_states;
inline libmcc::s_game_globals_states g_game_globals_states_original;

extern libmcc::i_game_manager_vftable g_game_manager_vftable;
inline libmcc::i_game_manager_vftable g_game_manager_vftable_original;
