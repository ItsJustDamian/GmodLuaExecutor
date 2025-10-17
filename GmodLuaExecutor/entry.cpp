#include <Windows.h>
#include <iostream>

#include "cheat/cheat.hpp"

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hModule);
			HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cheat::InitialThread, hModule, 0, NULL);
			
			if (!threadHandle)
			{
				MessageBoxA(NULL, "Failed to create thread!", "Error", MB_OK | MB_ICONERROR);
				return FALSE; // Failed to create thread.
			}

			CloseHandle(threadHandle); // Close the thread handle as we don't need it anymore.
			break;
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}