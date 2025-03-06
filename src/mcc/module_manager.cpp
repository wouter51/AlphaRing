#include "./module_manager.h"

#include "../halo3/render/view/render_player_view.h"

using namespace libmcc;

#define Entry(returnType, pDetour, ...) \
    static returnType (__fastcall* pDetour##_original)(...); \
    returnType __fastcall pDetour(__VA_ARGS__)

Entry(void, halo3_process_game_engine_globals_messages) {
	halo3_process_game_engine_globals_messages_original();

	event_queue_manager()->process();
}

int idIsMP = -1;

Entry(int, RegisterState, void* a1, const char* id, bool isToggleEventOnChange) {
	auto result = RegisterState_original(a1, id, isToggleEventOnChange);

	if (id && strcmp("IsMP", id) == 0) {
		idIsMP = result;
	}

	return result;
}

Entry(halo1::dsDATA*, GetState, void* a1, int id) {
	auto result = GetState_original(a1, id);

	if (id == idIsMP) {
		result->storage = false;
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
	auto halo1 = get(_module_halo1);

	halo1->patches.emplace_back(0x67492, " EB 18");
	halo1->patches.emplace_back(0x427978, " EB");
	halo1->hooks.emplace_back(0x18B150, GetState, (void**)&GetState_original);
	halo1->hooks.emplace_back(0x18ABA0, RegisterState, (void**)&RegisterState_original);


	auto halo2 = get(_module_halo2);
	halo2->patches.emplace_back(0x6A6C30, " 31 C0 B0 01 C3 90");
	halo2->patches.emplace_back(0x893FDA, " 04");
	halo2->patches.emplace_back(0x894037, " 04");
	halo2->patches.emplace_back(0x5153E, " 83 F8 01 74 04");

	auto halo3 = get(_module_halo3);
	halo3->patches.emplace_back(0x11DF8, " 31 C0 C3 90");
	halo3->patches.emplace_back(0x8AD160, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");
	halo3->patches.emplace_back(0x8AD174, " 00 00 00 00 00 00 00 3F 00 00 80 3F 00 00 80 3F 01");
	halo3->patches.emplace_back(0x8AD1B0, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");
	halo3->hooks.emplace_back(0xB468, halo3_process_game_engine_globals_messages, (void**) & halo3_process_game_engine_globals_messages_original);
	halo3->vftables.emplace_back(0x7FE970, (const void**) &::halo3::g_player_view_vftable, (void**)&::halo3::g_player_view_vftable_original, sizeof(::halo3::c_view_vftable<libmcc::halo3::c_view>));

	auto halo4 = get(_module_halo4);
	halo4->patches.emplace_back(0x566794, " 31 C0 C3 90");
	halo4->patches.emplace_back(0x4E301B, " EB");
	halo4->patches.emplace_back(0x3F5D7, " 90 90 90 90 90 90");
	halo4->patches.emplace_back(0xE84E40, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");
	halo4->patches.emplace_back(0xE84E54, " 00 00 00 00 00 00 00 3F 00 00 80 3F 00 00 80 3F 01");
	halo4->patches.emplace_back(0xE84E90, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");

	auto groundhog = get(_module_groundhog);
	groundhog->patches.emplace_back(0x567E28, " 31 C0 C3 90");
	groundhog->patches.emplace_back(0x4E4FAB, " EB");
	groundhog->patches.emplace_back(0xE76560, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");
	groundhog->patches.emplace_back(0xE76574, " 00 00 00 00 00 00 00 3F 00 00 80 3F 00 00 80 3F 01");
	groundhog->patches.emplace_back(0xE765B0, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");

	auto halo3odst = get(_module_halo3odst);
	halo3odst->patches.emplace_back(0x1258C, " 31 C0 C3 90");
	halo3odst->patches.emplace_back(0x8F1FC0, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");
	halo3odst->patches.emplace_back(0x8F1FD4, " 00 00 00 00 00 00 00 3F 00 00 80 3F 00 00 80 3F 01");
	halo3odst->patches.emplace_back(0x8F2010, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");

	auto haloreach = get(_module_haloreach);
	haloreach->patches.emplace_back(0x3971C4, " 31 C0 C3 90");
	haloreach->patches.emplace_back(0x39489C, " EB");
	haloreach->patches.emplace_back(0xB43D10, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");
	haloreach->patches.emplace_back(0xB43D24, " 00 00 00 00 00 00 00 3F 00 00 80 3F 00 00 80 3F 01");
	haloreach->patches.emplace_back(0xB43D60, " 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 3F 01");

	return 0;
}

int ::c_module_manager::shutdown() {
	return 0;
}
