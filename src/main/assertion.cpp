#include "assertion.h"

#include <Windows.h>

void _check(const char* Expression, const char* File, unsigned int Line, const char* Message) {
	auto msg = fmt::format("Check failed: {} at {}:{}", Expression, File, Line);

	LOG_ERROR(msg);

	if (Message != nullptr) {
		LOG_ERROR(Message);
		msg = fmt::format("{}\n{}", msg, Message);
	}
}

void _assertion(const char* Expression, const char* File, unsigned int Line, const char* Message) {
	auto msg = fmt::format("Assertion failed: {} at {}:{}", Expression, File, Line);

	LOG_CRITICAL(msg);

	if (Message != nullptr) {
		LOG_CRITICAL(Message);
		msg = fmt::format("{}\n{}", msg, Message);
	}

	auto result = MessageBoxA(nullptr, msg.c_str(), 
		"Assertion failed", MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_DEFBUTTON2);

	if (result == IDRETRY) {
		__debugbreak(); // Trigger a breakpoint
	} else if (result == IDIGNORE) {
		return; // Do nothing
	} else {
		abort(); // Terminate the program and generate a core dump
	}
}
