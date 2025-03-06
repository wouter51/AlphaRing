#pragma once

#include "../common.h"

struct s_patch_manager_private;

class c_patch_manager : public i_manager {
public:
	c_patch_manager();
	~c_patch_manager();

	int initialize() override;
	int shutdown() override;

	void* create(void* pTarget, const char* pPatch);
	void* create(void* pTarget, const void* pPatch, size_t size);
	int enable(void* pTarget);
	int disable(void* pTarget);
	int remove(void* pTarget);

private:
	s_patch_manager_private* m_private;

};

inline c_patch_manager g_patch_manager;

inline c_patch_manager* patch_manager() {
	return &g_patch_manager;
}
