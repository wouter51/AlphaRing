#include "event_queue_manager.h"

int c_event_queue_manager::initialize() {
	m_header = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);

	if (!m_header) {
		return -1;
	}

	InitializeSListHead(m_header);

	return 0;
}

int c_event_queue_manager::shutdown() {
	clear();

	_aligned_free(m_header);

	return 0;
}

int c_event_queue_manager::push(std::unique_ptr<s_event_base>&& event) {
	auto item = (s_event_queue_item*)_aligned_malloc(sizeof(s_event_queue_item), MEMORY_ALLOCATION_ALIGNMENT);

	if (!item) {
		return -1;
	}

	memset(item, 0, sizeof(s_event_queue_item));

	item->event = std::move(event);

	InterlockedPushEntrySList(m_header, &item->entry);

	return 0;
}

void c_event_queue_manager::process() {
	auto item = InterlockedFlushSList(m_header);

	while (item) {
		auto event = CONTAINING_RECORD(item, s_event_queue_item, entry);
		event->event->execute();
		event->event->free();
		item = item->Next;
		_aligned_free(event);
	}
}

void c_event_queue_manager::clear() {
	auto item = InterlockedFlushSList(m_header);

	while (item) {
		auto event = CONTAINING_RECORD(item, s_event_queue_item, entry);
		event->event->free();
		item = item->Next;
		_aligned_free(event);
	}
}
