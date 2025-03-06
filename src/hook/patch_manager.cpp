#include "patch_manager.h"

struct s_patch_info {
	s_patch_info(void* target, std::vector<char>&& original, std::vector<char>&& patch)
		: target(target), original(std::move(original)), patch(std::move(patch)), enabled(false) {
	}

	bool enabled;

	void* target;
	std::vector<char> original;
	std::vector<char> patch;
};

struct s_patch_manager_private {
	std::unordered_map<void*, s_patch_info> patches;
};

c_patch_manager::c_patch_manager() : m_private(new s_patch_manager_private()) {
}

c_patch_manager::~c_patch_manager() {
	delete m_private;
}

int c_patch_manager::initialize() {
	return 0;
}

int c_patch_manager::shutdown() {
	return 0;
}

void* c_patch_manager::create(void* pTarget, const char* pPatch) {
	if (pTarget == nullptr || pPatch == nullptr) {
		return nullptr;
	}

	int length = strlen(pPatch);

	if (length == 0) {
		return nullptr;
	}

	std::vector<char> patch;

	patch.reserve(length);

	char count = 0;
	char byte = 0;

	for (int i = 0; i <= length; ++i) {
		char ch = pPatch[i];

		if (ch == ' ' || ch == '\0') {
			if (count) {
				patch.emplace_back(byte);
				byte = 0;
				count = 0;
			}
		} else if (ch >= '0' && ch <= '9') {
			if (count >= 2) {
				return nullptr;
			}

			byte = byte << 4 | static_cast<char>(ch - '0');

			++count;
		} else {
			if (ch >= 'a' && ch <= 'f') {
				ch -= 0x20;
			}

			if (ch >= 'A' && ch <= 'F') {
				if (count >= 2) {
					return nullptr;
				}

				byte = byte << 4 | static_cast<char>(ch - 'A' + 10);

				++count;
			}
		}
	}

	if (patch.size() == 0) {
		return nullptr;
	}

	return create(pTarget, patch.data(), patch.size());
}

void* c_patch_manager::create(void* pTarget, const void* pPatch, size_t size) {
	if (pTarget == nullptr || pPatch == nullptr || size == 0) {
		return nullptr;
	}

	c_critical_section cs(_critical_section_patch);

	auto it = m_private->patches.find(pTarget);

	if (it != m_private->patches.end()) {
		return pTarget;
	}

	auto patch = std::vector<char>(size);
	auto original = std::vector<char>(size);

	// copy patch bytes
	memcpy(patch.data(), pPatch, size);

	// backup original bytes
	memcpy(original.data(), pTarget, size);

	auto result = m_private->patches.emplace(
		std::piecewise_construct, 
		std::forward_as_tuple(pTarget), 
		std::forward_as_tuple(pTarget, std::move(original), std::move(patch)));

	if (!result.second) {
		return nullptr;
	}

	return pTarget;
}

int c_patch_manager::enable(void* pTarget) {
	c_critical_section cs(_critical_section_patch);

	auto it = m_private->patches.find(pTarget);

	if (it == m_private->patches.end()) {
		return -1;
	}

	if (it->second.enabled) {
		return 0;
	}

	write_memory(it->second.target, it->second.patch.data(), it->second.patch.size());

	it->second.enabled = true;

	return 0;
}

int c_patch_manager::disable(void* pTarget) {
	c_critical_section cs(_critical_section_patch);

	auto it = m_private->patches.find(pTarget);

	if (it == m_private->patches.end()) {
		return -1;
	}

	if (!it->second.enabled) {
		return 0;
	}

	write_memory(it->second.target, it->second.original.data(), it->second.original.size());

	it->second.enabled = false;

	return 0;
}

int c_patch_manager::remove(void* pTarget) {
	c_critical_section cs(_critical_section_patch);

	auto it = m_private->patches.find(pTarget);

	if (it == m_private->patches.end()) {
		return 0;
	}

	if (it->second.enabled) {
		disable(pTarget);
	}

	m_private->patches.erase(it);

	return 0;
}
