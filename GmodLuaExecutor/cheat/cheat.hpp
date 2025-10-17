#pragma once

#include <iostream>
#include <string>

#include "../globals.hpp"
#include "overlayhook.hpp"
#include "luahook.hpp"
#include "createmovehook.hpp"

namespace cheat
{
	void InitialThread(HMODULE hMod)
	{
		logs = new AsyncLogger("GmodLuaExecutor.log");
		logs->println("Cheat InitialThread started.");

		// Your initialization code here
		if (!interfaces.Initialize())
		{
			logs->println("Failed to initialize interfaces.");
			delete logs;
			FreeLibraryAndExitThread(hMod, 0);
			return;
		}

		if (!OverlayHook::Initialize())
		{
			logs->println("Failed to initialize OverlayHook.");
			delete logs;
			FreeLibraryAndExitThread(hMod, 0);
			return;
		}

		if (!LuaHook::Initialize())
		{
			logs->println("Failed to initialize LuaHook.");
			OverlayHook::Cleanup();
			delete logs;
			FreeLibraryAndExitThread(hMod, 0);
			return;
		}

		if (!CreateMoveHook::Initialize())
		{
			logs->println("Failed to initialize CreateMoveHook.");
			LuaHook::Cleanup();
			OverlayHook::Cleanup();
			delete logs;
			FreeLibraryAndExitThread(hMod, 0);
			return;
		}

		logs->println("Cheat InitialThread finished.");

		while (!GetAsyncKeyState(VK_END))
			Sleep(100);

		logs->println("Cheat unloading...");

		// Your cleanup code here
		CreateMoveHook::Cleanup();
		LuaHook::Cleanup();
		OverlayHook::Cleanup();

		delete logs;
		FreeLibraryAndExitThread(hMod, 0);
	}
}