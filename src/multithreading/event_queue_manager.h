#pragma once

#include "../main/win32.h"
#include "../main/manager.h"

class s_event_base {
public:
	virtual void free() {};
	virtual void execute() = 0;
};

struct s_event_queue_item {
	SLIST_ENTRY entry;
	std::unique_ptr<s_event_base> event;
};

class c_event_queue_manager : public i_manager {
public:
	int initialize() override;
	int shutdown() override;

	int push(std::unique_ptr<s_event_base>&& event);
	void process();
	void clear();

private:
	PSLIST_HEADER m_header;
};

inline c_event_queue_manager g_event_queue_manager;
inline c_event_queue_manager* event_queue_manager() {
	return &g_event_queue_manager;
}
