#include <Windows.h>
#include "Hook.hpp"

static decltype(&CreateWindowExW) originalCreateWindowExW;
static decltype(&CreateWindowExW) hookedCreateWindowExW;

static decltype(&ShowWindow) originalShowWindow;
decltype(&ShowWindow) hookedShowWindow;

static HANDLE lineProcessHandle;
static HANDLE lineThreadHandle;
static HWND hWndLine;

void HookLine()
{
	originalCreateWindowExW = CreateWindowExW;
	hookedCreateWindowExW = [](
		DWORD dwExStyle,
		LPCWSTR lpClassName,
		LPCWSTR lpWindowName,
		DWORD dwStyle,
		int X,
		int Y,
		int nWidth,
		int nHeight,
		HWND hWndParent,
		HMENU hMenu,
		HINSTANCE hInstance,
		LPVOID lpParam) -> HWND
	{
		HWND result;

		if (dwExStyle == (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP))
		{
			// Hide splash window
			nWidth = 0;
			nHeight = 0;
		}

		result = originalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		if (dwStyle == (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP))
		{
			nWidth = 0;
			nHeight = 0;
			hWndLine = result;
		}

		return result;
	};

	originalShowWindow = ShowWindow;
	hookedShowWindow = [](
		HWND hWnd,
		int nCmdShow) -> BOOL
	{
		if (hWnd == hWndLine && nCmdShow == SW_SHOWNORMAL)
		{
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);

			// unhook
			DetourFunction(false, reinterpret_cast<PVOID*>(&originalCreateWindowExW), hookedCreateWindowExW);
			DetourFunction(false, reinterpret_cast<PVOID*>(&originalShowWindow), hookedShowWindow);

			return TRUE;
		}
		else
		{
			return originalShowWindow(hWnd, nCmdShow);
		}

		return originalShowWindow(hWnd, nCmdShow);
	};

	DetourFunction(true, reinterpret_cast<PVOID*>(&originalCreateWindowExW), hookedCreateWindowExW);
	DetourFunction(true, reinterpret_cast<PVOID*>(&originalShowWindow), hookedShowWindow);
}
