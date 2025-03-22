#pragma once

#include "../main/win32.h"
#include "../main/manager.h"

enum e_critical_section {
	_critical_section_player,
	_critical_section_imgui,
	_critical_section_hook,
	_critical_section_patch,
	_critical_section_vftable,
	_critical_section_mcc,
	_critical_section_dollycam,
	_critical_section_resize,

	k_critical_section_count,
	k_critical_section_none = -1,
};

class c_critical_section_manager : public i_manager {
public:
	c_critical_section_manager() { initialize(); }
	~c_critical_section_manager() { shutdown(); }

	int initialize() override;
	int shutdown() override;

	int enter(e_critical_section section);
	int try_enter(e_critical_section section);
	int leave(e_critical_section section);

private:
	CRITICAL_SECTION m_critical_sections[k_critical_section_count];
};

inline c_critical_section_manager g_critical_section_manager;

inline c_critical_section_manager* critical_section_manager() {
	return &g_critical_section_manager;
}

class c_critical_section {
public:
	explicit c_critical_section(e_critical_section cs) : m_cs(cs) {
		if (m_cs == k_critical_section_none) return;
		critical_section_manager()->enter(m_cs);
	}

	c_critical_section(c_critical_section&& other) = delete;

	~c_critical_section() {
		if (m_cs == k_critical_section_none) return;
		critical_section_manager()->leave(m_cs);
	}
private:
	e_critical_section m_cs;
};
