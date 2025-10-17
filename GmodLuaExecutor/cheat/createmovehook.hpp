#pragma once
#include <Windows.h>
#include <iostream>

#include "../globals.hpp"
#include "../helpers/memory.hpp"
#include "../helpers/simplehook.hpp"

typedef bool(__thiscall* CreateMoveFn)(void*, float, void*);

// This hook/func is being used for lua thread safety, so we dont crash when sending lua.
namespace CreateMoveHook
{
	inline VmtHook64* hook = nullptr;
	inline CreateMoveFn originalCreateMove = nullptr;

	bool __fastcall hookedCreateMove(void* ecx, float input_sample_frametime, void* user_cmd)
	{
		auto original = originalCreateMove(ecx, input_sample_frametime, user_cmd);

		if (!executor.scriptQueue.empty())
		{
			auto script = executor.scriptQueue.front();
			executor.scriptQueue.pop();
			executor.ExecuteScript(script.c_str());
			logs->println("Executed queued script.");
		}

		return original;
	}

	bool Initialize()
	{
		hook = new VmtHook64(interfaces.clientModeShared);
		hook->HookMethod(&hookedCreateMove, 21);
		originalCreateMove = reinterpret_cast<CreateMoveFn>(hook->GetOriginal(21));

		if (originalCreateMove)
		{
			logs->println("Successfully hooked CreateMove at index 21.");
			return true;
		}
		else
		{
			logs->println("Failed to hook CreateMove.");
			return false;
		}
	}

	bool Cleanup()
	{
		if (hook)
		{
			hook->UnhookMethod(21);
			hook->Restore();
			delete hook;
			hook = nullptr;
			logs->println("Unhooked CreateMove and cleaned up.");
			return true;
		}
		logs->println("No hook to clean up.");
		return true;
	}
}