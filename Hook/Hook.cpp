#include <Windows.h>
#include <iostream>
#include "Injector.hpp"
#include "DetoursHelper.h"

// windows-internals-book:"Chapter 5" 
typedef enum _PS_CREATE_STATE {
	PsCreateInitialState,
	PsCreateFailOnFileOpen,
	PsCreateFailOnSectionCreate,
	PsCreateFailExeFormat,
	PsCreateFailMachineMismatch,
	PsCreateFailExeName, // Debugger specified
	PsCreateSuccess,
	PsCreateMaximumStates
} PS_CREATE_STATE;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _PS_CREATE_INFO {
	SIZE_T Size;
	PS_CREATE_STATE State;
	union {
		// PsCreateInitialState
		struct {
			union {
				ULONG InitFlags;
				struct {
					UCHAR WriteOutputOnExit : 1;
					UCHAR DetectManifest : 1;
					UCHAR IFEOSkipDebugger : 1;
					UCHAR IFEODoNotPropagateKeyState : 1;
					UCHAR SpareBits1 : 4;
					UCHAR SpareBits2 : 8;
					USHORT ProhibitedImageCharacteristics : 16;
				};
			};
			ACCESS_MASK AdditionalFileAccess;
		} InitState;

		// PsCreateFailOnSectionCreate
		struct {
			HANDLE FileHandle;
		} FailSection;

		// PsCreateFailExeFormat
		struct {
			USHORT DllCharacteristics;
		} ExeFormat;

		// PsCreateFailExeName
		struct {
			HANDLE IFEOKey;
		} ExeName;

		// PsCreateSuccess
		struct {
			union {
				ULONG OutputFlags;
				struct {
					UCHAR ProtectedProcess : 1;
					UCHAR AddressSpaceOverride : 1;
					UCHAR DevOverrideEnabled : 1; // from Image File Execution Options
					UCHAR ManifestDetected : 1;
					UCHAR ProtectedProcessLight : 1;
					UCHAR SpareBits1 : 3;
					UCHAR SpareBits2 : 8;
					USHORT SpareBits3 : 16;
				};
			};
			HANDLE FileHandle;
			HANDLE SectionHandle;
			ULONGLONG UserProcessParametersNative;
			ULONG UserProcessParametersWow64;
			ULONG CurrentParameterFlags;
			ULONGLONG PebAddressNative;
			ULONG PebAddressWow64;
			ULONGLONG ManifestAddress;
			ULONG ManifestSize;
		} SuccessState;
	};
} PS_CREATE_INFO, * PPS_CREATE_INFO;

typedef struct _PS_ATTRIBUTE {
	ULONGLONG Attribute;				/// PROC_THREAD_ATTRIBUTE_XXX | PROC_THREAD_ATTRIBUTE_XXX modifiers, see ProcThreadAttributeValue macro and Windows Internals 6 (372)
	SIZE_T Size;						/// Size of Value or *ValuePtr
	union {
		ULONG_PTR Value;				/// Reserve 8 bytes for data (such as a Handle or a data pointer)
		PVOID ValuePtr;					/// data pointer
	};
	PSIZE_T ReturnLength;				/// Either 0 or specifies size of data returned to caller via "ValuePtr"
} PS_ATTRIBUTE, * PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST {
	SIZE_T TotalLength;					/// sizeof(PS_ATTRIBUTE_LIST)
	PS_ATTRIBUTE Attributes[2];			/// Depends on how many attribute entries should be supplied to NtCreateUserProcess
} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;

typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	PVOID Handle;
} CURDIR, * PCURDIR;

typedef struct _STRING
{
	WORD Length;
	WORD MaximumLength;
	CHAR* Buffer;
} STRING, * PSTRING;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	WORD Flags;
	WORD Length;
	ULONG TimeStamp;
	STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	PVOID StandardInput;
	PVOID StandardOutput;
	PVOID StandardError;
	CURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;
	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopInfo;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
	ULONG EnvironmentSize;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	PVOID RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

NTSTATUS NTAPI NtCreateUserProcess(
	__out PHANDLE ProcessHandle,
	__out PHANDLE ThreadHandle,
	__in ACCESS_MASK ProcessDesiredAccess,
	__in ACCESS_MASK ThreadDesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ProcessObjectAttributes,
	__in_opt POBJECT_ATTRIBUTES ThreadObjectAttributes,
	__in ULONG ProcessFlags,
	__in ULONG ThreadFlags,
	__in_opt PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
	__in PPS_CREATE_INFO  CreateInfo,
	__in_opt PPS_ATTRIBUTE_LIST  AttributeList
);

