#include "./mcc_impl.h"

#include "./mcc_manager.h"
#include "./module_manager.h"

using namespace libmcc;

static void game_globals_state_launch_update(s_game_globals* game_globals);
static void game_globals_state_exit_enter(s_game_globals* game_globals);
static void game_globals_state_start_update(s_game_globals* game_globals);

s_game_globals_states g_game_globals_states {
	.enter = {
		nullptr,
		nullptr,
		game_globals_state_start_update,
		game_globals_state_exit_enter,
	},
	.update = {
		nullptr,
		game_globals_state_launch_update,
		nullptr,
		nullptr,
	},
};

constexpr HMODULE* module_handles[] = {
	&libmcc::halo1::hModule,
	&libmcc::halo2::hModule,
	&libmcc::halo3::hModule,
	&libmcc::halo4::hModule,
	&libmcc::groundhog::hModule,
	&libmcc::halo3odst::hModule,
	&libmcc::haloreach::hModule,
};

static void game_globals_state_launch_update(s_game_globals* game_globals) {
	int state;
	HMODULE hModule;
	s_game_options* options;
	e_module module;

	if (mcc_manager()->is_winstore()) {
		auto globals = reinterpret_cast<mccwinstore::s_game_globals*>(game_globals);
		state = *globals->state();
		hModule = globals->current_module_handle;
		options = &globals->options;
		module = globals->current_module;
	} else {
		auto globals = reinterpret_cast<mcc::s_game_globals*>(game_globals);
		state = *globals->state();
		hModule = globals->current_module_handle;
		options = &globals->options;
		module = globals->current_module;
	}

	if (state == 11) {
		ASSERT(hModule != nullptr, "Game module handle is nullptr!");

		*module_handles[module] = hModule;

		// Set game state
		{
			c_critical_section cs(_critical_section_mcc);
			mcc_manager()->set_game_state(_game_globals_state_launch);
			mcc_manager()->set_game_module(module);
			mcc_manager()->set_is_theater(options->is_theater());
			mcc_manager()->set_game_mode(options->game_mode);
		}

		module_manager()->get(module)->initialize(hModule);
	}

	if (g_game_globals_states_original.update[_game_globals_state_launch])
		g_game_globals_states_original.update[_game_globals_state_launch](game_globals);
}

static void game_globals_state_start_update(s_game_globals* game_globals) {
	// Set game state
	{
		c_critical_section cs(_critical_section_mcc);
		mcc_manager()->set_game_state(_game_globals_state_start);
	}
}

static void game_globals_state_exit_enter(s_game_globals* game_globals) {
	e_module module;

	if (mcc_manager()->is_winstore()) {
		auto globals = (mccwinstore::s_game_globals*)game_globals;
		module = globals->current_module;
	} else {
		auto globals = (mcc::s_game_globals*)game_globals;
		module = globals->current_module;
	}

	// Set game state
	{
		c_critical_section cs(_critical_section_mcc);
		mcc_manager()->set_game_state(_game_globals_state_exit);
		mcc_manager()->set_game_module(k_module_none);
		mcc_manager()->set_game_mode(_game_mode_none);
		mcc_manager()->set_is_theater(false);
	}

	module_manager()->get(module)->shutdown();

	if (g_game_globals_states_original.enter[_game_globals_state_exit])
		g_game_globals_states_original.enter[_game_globals_state_exit](game_globals);
}
