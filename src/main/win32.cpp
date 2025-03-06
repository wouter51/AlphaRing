#include "win32.h"

#include <winnt.h>
#include <winternl.h>

#include "../common.h"

std::string s_file_version::to_string() {
	return fmt::format("{}.{}.{}.{}", w[1], w[0], w[3], w[2]);
}

bool is_executable_address(void* address) {
	MEMORY_BASIC_INFORMATION mi;

	VirtualQuery(address, &mi, sizeof(mi));

	return (mi.State == MEM_COMMIT && (mi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)));
}

std::wstring get_document_folder_path() {
	HKEY key;
	DWORD size;
	std::vector<WCHAR> buffer;
	std::vector<WCHAR> formatted;
	LSTATUS status;
	std::wstring result;

	status = RegOpenKeyExW(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders",
		0,
		KEY_READ,
		&key);

	if (status != ERROR_SUCCESS) {
		return result;
	}

	status = RegQueryValueExW(
		key,
		L"Personal",
		0,
		0,
		0,
		&size);

	if (status != ERROR_SUCCESS) {
		RegCloseKey(key);
		return result;
	}

	buffer.resize(size);

	status = RegQueryValueExW(
		key,
		L"Personal",
		0,
		0,
		(LPBYTE)buffer.data(),
		&size);

	if (status != ERROR_SUCCESS) {
		RegCloseKey(key);
		return result;
	}

	size = ExpandEnvironmentStringsW(buffer.data(), nullptr, 0);

	if (size == 0) {
		RegCloseKey(key);
		return result;
	}

	formatted.resize(size);

	size = ExpandEnvironmentStringsW(buffer.data(), formatted.data(), size);

	if (size == 0) {
		RegCloseKey(key);
		return result;
	}

	result = formatted.data();

	return result;
}

std::unique_ptr<std::vector<char>> read_file(const wchar_t* path) {
	DWORD read;
	LARGE_INTEGER size;
	std::unique_ptr<std::vector<char>> pFile;

	auto hFile = CreateFileW(
		path,					// file to open
		GENERIC_READ,			// open for reading
		FILE_SHARE_READ,		// share for reading
		NULL,					// default security
		OPEN_EXISTING,			// open existing
		FILE_ATTRIBUTE_NORMAL,	// normal file
		NULL);					// no template file

	if (hFile == INVALID_HANDLE_VALUE) {
		return nullptr;
	}

	auto result = GetFileSizeEx(hFile, &size);

	if (!result) {
		CloseHandle(hFile);
		return nullptr;
	}

	pFile = std::make_unique<std::vector<char>>();

	pFile->resize(size.QuadPart);

	result = ReadFile(hFile, (LPVOID)pFile->data(), size.QuadPart, &read, NULL);

	if (!result) {
		CloseHandle(hFile);
		return nullptr;
	}

	CloseHandle(hFile);
	return pFile;
}

bool write_file(const wchar_t* path, const char* data, size_t size) {
	DWORD written;
	auto hFile = CreateFileW(
		path,					// file to open
		GENERIC_WRITE,			// open for writing
		FILE_SHARE_READ,		// share for reading
		NULL,					// default security
		CREATE_ALWAYS,			// overwrite existing
		FILE_ATTRIBUTE_NORMAL,	// normal file
		NULL);					// no template file

	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	auto result = WriteFile(hFile, data, size, &written, NULL);

	if (!result) {
		CloseHandle(hFile);
		return false;
	}

	CloseHandle(hFile);

	return true;
}

bool check_directory(const wchar_t* path, bool create_if_not_exist) {
	DWORD fileAttributes = GetFileAttributesW(path);

	if (fileAttributes == INVALID_FILE_ATTRIBUTES && create_if_not_exist) {
		auto result = CreateDirectoryW(path, NULL);

		ASSERT_HD(result, "Failed to create directory!");

		return true;
	} else if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		return true;
	} else {
		return false;
	}
}

HRESULT write_memory(void* dst, void* src, size_t size) {
	DWORD old_protect;

	auto result = VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &old_protect);

	ASSERT_HD(result, "Failed to open memory!");

	memcpy(dst, src, size);

	result = VirtualProtect(dst, size, old_protect, &old_protect);

	ASSERT_HD(result, "Failed to close memory!");

	return S_OK;
}

HMODULE load_system_dll(const char* name) {
	char path[MAX_PATH];

	auto len = GetSystemDirectoryA(path, MAX_PATH);

	ASSERT(len != 0, "Failed to get System Directory!");

	auto full_path = fmt::format("{}\\{}", path, name);

	return LoadLibraryA(full_path.c_str());
}

