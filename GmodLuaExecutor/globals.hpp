#pragma once
#include <Windows.h>
#include <iostream>

#include "helpers/logger.hpp"
#include "helpers/library.hpp"

#define get_vfunc(base, index) static_cast<uintptr_t>((*reinterpret_cast<uintptr_t**>(base))[index])
#define get_vmt(address, index, offset, T) *(T**)(((*(uintptr_t**)(address))[index] + offset) + 7 + *(DWORD*)(((*(uintptr_t**)(address))[index] + offset) + 3))

typedef bool(__thiscall* RunStringExFn)(void* state, const char* filename, const char* path, const char* content, bool run, bool show_errors, bool handle_errors, bool sandbox);

inline AsyncLogger* logs = nullptr;
inline bool menuOpen = false;

struct Executor
{
	void* luaInterface = nullptr;
	RunStringExFn runStringEx = nullptr;
	std::queue<std::string> scriptQueue;

	void ExecuteScript(const char* script)
	{
		if (luaInterface && runStringEx)
		{
			bool result = runStringEx(luaInterface, "@", "Lua", script, true, true, true, false);
			logs->println("Executed script with result: %d", result);
		}
		else
		{
			logs->println("Lua interface or RunStringEx not initialized.");
		}
	}

} inline executor;

struct Interfaces
{
	void* luaShared = nullptr;
	void* vclient = nullptr;
	void* clientModeShared = nullptr;

	bool Initialize()
	{
		try {
			auto luaSharedModule = Library("lua_shared.dll");
			auto clientModule = Library("client.dll");

			luaShared = luaSharedModule.create_interface<void*>("LUASHARED", true);
			if (!luaShared) {
				logs->println("Failed to get LUASHARED interface.");
				return false;
			}

			vclient = clientModule.create_interface<void*>("VClient017", false);
			if (!vclient) {
				logs->println("Failed to get VClient017 interface.");
				return false;
			}

			clientModeShared = get_vmt(vclient, 10, 0, void*);
			if (!clientModeShared) {
				logs->println("Failed to get ClientModeShared.");
				return false;
			}


			logs->println("Successfully obtained LUASHARED interface: 0x%p", luaShared);
			logs->println("Successfully obtained VClient017 interface: 0x%p", vclient);
			logs->println("Successfully obtained ClientModeShared: 0x%p", clientModeShared);
			return true;
		}
		catch (const std::runtime_error& e) {
			logs->println("Error loading interfaces: %s", e.what());
			return false;
		}
	}
} inline interfaces;