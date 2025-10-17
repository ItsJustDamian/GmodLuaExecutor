#pragma once
#include <Windows.h>
#include <iostream>

#include "../globals.hpp"
#include "../helpers/simplehook.hpp"

typedef void* (__thiscall* CreateLuaInterfaceFn)(void*, int, bool);

namespace LuaHook
{
	inline VmtHook64* hook = nullptr, *interfaceHook = nullptr;
	inline CreateLuaInterfaceFn originalCreateLuaInterface = nullptr;

	void* __fastcall hkCreateLuaInterface(void* _this, int type, bool renew)
	{
		auto realInterface = originalCreateLuaInterface(_this, type, renew);
		if (type != 0) return realInterface;

		executor.luaInterface = realInterface;
		logs->println("New lua interface was created, moving to globals: 0x%p", executor.luaInterface);

		executor.runStringEx = reinterpret_cast<RunStringExFn>(get_vfunc(executor.luaInterface, 111));
		if (executor.runStringEx)
			logs->println("Successfully obtained RunStringEx at index 111: 0x%p", executor.runStringEx);
		else
			logs->println("Failed to obtain RunStringEx.");

		return realInterface;
	}

	bool Initialize()
	{
		hook = new VmtHook64(interfaces.luaShared);
		hook->HookMethod(&hkCreateLuaInterface, 4);
		originalCreateLuaInterface = reinterpret_cast<CreateLuaInterfaceFn>(hook->GetOriginal(4));

		if(originalCreateLuaInterface)
			logs->println("Successfully hooked CreateLuaInterface at index 4.");
		else
			logs->println("Failed to hook CreateLuaInterface.");

		return originalCreateLuaInterface;
	}

	bool Cleanup()
	{
		if (hook)
		{
			hook->UnhookMethod(4);
			hook->Restore();
			delete hook;
			hook = nullptr;
			logs->println("Unhooked CreateLuaInterface and cleaned up.");
			return true;
		}

		logs->println("No hook to clean up.");
		return true;

	}
}