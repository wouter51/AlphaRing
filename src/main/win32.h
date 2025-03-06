#pragma once

#include <vector>
#include <string>
#include <memory>
#include <Windows.h>

union s_file_version {
    uint64_t n;
    uint16_t w[4];
    uint32_t d[2];

    std::string to_string();
};

const char* HR2STR(HRESULT hr);

bool is_executable_address(void* address);

std::wstring get_document_folder_path();

std::unique_ptr<std::vector<char>> read_file(const wchar_t* path);

bool write_file(const wchar_t* path, const char* data, size_t size);

bool check_directory(const wchar_t* path, bool create_if_not_exist);

HRESULT write_memory(void* dst, void* src, size_t size);

HMODULE load_system_dll(const char* name);

s_file_version get_file_version(HMODULE hModule);

std::string get_last_error_string();

std::string get_module_name(HMODULE hModule);

std::wstring get_module_path(HMODULE hModule);

std::vector<void*> get_export_directory_functions(HMODULE hModule);

void* get_image_directory(HMODULE hModule, int IMAGE_DIRECTORY_ENTRY_);
