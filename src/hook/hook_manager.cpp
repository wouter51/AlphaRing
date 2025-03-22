#include "hook_manager.h"

#include "../multithreading/critical_section_manager.h"

#include "./hde/hde64.h"

struct s_hook_info {
	s_hook_info(void* pTarget, void* pDetour, void* pOriginal);

	bool enabled;
	void* target;

	void* detour; // saved for debugging
	void* original;
};

struct s_hook_manager_private {
	std::unordered_map<void*, s_hook_info> hooks;
};

#pragma pack(push, 1)

struct s_call_rel {
    const uint8_t opcode = 0xE8;
    int32_t offset = 0x0;
};

struct s_call_abs {
    const uint8_t opcode1 = 0xFF;
    const uint8_t opcode2 = 0x15;
    const uint32_t dummy = 0x2;
    const uint8_t dummy1 = 0xEB;
    const uint8_t dummy2 = 0x08;
    uint64_t address = 0x0;
};

struct s_jmp_rel_byte {
	const uint8_t opcode = 0xE9;
	int8_t offset = 0x0;
};

struct s_jmp_rel {
    const uint8_t opcode = 0xE9;
    int32_t offset = 0x0;
};

struct s_jmp_abs {
    const uint8_t opcode1 = 0xFF;
    const uint8_t  opcode2 = 0x25;
    const uint32_t dummy = 0x0;
    uint64_t address = 0x0;
};

#pragma pack(pop)

enum e_jmp_type {
    jmp_type_rel_byte,
	jmp_type_rel,
	jmp_type_abs,
};

s_hook_info::s_hook_info(void* pTarget, void* pDetour, void* pOriginal)
    : target(pTarget), detour(pDetour), original(pOriginal), enabled(false) {
}

c_hook_manager::c_hook_manager() : m_private(new s_hook_manager_private()) {
}

c_hook_manager::~c_hook_manager() {
	delete m_private;
}

int c_hook_manager::initialize() {
    return 0;
}

int c_hook_manager::shutdown() {
    return 0;
}

void* c_hook_manager::create(void* pTarget, void* pDetour, void** ppOriginal) {
    if (pTarget == nullptr || pDetour == nullptr || pTarget == pDetour) {
        return nullptr;
    }

    if (!is_executable_address(pTarget) || !is_executable_address(pDetour)) {
        return nullptr;
    }

	c_critical_section cs(_critical_section_hook);

	// check if the target is already hooked
	auto it = m_private->hooks.find(pTarget);

    if (it != m_private->hooks.end()) {
        return pTarget;
    }

	// calculate the offset
	uint64_t target = reinterpret_cast<uint64_t>(pTarget);
	uint64_t detour = reinterpret_cast<uint64_t>(pDetour);

	bool below = detour < target;
	uint64_t offset = below ? target - detour : detour - target;

    e_jmp_type jmp_type = jmp_type_abs;
	size_t jmp_size = sizeof(s_jmp_abs);

	if (offset < 0x7F) {
		jmp_type = jmp_type_rel_byte;
		jmp_size = sizeof(s_jmp_rel_byte);
	} else if (offset < 0x7FFFFFFF) {
		jmp_type = jmp_type_rel;
		jmp_size = sizeof(s_jmp_rel);
	}

	// analyze the target patch size
	hde64s hs;
	size_t size = 0;

	char opcode[32];
	auto original = std::vector<char>();

    do {
        auto ptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pTarget) + size);

		// disassemble the opcode
        hde64_disasm(ptr, &hs);

		if (hs.flags & F_ERROR) {
			return nullptr;
		}

		switch (hs.opcode) {
		case 0xC3: { // RET
			// not enough space for the JMP
			if (size < jmp_size)
				return nullptr;
			break;
		}
		case 0xE8: { // call
			// convert to absolute call
			s_call_abs call;
			call.address = reinterpret_cast<uintptr_t>(ptr) + hs.len + (int)hs.imm.imm32;

			auto tmp = reinterpret_cast<char*>(&call);

			original.insert(original.end(), tmp, tmp + sizeof(call));

			break;
		}
		default: {
			ASSERT(sizeof(opcode) >= hs.len, "opcode buffer is too small!");

			memcpy(opcode, ptr, hs.len);

			original.insert(original.end(), opcode, opcode + hs.len);
			break;
		}
		}

        size += hs.len;

        if (size >= jmp_size) {
            break;
        }

    } while (true);

	// create patch
	auto patch = std::vector<char>(size);

	// fill with NOP
	memset(patch.data(), 0x90, size);

	// fill with JMP
	switch (jmp_type) {
	case jmp_type_rel_byte: {
		s_jmp_rel_byte jmp;
		jmp.offset = static_cast<int8_t>(offset);
		if (below)
			jmp.offset = -jmp.offset;
		jmp.offset -= (int)sizeof(s_jmp_rel_byte);
		memcpy(patch.data(), &jmp, jmp_size);
		break;
	}
	case jmp_type_rel: {
		s_jmp_rel jmp;
		jmp.offset = static_cast<int32_t>(offset);
		if (below)
			jmp.offset = -jmp.offset;
		jmp.offset -= (int)sizeof(s_jmp_rel);
		memcpy(patch.data(), &jmp, jmp_size);
		break;
	}
	case jmp_type_abs: {
		s_jmp_abs jmp;
		jmp.address = detour;
		memcpy(patch.data(), &jmp, jmp_size);
		break;
	}
	}

	// create patch
	if (patch_manager()->create(pTarget, patch.data(), patch.size()) == nullptr) {
		return nullptr;
	}

	// create original
	{
		s_jmp_abs jmp;

		jmp.address = target + size;

		auto ptr = reinterpret_cast<char*>(&jmp);

		original.insert(original.end(), ptr, ptr + sizeof(jmp));
	}

	auto pOriginal = VirtualAlloc(nullptr, original.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (pOriginal == nullptr) {
		return nullptr;
	}

	memcpy(pOriginal, original.data(), original.size());

	// emplace hook info
	auto result = m_private->hooks.emplace(
        std::piecewise_construct, 
        std::forward_as_tuple(pTarget), 
        std::forward_as_tuple(pTarget, pDetour, pOriginal));

    if (!result.second) {
		VirtualFree(pOriginal, 0, MEM_RELEASE);

        return nullptr;
    }

	if (ppOriginal)
		*ppOriginal = pOriginal;

    return pTarget;
}

int c_hook_manager::enable(void* pTarget) {
    c_critical_section cs(_critical_section_hook);

	auto it = m_private->hooks.find(pTarget);

	if (it == m_private->hooks.end()) {
		return -1;
	}

    if (it->second.enabled) {
		return 0;
    }

	patch_manager()->enable(it->second.target);

	it->second.enabled = true;

    return 0;
}

int c_hook_manager::disable(void* pTarget) {
    c_critical_section cs(_critical_section_hook);

	auto it = m_private->hooks.find(pTarget);

	if (it == m_private->hooks.end()) {
		return -1;
	}

	if (!it->second.enabled) {
		return 0;
	}

	patch_manager()->disable(it->second.target);

	it->second.enabled = false;

    return 0;
}

int c_hook_manager::remove(void* pTarget) {
    c_critical_section cs(_critical_section_hook);

	auto it = m_private->hooks.find(pTarget);

	if (it == m_private->hooks.end()) {
		return 0;
	}

	if (it->second.enabled) {
		patch_manager()->remove(it->second.target);
	}

	VirtualFree(it->second.original, 0, MEM_RELEASE);

	m_private->hooks.erase(it);

	return 0;
}