s_file_version get_file_version(HMODULE hModule) {
	struct VERHEAD {
		WORD wTotLen;
		WORD wValLen;
		WORD wType;         /* always 0 */
		WCHAR szKey[(sizeof("VS_VERSION_INFO") + 3) & ~03];
		VS_FIXEDFILEINFO vsf;
	} *pVerHead;
	
	HRSRC hVerRes = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), VS_FILE_INFO);

	ASSERT_HD(hVerRes, "Failed to find Version Resource!");

	pVerHead = reinterpret_cast<VERHEAD*>(LoadResource(hModule, hVerRes));

	ASSERT_HD(pVerHead, "Failed to load Version Resource!");

	ASSERT(pVerHead->vsf.dwSignature == 0xfeef04bd, "Version is invalid!");

	return {.d = {pVerHead->vsf.dwFileVersionMS, pVerHead->vsf.dwFileVersionLS}};
}

std::string get_last_error_string() {
	const int buffer_size = 1024;
	char buffer[buffer_size];

	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer, buffer_size, nullptr);

	return std::string(buffer);
}

std::string get_module_name(HMODULE hModule) {
	auto pExport = (PIMAGE_EXPORT_DIRECTORY)get_image_directory(hModule, IMAGE_DIRECTORY_ENTRY_EXPORT);

	ASSERT(pExport != nullptr, "Failed to get Export Directory!");

	auto pName = reinterpret_cast<char*>(reinterpret_cast<uint64_t>(hModule) + pExport->Name);

	return std::string(pName);
}

std::wstring get_module_path(HMODULE hModule) {
	wchar_t path[MAX_PATH];

	auto len = GetModuleFileNameW(hModule, path, MAX_PATH);

	ASSERT_HD(len, "Failed to get module path");

	while (len != 1) {
		if (path[len - 1] == '\\' || path[len - 1] == '/')
			break;
		--len;
	}

	path[len] = '\0';
	
	return std::wstring(path);
}

std::vector<void*> get_export_directory_functions(HMODULE hModule) {
	auto pExport = (PIMAGE_EXPORT_DIRECTORY)get_image_directory(hModule, IMAGE_DIRECTORY_ENTRY_EXPORT);

	ASSERT(pExport != nullptr, "Failed to get Export Directory!");

	std::vector<void*> result(pExport->NumberOfFunctions);

	auto module = reinterpret_cast<uint64_t>(hModule);
	auto functions = reinterpret_cast<DWORD*>(module + pExport->AddressOfFunctions);

	for (int i = 0; i < pExport->NumberOfFunctions; i++) {
		result[i] = reinterpret_cast<void*>(module + functions[i]);
	}

	return result;
}

void* get_image_directory(HMODULE hModule, int IMAGE_DIRECTORY_ENTRY_) {
	auto module = reinterpret_cast<uint64_t>(hModule);

	auto pnth = reinterpret_cast<PIMAGE_NT_HEADERS>(module + reinterpret_cast<PIMAGE_DOS_HEADER>(hModule)->e_lfanew);

	return reinterpret_cast<void*>(module + pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_].VirtualAddress);
}

#define HR_AND_STR(HR) case HR: return #HR

const char* HR2STR(HRESULT hr) {
	switch (hr) {
		HR_AND_STR(S_OK);
		HR_AND_STR(S_FALSE);
		HR_AND_STR(E_FAIL);
		HR_AND_STR(E_INVALIDARG);
		HR_AND_STR(E_NOINTERFACE);
		HR_AND_STR(E_NOTIMPL);
		HR_AND_STR(E_OUTOFMEMORY);
		HR_AND_STR(E_POINTER);
		HR_AND_STR(E_UNEXPECTED);
		// D3D11
		HR_AND_STR(D3D11_ERROR_FILE_NOT_FOUND);
		HR_AND_STR(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS);
		HR_AND_STR(D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS);
		HR_AND_STR(D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD);
		HR_AND_STR(DXGI_ERROR_INVALID_CALL);
		HR_AND_STR(DXGI_ERROR_WAS_STILL_DRAWING);
		HR_AND_STR(DXGI_ERROR_ACCESS_DENIED);
		HR_AND_STR(DXGI_ERROR_ACCESS_LOST);
		HR_AND_STR(DXGI_ERROR_ALREADY_EXISTS);
		HR_AND_STR(DXGI_ERROR_MORE_DATA);

		default: return "UNKNOWN";
	}
}
