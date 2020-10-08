#pragma once
#include <detours/detours.h>
#pragma comment(lib, "detours.lib")

BOOL DetourFunction(BOOL hookEnabled, PVOID* ppPointer, PVOID pDetour)
{
	if (DetourTransactionBegin() == NO_ERROR) {
		if (DetourUpdateThread(GetCurrentThread()) == NO_ERROR) {
			if ((hookEnabled ? DetourAttach : DetourDetach)(ppPointer, pDetour) == NO_ERROR) {
				if (DetourTransactionCommit() == NO_ERROR) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
