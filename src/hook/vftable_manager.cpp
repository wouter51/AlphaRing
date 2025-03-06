#include "vftable_manager.h"

#include "patch_manager.h"

struct s_vftable_info {
	void* target;
	
	std::vector<void*> original;
	std::vector<void*> patch;
};

struct s_vftable_manager_private {
	std::unordered_map<void*, s_vftable_info> vftables;
};

c_vftable_manager::c_vftable_manager() : m_private(new s_vftable_manager_private()) {

}

c_vftable_manager::~c_vftable_manager() {
	delete m_private;
}

int c_vftable_manager::initialize() {
	return 0;
}

int c_vftable_manager::shutdown() {
	return 0;
}

void* c_vftable_manager::create(void* pTarget, const void** pPatch, size_t size, void** ppOriginal) {
	s_vftable_info info;
	size_t count = size / sizeof(void*);

	if (pTarget == nullptr || pPatch == nullptr || size == 0) {
		return nullptr;
	}

	info.target = pTarget;
	info.original.resize(count);
	info.patch.resize(count);

	memcpy(info.original.data(), pTarget, size);
	memcpy(info.patch.data(), info.original.data(), size);

	for (size_t i = 0; i < count; ++i) {
		if (pPatch[i] == nullptr) {
			continue;
		}

		info.patch[i] = const_cast<void*>(pPatch[i]);
	}

	// create patch
	if (patch_manager()->create(pTarget, info.patch.data(), size) == nullptr) {
		return nullptr;
	}

	// empalce vftable info
	{
		c_critical_section cs(_critical_section_vftable);

		auto result = m_private->vftables.emplace(pTarget, info);

		if (!result.second) {
			return nullptr;
		}
	}

	// copy original
	if (ppOriginal) {
		memcpy(ppOriginal, info.original.data(), size);
	}

	return pTarget;
}

int c_vftable_manager::enable(void* pTarget) {
	c_critical_section cs(_critical_section_vftable);

	auto it = m_private->vftables.find(pTarget);

	if (it == m_private->vftables.end()) {
		return -1;
	}

	patch_manager()->enable(pTarget);

	return 0;
}

int c_vftable_manager::disable(void* pTarget) {
	c_critical_section cs(_critical_section_vftable);

	auto it = m_private->vftables.find(pTarget);

	if (it == m_private->vftables.end()) {
		return -1;
	}

	if (it->second.original == it->second.patch) {
		return 0;
	}

	patch_manager()->disable(pTarget);

	return 0;
}

int c_vftable_manager::remove(void* pTarget) {
	c_critical_section cs(_critical_section_vftable);

	auto it = m_private->vftables.find(pTarget);

	if (it == m_private->vftables.end()) {
		return -1;
	}

	patch_manager()->remove(pTarget);

	m_private->vftables.erase(it);

	return 0;
}
