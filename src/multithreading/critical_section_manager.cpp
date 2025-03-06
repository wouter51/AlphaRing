#include "critical_section_manager.h"

int c_critical_section_manager::initialize() {
	for (int i = 0; i < k_critical_section_count; i++) {
		InitializeCriticalSection(m_critical_sections + i);
	}
    return 0;
}

int c_critical_section_manager::shutdown() {
    for (int i = 0; i < k_critical_section_count; i++) {
        DeleteCriticalSection(m_critical_sections + i);
    }
    return 0;
}

int c_critical_section_manager::enter(e_critical_section section) {
	EnterCriticalSection(m_critical_sections + section);
    return 0;
}

int c_critical_section_manager::try_enter(e_critical_section section) {
    return TryEnterCriticalSection(m_critical_sections + section);
}

int c_critical_section_manager::leave(e_critical_section section) {
	LeaveCriticalSection(m_critical_sections + section);
    return 0;
}
