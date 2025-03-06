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

constexpr const char* module_names[] = {
	"halo1.dll",
	"halo2.dll",
	"halo3.dll",
	"halo4.dll",
	"groundhog.dll",
	"halo3odst.dll",
	"haloreach.dll",
};

static void game_globals_state_launch_update(s_game_globals* game_globals) {
	auto globlas = (mcc::s_game_globals*)game_globals;

	if (*globlas->state() == 11) {
		event_queue_manager()->clear();


		auto module = globlas->data.current_module;

		auto hModule = GetModuleHandleA(module_names[globlas->data.current_module]);

		ASSERT(hModule != nullptr, "Game module handle is nullptr!");

		auto options = &globlas->data.options;

		// Set game state
		{
			c_critical_section cs(_critical_section_mcc);
			mcc_manager()->set_game_state(_game_globals_state_launch);
			mcc_manager()->set_game_module(module);
			mcc_manager()->set_is_theater(options->is_theater());
			mcc_manager()->set_game_mode(options->game_mode);
		}

		module_manager()->get(module)->initialize(hModule);

		switch (module) {
		case _module_halo3: {
			halo3::Initialize(hModule);
			break;
		}
		default:
			break;
		}
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
	// Set game state
	{
		c_critical_section cs(_critical_section_mcc);
		mcc_manager()->set_game_state(_game_globals_state_exit);
		mcc_manager()->set_game_module(k_module_none);
		mcc_manager()->set_game_mode(_game_mode_none);
		mcc_manager()->set_is_theater(false);
	}

	auto globlas = (mcc::s_game_globals*)game_globals;

	auto module = globlas->data.current_module;

	module_manager()->get(module)->shutdown();

	if (g_game_globals_states_original.enter[_game_globals_state_exit])
		g_game_globals_states_original.enter[_game_globals_state_exit](game_globals);
}
