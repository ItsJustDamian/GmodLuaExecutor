#pragma once
#include <Windows.h>
#include <iostream>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")

class Library
{
private:
	const char* library_name;
	HMODULE library_module;

public:
	Library(const char* library_name)
	{
		this->library_name = library_name;
		this->library_module = GetModuleHandleA(library_name);

		if (!this->library_module)
			throw std::runtime_error("Failed to get module handle");
	}

	template<typename t>
	t create_interface(const char* interface_name, bool brute_furce = false)
	{
		using fn = void* (__cdecl*)(const char*, int*);
		fn create_interface_fn = reinterpret_cast<fn>(GetProcAddress(library_module, "CreateInterface"));

		if (!create_interface_fn)
			return nullptr;

		t _interface = nullptr;

		if (brute_furce)
		{
			char* buffer = new char[128];
			for (int i = 0; i < 100; i++)
			{
				sprintf_s(buffer, 128, "%s%03d", interface_name, i);

				_interface = reinterpret_cast<t>(create_interface_fn(buffer, nullptr));

				if (_interface)
					break;
			}
			delete buffer;
		}
		else
		{
			_interface = reinterpret_cast<t>(create_interface_fn(interface_name, nullptr));
		}

		return _interface;
	}

	template<typename t>
	t find_pattern(const char* pattern, const char* mask)
	{
		MODULEINFO module_info;
		GetModuleInformation(GetCurrentProcess(), library_module, &module_info, sizeof(MODULEINFO));

		uintptr_t module_start = reinterpret_cast<uintptr_t>(module_info.lpBaseOfDll);
		uintptr_t module_end = module_start + module_info.SizeOfImage;

		uintptr_t pattern_length = strlen(mask);

		for (uintptr_t i = 0; i < module_info.SizeOfImage - pattern_length; i++)
		{
			bool found = true;
			for (uintptr_t j = 0; j < pattern_length; j++)
			{
				if (mask[j] != '?' && pattern[j] != *reinterpret_cast<char*>(module_start + i + j))
				{
					found = false;
					break;
				}
			}

			if (found)
				return (t)(module_start + i);
		}

		return NULL;
	}

	template<typename t = HMODULE>
	t get_address()
	{
		return reinterpret_cast<t>(library_module);
	}

	template<typename t>
	t get_export(const char* export_name)
	{
		return reinterpret_cast<t>(GetProcAddress(library_module, export_name));
	}
};