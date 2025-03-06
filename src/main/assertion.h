#pragma once

#include "logging.h"
#include "win32.h"

void _check(const char* Expression, const char* File, unsigned int Line, const char* Message);
void _assertion(const char* Expression, const char* File, unsigned int Line, const char* Message);

// ASSERT macro
#define ASSERT(exp) \
	if (!(exp)) { \
		_assertion(#exp, __FILE__, __LINE__, nullptr); \
	}

#define ASSERT(exp, msg) \
	if (!(exp)) { \
		_assertion(#exp, __FILE__, __LINE__, msg); \
	}

#define ASSERTF(exp, fms, ...) \
	if (!(exp)) { \
		_assertion(#exp, __FILE__, __LINE__, fmt::format(fms, __VA_ARGS__).c_str()); \
	}

#define ASSERT_HR(hresult) \
	if (!SUCCEEDED(hresult)) { \
		_assertion("SUCCEEDED(hresult)", __FILE__, __LINE__, HR2STR(hresult)); \
	}

#define ASSERT_HR(hresult, msg) \
	if (!SUCCEEDED(hresult)) { \
		_assertion("SUCCEEDED(hresult)", __FILE__, __LINE__, fmt::format("{} {}", msg, HR2STR(hresult)).c_str()); \
	}

#define ASSERT_HD(handle) \
	if (!(handle != NULL)) { \
		_assertion("handle != NULL", __FILE__, __LINE__, get_last_error_string().c_str()); \
	}

#define ASSERT_HD(handle, msg) \
	if (!(handle != NULL)) { \
		_assertion("handle != NULL", __FILE__, __LINE__, fmt::format("{} {}", msg, get_last_error_string()).c_str()); \
	}

// CHECK macro

#define CHECK(exp) \
	((!(exp)) ? (_check(#exp, __FILE__, __LINE__, nullptr), 1) : 0)

#define CHECK(exp, msg) \
	((!(exp)) ? (_check(#exp, __FILE__, __LINE__, msg), 1) : 0)

#define CHECKF(exp, fms, ...) \
	((!(exp)) ? (_check(#exp, __FILE__, __LINE__, fmt::format(fms, __VA_ARGS__).c_str()), 1) : 0)

#define CHECK_HR(hresult) \
	((!SUCCEEDED(hresult)) ? (_check("SUCCEEDED(hresult)", __FILE__, __LINE__, HR2STR(hresult)), 1) : 0)

#define CHECK_HR(hresult, msg) \
	((!SUCCEEDED(hresult)) ? (_check("SUCCEEDED(hresult)", __FILE__, __LINE__, fmt::format("{} {}", msg, HR2STR(hresult)).c_str()), 1) : 0)

#define CHECK_HD(handle) \
	((!(handle != NULL)) ? (_check("handle != NULL", __FILE__, __LINE__, get_last_error_string().c_str()), 1) : 0)

#define CHECK_HD(handle, msg) \
	((!(handle != NULL)) ? (_check("handle != NULL", __FILE__, __LINE__, fmt::format("{} {}", msg, get_last_error_string()).c_str()), 1) : 0)

#define CHECK_HD_VALID(handle) \
	((!(handle != INVALID_HANDLE_VALUE)) ? (_check("handle != INVALID_HANDLE_VALUE", __FILE__, __LINE__, get_last_error_string().c_str()), 1) : 0)

#define CHECK_HD_VALID(handle, msg) \
	((!(handle != INVALID_HANDLE_VALUE)) ? (_check("handle != INVALID_HANDLE_VALUE", __FILE__, __LINE__, fmt::format("{} {}", msg, get_last_error_string()).c_str()), 1) : 0)
