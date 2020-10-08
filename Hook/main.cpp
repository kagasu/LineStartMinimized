#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include "Status.h"
#include "VerifierDll.hpp"
#include "Hook.hpp"

std::wstring GetCurrentProcessName()
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	for (BOOL bok = Process32First(processesSnapshot, &processInfo); bok; bok = Process32Next(processesSnapshot, &processInfo))
	{
		if (GetCurrentProcessId() == processInfo.th32ProcessID)
		{
			CloseHandle(processesSnapshot);
			return processInfo.szExeFile;
		}

	}
	CloseHandle(processesSnapshot);
	return std::wstring();
}

BOOL WINAPI DllMain(IN HINSTANCE hInstDLL, IN SIZE_T nReason, IN PVOID pvReserved)
{
	UNREFERENCED_PARAMETER(hInstDLL);

	switch(nReason)
	{
	case VERIFIERDLL_DLL_PROCESS_VERIFIER:
		return VERIFIERDLL_DllMainProcessVerifier(pvReserved);
	case DLL_PROCESS_ATTACH:
	{
		/*
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE* pFile = nullptr;
		freopen_s(&pFile, "CON", "r", stdin);
		freopen_s(&pFile, "CON", "w", stdout);
		freopen_s(&pFile, "CON", "w", stderr);
		*/

		auto processName = GetCurrentProcessName();
		if (processName.compare(L"LineLauncher.exe") == 0)
		{
			HookLineLauncher();
		}
		else
		{
			HookLine();
		}
		return TRUE;
	}
	default:
		return TRUE;
	}
}
