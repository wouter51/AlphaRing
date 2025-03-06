#pragma once

#include "../common.h"

struct s_vftable_manager_private;

class c_vftable_manager : public i_manager {
public:
	c_vftable_manager();
	~c_vftable_manager();

	int initialize() override;
	int shutdown() override;

	void* create(void* pTarget, const void** pPatch, size_t size, void** ppOriginal);
	int enable(void* pTarget);
	int disable(void* pTarget);
	int remove(void* pTarget);

private:
	s_vftable_manager_private* m_private;

};

inline c_vftable_manager g_vftable_manager;

inline c_vftable_manager* vftable_manager() {
	return &g_vftable_manager;
}
