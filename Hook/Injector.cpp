#include <Windows.h>

bool InjectDll(const int pid)
{
	auto path = "C:\\Windows\\SysWOW64\\Hook.dll";
	auto pathSize = strlen(path) + 1;
	auto hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProc == NULL)
	{
		return false;
	}

	auto address = VirtualAllocEx(hProc, NULL, pathSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (address == NULL)
	{
		return false;
	}

	if (WriteProcessMemory(hProc, address, path, pathSize, 0) == 0)
	{
		return false;
	}

	auto hModule = LoadLibrary(L"kernel32");
	if (hModule == 0)
	{
		return false;
	}

	auto addrLoadLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hModule, "LoadLibraryA"));
	if (CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, address, 0, nullptr) == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}