NTSYSAPI NTSTATUS NTAPI NtResumeThread(
	IN HANDLE ThreadHandle,
	OUT PULONG SuspendCount OPTIONAL
);

static decltype(&NtCreateUserProcess) originalNtCreateUserProcess;
static decltype(&NtCreateUserProcess) hookedNtCreateUserProcess;

static decltype(&CreateWindowExW) originalCreateWindowExW;
static decltype(&CreateWindowExW) hookedCreateWindowExW;

static decltype(&NtResumeThread) originalNtResumeThread;
decltype(&NtResumeThread) hookedNtResumeThread;

static decltype(&ShowWindow) originalShowWindow;
decltype(&ShowWindow) hookedShowWindow;

static HANDLE lineProcessHandle;
static HANDLE lineThreadHandle;
static HWND hWndLine;

void HookLineLauncher()
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
		if (dwExStyle == WS_EX_TOOLWINDOW && dwStyle == WS_POPUP)
		{
			// Hide splash window
			nWidth = 0;
			nHeight = 0;
		}

		if (dwStyle == (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP))
		{
			// Hide LINE window
			nWidth = 0;
			nHeight = 0;
		}

		return originalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	};

	auto hModule = GetModuleHandle(L"ntdll");
	if (hModule == nullptr)
	{
		return;
	}
	originalNtCreateUserProcess = reinterpret_cast<decltype(&NtCreateUserProcess)>(GetProcAddress(hModule, "NtCreateUserProcess"));
	//std::cout << std::hex << originalNtCreateUserProcess << std::endl;

	hookedNtCreateUserProcess = [](
		PHANDLE ProcessHandle,
		PHANDLE ThreadHandle,
		ACCESS_MASK ProcessDesiredAccess,
		ACCESS_MASK ThreadDesiredAccess,
		POBJECT_ATTRIBUTES ProcessObjectAttributes,
		POBJECT_ATTRIBUTES ThreadObjectAttributes,
		ULONG ProcessFlags,
		ULONG ThreadFlags,
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
		PPS_CREATE_INFO  CreateInfo,
		PPS_ATTRIBUTE_LIST  AttributeList) -> NTSTATUS
	{
		ThreadFlags = 0x1; // THREAD_CREATE_FLAGS_CREATE_SUSPENDED
		std::cout << "hookedNtCreateUserProcess called" << std::endl;
		auto ret = originalNtCreateUserProcess(ProcessHandle, ThreadHandle, ProcessDesiredAccess, ThreadDesiredAccess, ProcessObjectAttributes, ThreadObjectAttributes, ProcessFlags, ThreadFlags, ProcessParameters, CreateInfo, AttributeList);
		lineProcessHandle = *ProcessHandle;
		lineThreadHandle = *ThreadHandle;
		std::cout << std::hex << "lineThreadHandle:" << lineThreadHandle << std::endl;
		return ret;
	};

	originalNtResumeThread = reinterpret_cast<decltype(&NtResumeThread)>(GetProcAddress(hModule, "NtResumeThread"));
	hookedNtResumeThread = [](
		HANDLE ThreadHandle,
		PULONG SuspendCount) -> NTSTATUS
	{
		std::cout << "hookedNtResumeThread called" << std::endl;

		if (ThreadHandle == lineThreadHandle)
		{
			std::cout << "LINE NtResumeThread" << std::hex << "ThreadHandle:" << ThreadHandle << std::endl;
			std::cout << std::dec << "line.exe pid=" << GetProcessId(lineProcessHandle) << std::endl;
			auto result = InjectDll(GetProcessId(lineProcessHandle));
			std::cout << "inject result" << result << std::endl;
		}
		
		return originalNtResumeThread(ThreadHandle, SuspendCount);
	};

	DetourFunction(true, reinterpret_cast<PVOID*>(&originalCreateWindowExW), hookedCreateWindowExW);
	DetourFunction(true, reinterpret_cast<PVOID*>(&originalNtCreateUserProcess), hookedNtCreateUserProcess);
	DetourFunction(true, reinterpret_cast<PVOID*>(&originalNtResumeThread), hookedNtResumeThread);
}

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
			std::cout << "Hide splash window" << std::endl;
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
