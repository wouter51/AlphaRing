#include "./module_manager.h"

#include "../halo3/render/view/render_player_view.h"

using namespace libmcc;

#define Entry(returnType, pDetour, ...) \
    static returnType (__fastcall* pDetour##_original)(...); \
    returnType __fastcall pDetour(__VA_ARGS__)

#ifdef _DEBUG	

Entry(void, dsSTATE_MGR__SetState, libmcc::halo1::dsSTATE_MGR* This, int id, const libmcc::halo1::dsDATA* data) {
	dsSTATE_MGR__SetState_original(This, id, data);

	if (This->RegisterState("IsMP", true) == id) {
		if (*reinterpret_cast<const bool*>(data->type->GetPtr(data)) == true) {
			__debugbreak();
		}
	}
}

#endif

Entry(halo1::dsDATA*, dsSTATE_MGR__GetState, libmcc::halo1::dsSTATE_MGR* This, int id) {
	auto result = dsSTATE_MGR__GetState_original(This, id);

	if (This->RegisterState("IsMP", true) == id) {
		*(bool*)result->type->GetPtr(result) = false;
	}

	return result;
}

int s_module::initialize(HMODULE hModule) {
	for (auto& patch : patches) {
		if (!patch.enabled)
			continue;

		patch.target = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(hModule) + patch.offset);

		if (!patch_manager()->create(patch.target, patch.patch)) {
			continue;
		}

		patch_manager()->enable(patch.target);
	}

	for (auto& hook : hooks) {
		if (!hook.enabled)
			continue;

		hook.target = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(hModule) + hook.offset);

		if (!hook_manager()->create(hook.target, hook.detour, hook.original)) {
			continue;
		}

		hook_manager()->enable(hook.target);
	}

	for (auto& vftable : vftables) {
		auto target = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(hModule) + vftable.offset);

		if (!vftable_manager()->create(target, vftable.src, vftable.size, vftable.original)) {
			continue;
		}

		vftable_manager()->enable(target);
	}

	return 0;
}

int s_module::shutdown() {
	for (auto& patch : patches) {
		if (!patch.enabled)
			continue;

		patch_manager()->remove(patch.target);
	}
	for (auto& hook : hooks) {
		if (!hook.enabled)
			continue;
		
		hook_manager()->remove(hook.target);
	}
	return 0;
}

int ::c_module_manager::initialize() {
	for (int i = 0; i < libmcc::k_game_count; ++i) {
		auto module = m_modules + i;

		switch (i) {
		case _module_halo1: {
			module->patches.emplace_back(0x67492, " EB 18");
			module->patches.emplace_back(0x427978, " EB");
#ifdef _DEBUG	
			module->hooks.emplace_back(libmcc::halo1::s_function_offset_table::dsSTATE_MGR__SetState.first, dsSTATE_MGR__SetState, (void**)&dsSTATE_MGR__SetState_original);
#endif
			module->hooks.emplace_back(libmcc::halo1::s_function_offset_table::dsSTATE_MGR__GetState.first, dsSTATE_MGR__GetState, (void**)&dsSTATE_MGR__GetState_original);
			break;
		}
		case _module_halo2: {
			module->patches.emplace_back(libmcc::halo2::s_function_offset_table::game_options_verify.first, " 31 C0 B0 01 C3 90");
			module->patches.emplace_back(0x8940CA, " 04");
			module->patches.emplace_back(0x894127, " 04");
			module->patches.emplace_back(0x5153E, " 83 F8 01 74 04");
			break;
		}
		case _module_halo3: {
			module->patches.emplace_back(libmcc::halo3::s_function_offset_table::c_network_session__can_accept_any_join_request.first, "31 C0 C3 90");
			break;
		}
		case _module_halo4: {
			module->patches.emplace_back(0x56671C, " 31 C0 C3 90");
			module->patches.emplace_back(0x4E311B, " EB");
			module->patches.emplace_back(0x3F5D7, " 90 90 90 90 90 90");
			break;
		}
		case _module_groundhog: {
			module->patches.emplace_back(0x567DF0, " 31 C0 C3 90");
			module->patches.emplace_back(0x4E50DB, " EB");
			break;
		}
		case _module_halo3odst: {
			module->patches.emplace_back(libmcc::halo3odst::s_function_offset_table::c_network_session__can_accept_any_join_request.first, "31 C0 C3 90");
			break;
		}
		case _module_haloreach: {
			module->patches.emplace_back(libmcc::haloreach::s_function_offset_table::c_network_session__can_accept_any_join_request.first, " 31 C0 C3 90");
			module->patches.emplace_back(0x394A2C, " EB");
			break;
		}
		default:
			break;
		}
	}
	return 0;
}

int ::c_module_manager::shutdown() {
	return 0;
}
