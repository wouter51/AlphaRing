#pragma once

#include "./patch_manager.h"

struct s_hook_manager_private;

class c_hook_manager : public i_manager {
public:
	c_hook_manager();
	~c_hook_manager();

    int initialize() override;
    int shutdown() override;

    void* create(void* pTarget, void* pDetour, void** ppOriginal);
	int enable(void* pTarget);
	int disable(void* pTarget);
	int remove(void* pTarget);

private:
	s_hook_manager_private* m_private;

};

inline c_hook_manager g_hook_manager;

inline c_hook_manager* hook_manager() {
    return &g_hook_manager;
}
