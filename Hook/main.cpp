#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <shlobj.h>
#include <winternl.h>
#include "Hook.hpp"

#pragma comment(lib, "ntdll.lib")

#pragma region PROXY_DLL
#define FUNCTION_COUNT 204

#ifdef __EDG__
#define DLLEXPORT
#else
#define DLLEXPORT(i, name) __pragma(comment(linker, "/EXPORT:" name "=" __FUNCDNAME__ ",@" #i))
#endif

extern "C" uintptr_t functions[FUNCTION_COUNT] = { 0 };
std::string functionNames[] =
{
	"DbgHelpCreateUserDump",
	"DbgHelpCreateUserDumpW",
	"EnumDirTree",
	"EnumDirTreeW",
	"EnumerateLoadedModules",
	"EnumerateLoadedModules64",
	"EnumerateLoadedModulesEx",
	"EnumerateLoadedModulesExW",
	"EnumerateLoadedModulesW64",
	"ExtensionApiVersion",
	"FindDebugInfoFile",
	"FindDebugInfoFileEx",
	"FindDebugInfoFileExW",
	"FindExecutableImage",
	"FindExecutableImageEx",
	"FindExecutableImageExW",
	"FindFileInPath",
	"FindFileInSearchPath",
	"GetTimestampForLoadedLibrary",
	"ImageDirectoryEntryToData",
	"ImageDirectoryEntryToDataEx",
	"ImageNtHeader",
	"ImageRvaToSection",
	"ImageRvaToVa",
	"ImagehlpApiVersion",
	"ImagehlpApiVersionEx",
	"MakeSureDirectoryPathExists",
	"MapDebugInformation",
	"MiniDumpReadDumpStream",
	"MiniDumpWriteDump",
	"SearchTreeForFile",
	"SearchTreeForFileW",
	"StackWalk",
	"StackWalk64",
	"SymAddSourceStream",
	"SymAddSourceStreamA",
	"SymAddSourceStreamW",
	"SymAddSymbol",
	"SymAddSymbolW",
	"SymCleanup",
	"SymDeleteSymbol",
	"SymDeleteSymbolW",
	"SymEnumLines",
	"SymEnumLinesW",
	"SymEnumProcesses",
	"SymEnumSourceFileTokens",
	"SymEnumSourceFiles",
	"SymEnumSourceFilesW",
	"SymEnumSourceLines",
	"SymEnumSourceLinesW",
	"SymEnumSym",
	"SymEnumSymbols",
	"SymEnumSymbolsForAddr",
	"SymEnumSymbolsForAddrW",
	"SymEnumSymbolsW",
	"SymEnumTypes",
	"SymEnumTypesByName",
	"SymEnumTypesByNameW",
	"SymEnumTypesW",
	"SymEnumerateModules",
	"SymEnumerateModules64",
	"SymEnumerateModulesW64",
	"SymEnumerateSymbols",
	"SymEnumerateSymbols64",
	"SymEnumerateSymbolsW",
	"SymEnumerateSymbolsW64",
	"SymFindDebugInfoFile",
	"SymFindDebugInfoFileW",
	"SymFindExecutableImage",
	"SymFindExecutableImageW",
	"SymFindFileInPath",
	"SymFindFileInPathW",
	"SymFromAddr",
	"SymFromAddrW",
	"SymFromIndex",
	"SymFromIndexW",
	"SymFromName",
	"SymFromNameW",
	"SymFromToken",
	"SymFromTokenW",
	"SymFunctionTableAccess",
	"SymFunctionTableAccess64",
	"SymGetFileLineOffsets64",
	"SymGetHomeDirectory",
	"SymGetHomeDirectoryW",
	"SymGetLineFromAddr",
	"SymGetLineFromAddr64",
	"SymGetLineFromAddrW64",
	"SymGetLineFromName",
	"SymGetLineFromName64",
	"SymGetLineFromNameW64",
	"SymGetLineNext",
	"SymGetLineNext64",
	"SymGetLineNextW64",
	"SymGetLinePrev",
	"SymGetLinePrev64",
	"SymGetLinePrevW64",
	"SymGetModuleBase",
	"SymGetModuleBase64",
	"SymGetModuleInfo",
	"SymGetModuleInfo64",
	"SymGetModuleInfoW",
	"SymGetModuleInfoW64",
	"SymGetOmapBlockBase",
	"SymGetOmaps",
	"SymGetOptions",
	"SymGetScope",
	"SymGetScopeW",
	"SymGetSearchPath",
	"SymGetSearchPathW",
	"SymGetSourceFile",
	"SymGetSourceFileFromToken",
	"SymGetSourceFileFromTokenW",
	"SymGetSourceFileToken",
	"SymGetSourceFileTokenW",
	"SymGetSourceFileW",
	"SymGetSourceVarFromToken",
	"SymGetSourceVarFromTokenW",
	"SymGetSymFromAddr",
	"SymGetSymFromAddr64",
	"SymGetSymFromName",
	"SymGetSymFromName64",
	"SymGetSymNext",
	"SymGetSymNext64",
	"SymGetSymPrev",
	"SymGetSymPrev64",
	"SymGetSymbolFile",
	"SymGetSymbolFileW",
	"SymGetTypeFromName",
	"SymGetTypeFromNameW",
	"SymGetTypeInfo",
	"SymGetTypeInfoEx",
	"SymInitialize",
	"SymInitializeW",
	"SymLoadModule",
	"SymLoadModule64",
	"SymLoadModuleEx",
	"SymLoadModuleExW",
	"SymMatchFileName",
	"SymMatchFileNameW",
	"SymMatchString",
	"SymMatchStringA",
	"SymMatchStringW",
	"SymNext",
	"SymNextW",
	"SymPrev",
	"SymPrevW",
	"SymRefreshModuleList",
	"SymRegisterCallback",
	"SymRegisterCallback64",
	"SymRegisterCallbackW64",
	"SymRegisterFunctionEntryCallback",
	"SymRegisterFunctionEntryCallback64",
	"SymSearch",
	"SymSearchW",
	"SymSetContext",
	"SymSetHomeDirectory",
	"SymSetHomeDirectoryW",
	"SymSetOptions",
	"SymSetParentWindow",
	"SymSetScopeFromAddr",
	"SymSetScopeFromIndex",
	"SymSetSearchPath",
	"SymSetSearchPathW",
	"SymSrvDeltaName",
	"SymSrvDeltaNameW",
	"SymSrvGetFileIndexInfo",
	"SymSrvGetFileIndexInfoW",
	"SymSrvGetFileIndexString",
	"SymSrvGetFileIndexStringW",
	"SymSrvGetFileIndexes",
	"SymSrvGetFileIndexesW",
	"SymSrvGetSupplement",
	"SymSrvGetSupplementW",
	"SymSrvIsStore",
	"SymSrvIsStoreW",
	"SymSrvStoreFile",
	"SymSrvStoreFileW",
	"SymSrvStoreSupplement",
	"SymSrvStoreSupplementW",
	"SymUnDName",
	"SymUnDName64",
	"SymUnloadModule",
	"SymUnloadModule64",
	"UnDecorateSymbolName",
	"UnDecorateSymbolNameW",
	"UnmapDebugInformation",
	"WinDbgExtensionDllInit",
	"block",
	"chksym",
	"dbghelp",
	"dh",
	"fptr",
	"homedir",
	"itoldyouso",
	"lmi",
	"lminfo",
	"omap",
	"srcfiles",
	"stack_force_ebp",
	"stackdbg",
	"sym",
	"symsrv",
	"vc7fpo"
};

// DbgHelpCreateUserDump
__declspec(naked) void func_562d143f7b684532ab8f6736dfa7d304()
{
	DLLEXPORT(1, "DbgHelpCreateUserDump");
	__asm { jmp functions[4 * 0] }
}

// DbgHelpCreateUserDumpW
__declspec(naked) void func_abeffd80855848fa80f01a73f988f88a()
{
	DLLEXPORT(2, "DbgHelpCreateUserDumpW");
	__asm { jmp functions[4 * 1] }
}

// EnumDirTree
__declspec(naked) void func_fda8da14dc634f7a94901c47a6b0cb08()
{
	DLLEXPORT(3, "EnumDirTree");
	__asm { jmp functions[4 * 2] }
}

// EnumDirTreeW
__declspec(naked) void func_93c58c46da754d40940d82f0ad0a9947()
{
	DLLEXPORT(4, "EnumDirTreeW");
	__asm { jmp functions[4 * 3] }
}

// EnumerateLoadedModules
__declspec(naked) void func_8a936e5777a44389ab3fb4d43f7971b1()
{
	DLLEXPORT(5, "EnumerateLoadedModules");
	__asm { jmp functions[4 * 4] }
}

// EnumerateLoadedModules64
__declspec(naked) void func_cdb8c43f69f34393ad6e14cab76b774a()
{
	DLLEXPORT(6, "EnumerateLoadedModules64");
	__asm { jmp functions[4 * 5] }
}

// EnumerateLoadedModulesEx
__declspec(naked) void func_223c10cca5924746b016dc18b6f10942()
{
	DLLEXPORT(7, "EnumerateLoadedModulesEx");
	__asm { jmp functions[4 * 6] }
}

// EnumerateLoadedModulesExW
__declspec(naked) void func_862d69353ac747e2bcd16473e317ce18()
{
	DLLEXPORT(8, "EnumerateLoadedModulesExW");
	__asm { jmp functions[4 * 7] }
}

// EnumerateLoadedModulesW64
__declspec(naked) void func_772511d4f4e74c2084957513e118b713()
{
	DLLEXPORT(9, "EnumerateLoadedModulesW64");
	__asm { jmp functions[4 * 8] }
}

// ExtensionApiVersion
__declspec(naked) void func_e467243ee4a7476ea01a01743295e9ff()
{
	DLLEXPORT(10, "ExtensionApiVersion");
	__asm { jmp functions[4 * 9] }
}

// FindDebugInfoFile
__declspec(naked) void func_13d45aade0314fd1a7f0e72abb14d5d5()
{
	DLLEXPORT(11, "FindDebugInfoFile");
	__asm { jmp functions[4 * 10] }
}

// FindDebugInfoFileEx
__declspec(naked) void func_529ca17e27d8407e819bb840cf654571()
{
	DLLEXPORT(12, "FindDebugInfoFileEx");
	__asm { jmp functions[4 * 11] }
}

// FindDebugInfoFileExW
__declspec(naked) void func_5f4cdadae5874161ba0977dd4c0be951()
{
	DLLEXPORT(13, "FindDebugInfoFileExW");
	__asm { jmp functions[4 * 12] }
}

// FindExecutableImage
__declspec(naked) void func_2798ff4e9fcc4d578cb9e7eeddcff569()
{
	DLLEXPORT(14, "FindExecutableImage");
	__asm { jmp functions[4 * 13] }
}

// FindExecutableImageEx
__declspec(naked) void func_613f936f522443c992ff68cf649fc271()
{
	DLLEXPORT(15, "FindExecutableImageEx");
	__asm { jmp functions[4 * 14] }
}

// FindExecutableImageExW
__declspec(naked) void func_6ac2e56511694d6eb3eed1e596fb73aa()
{
	DLLEXPORT(16, "FindExecutableImageExW");
	__asm { jmp functions[4 * 15] }
}

// FindFileInPath
__declspec(naked) void func_0f46a8f1f66f49c6a501c7e80d716852()
{
	DLLEXPORT(17, "FindFileInPath");
	__asm { jmp functions[4 * 16] }
}

// FindFileInSearchPath
__declspec(naked) void func_6e1ada93d84e4e41b42d25a8feec2045()
{
	DLLEXPORT(18, "FindFileInSearchPath");
	__asm { jmp functions[4 * 17] }
}

// GetTimestampForLoadedLibrary
__declspec(naked) void func_33b5772afc094965a16085d724b2fbb0()
{
	DLLEXPORT(19, "GetTimestampForLoadedLibrary");
	__asm { jmp functions[4 * 18] }
}

// ImageDirectoryEntryToData
__declspec(naked) void func_1231cb42275f4a9aa4a74078c15b52ab()
{
	DLLEXPORT(20, "ImageDirectoryEntryToData");
	__asm { jmp functions[4 * 19] }
}

// ImageDirectoryEntryToDataEx
__declspec(naked) void func_7aaaf5fd88dc42229d26433e8abe1961()
{
	DLLEXPORT(21, "ImageDirectoryEntryToDataEx");
	__asm { jmp functions[4 * 20] }
}

// ImageNtHeader
__declspec(naked) void func_f4f56c314bde43518aa242ebb6e2659b()
{
	DLLEXPORT(22, "ImageNtHeader");
	__asm { jmp functions[4 * 21] }
}

// ImageRvaToSection
__declspec(naked) void func_19ea8141c1ff4ba8ad5807b6736bf1d4()
{
	DLLEXPORT(23, "ImageRvaToSection");
	__asm { jmp functions[4 * 22] }
}

// ImageRvaToVa
__declspec(naked) void func_1b2bb715ef2c4c7da8a562dcf4e4f3a2()
{
	DLLEXPORT(24, "ImageRvaToVa");
	__asm { jmp functions[4 * 23] }
}

// ImagehlpApiVersion
__declspec(naked) void func_8c53b023270841f191329b1a733c367b()
{
	DLLEXPORT(25, "ImagehlpApiVersion");
	__asm { jmp functions[4 * 24] }
}

// ImagehlpApiVersionEx
__declspec(naked) void func_cd44dede11cd4d2ea5c82e5efed2230d()
{
	DLLEXPORT(26, "ImagehlpApiVersionEx");
	__asm { jmp functions[4 * 25] }
}

// MakeSureDirectoryPathExists
__declspec(naked) void func_a8d968f7358d4b5bac04d4494db917d0()
{
	DLLEXPORT(27, "MakeSureDirectoryPathExists");
	__asm { jmp functions[4 * 26] }
}

// MapDebugInformation
__declspec(naked) void func_db6c7cacaee34d9bbbadc0081a57668a()
{
	DLLEXPORT(28, "MapDebugInformation");
	__asm { jmp functions[4 * 27] }
}

// MiniDumpReadDumpStream
__declspec(naked) void func_8ee4f164d8ab4da4b3bffbf879dabe8f()
{
	DLLEXPORT(29, "MiniDumpReadDumpStream");
	__asm { jmp functions[4 * 28] }
}

// MiniDumpWriteDump
__declspec(naked) void func_c7c8135617ea4724b3f83d43dcba0cde()
{
	DLLEXPORT(30, "MiniDumpWriteDump");
	__asm { jmp functions[4 * 29] }
}

// SearchTreeForFile
__declspec(naked) void func_5af38f7b1a9644b1bda19caeecf6ffbc()
{
	DLLEXPORT(31, "SearchTreeForFile");
	__asm { jmp functions[4 * 30] }
}

// SearchTreeForFileW
__declspec(naked) void func_eb82a1914e9d4b86bbb02e2ae40de2ec()
{
	DLLEXPORT(32, "SearchTreeForFileW");
	__asm { jmp functions[4 * 31] }
}

// StackWalk
__declspec(naked) void func_208f3d4704324969a30ad960f241ca67()
{
	DLLEXPORT(33, "StackWalk");
	__asm { jmp functions[4 * 32] }
}

// StackWalk64
__declspec(naked) void func_1e62ec45cb924fd48b99e643b60e7853()
{
	DLLEXPORT(34, "StackWalk64");
	__asm { jmp functions[4 * 33] }
}

// SymAddSourceStream
__declspec(naked) void func_878eb4b66f164df0b6874f0fc1bca102()
{
	DLLEXPORT(35, "SymAddSourceStream");
	__asm { jmp functions[4 * 34] }
}

// SymAddSourceStreamA
__declspec(naked) void func_d0b9d1aadfa74eb2a2460e8574de922d()
{
	DLLEXPORT(36, "SymAddSourceStreamA");
	__asm { jmp functions[4 * 35] }
}

// SymAddSourceStreamW
__declspec(naked) void func_820245f50e38485595d6380c895236b0()
{
	DLLEXPORT(37, "SymAddSourceStreamW");
	__asm { jmp functions[4 * 36] }
}

// SymAddSymbol
__declspec(naked) void func_770ef84ca5c4434fb87a56de7ce4c026()
{
	DLLEXPORT(38, "SymAddSymbol");
	__asm { jmp functions[4 * 37] }
}

// SymAddSymbolW
__declspec(naked) void func_0fb2042079994002b7a74acf14dd7d0c()
{
	DLLEXPORT(39, "SymAddSymbolW");
	__asm { jmp functions[4 * 38] }
}

// SymCleanup
__declspec(naked) void func_44e2dcceae734c49806dfffa38d4afde()
{
	DLLEXPORT(40, "SymCleanup");
	__asm { jmp functions[4 * 39] }
}

// SymDeleteSymbol
__declspec(naked) void func_e3445c4390a64d39a2c6cf60dae5ffec()
{
	DLLEXPORT(41, "SymDeleteSymbol");
	__asm { jmp functions[4 * 40] }
}

// SymDeleteSymbolW
__declspec(naked) void func_8be9f63e96ef45c59d11106ee69f005e()
{
	DLLEXPORT(42, "SymDeleteSymbolW");
	__asm { jmp functions[4 * 41] }
}

// SymEnumLines
__declspec(naked) void func_7f0442107e7842a69a02032c542f6fc7()
{
	DLLEXPORT(43, "SymEnumLines");
	__asm { jmp functions[4 * 42] }
}

// SymEnumLinesW
__declspec(naked) void func_98e807cb3531453aa4539c3256e3e9ef()
{
	DLLEXPORT(44, "SymEnumLinesW");
	__asm { jmp functions[4 * 43] }
}

// SymEnumProcesses
__declspec(naked) void func_16bb44ae4b774dc99f16094bd3529744()
{
	DLLEXPORT(45, "SymEnumProcesses");
	__asm { jmp functions[4 * 44] }
}

// SymEnumSourceFileTokens
__declspec(naked) void func_0b1ec1734c8b4f83959789509ea01d5d()
{
	DLLEXPORT(46, "SymEnumSourceFileTokens");
	__asm { jmp functions[4 * 45] }
}

// SymEnumSourceFiles
__declspec(naked) void func_d80f5c3d922b4d10a8d6423b5df8f711()
{
	DLLEXPORT(47, "SymEnumSourceFiles");
	__asm { jmp functions[4 * 46] }
}

// SymEnumSourceFilesW
__declspec(naked) void func_53a497d88a6e4ad0bac51b22550fea7d()
{
	DLLEXPORT(48, "SymEnumSourceFilesW");
	__asm { jmp functions[4 * 47] }
}

// SymEnumSourceLines
__declspec(naked) void func_21edb1639b994e01987b1f66ca8646a7()
{
	DLLEXPORT(49, "SymEnumSourceLines");
	__asm { jmp functions[4 * 48] }
}

// SymEnumSourceLinesW
__declspec(naked) void func_1b3335607fb64530bd3495c0ff266fb3()
{
	DLLEXPORT(50, "SymEnumSourceLinesW");
	__asm { jmp functions[4 * 49] }
}

// SymEnumSym
__declspec(naked) void func_6066a8f9c1c347ae8b975e6479c62de7()
{
	DLLEXPORT(51, "SymEnumSym");
	__asm { jmp functions[4 * 50] }
}

// SymEnumSymbols
__declspec(naked) void func_ccfe3e2b090e487ea15050b5541ab8b0()
{
	DLLEXPORT(52, "SymEnumSymbols");
	__asm { jmp functions[4 * 51] }
}

// SymEnumSymbolsForAddr
__declspec(naked) void func_65d70ff7baab4bfb89d70a66b31194c7()
{
	DLLEXPORT(53, "SymEnumSymbolsForAddr");
	__asm { jmp functions[4 * 52] }
}

// SymEnumSymbolsForAddrW
__declspec(naked) void func_10c15b85c30d47f888edafb43a270749()
{
	DLLEXPORT(54, "SymEnumSymbolsForAddrW");
	__asm { jmp functions[4 * 53] }
}

// SymEnumSymbolsW
__declspec(naked) void func_dec2580687fd4823ba6dc2e0ebc630db()
{
	DLLEXPORT(55, "SymEnumSymbolsW");
	__asm { jmp functions[4 * 54] }
}

// SymEnumTypes
__declspec(naked) void func_8545332119ec43d4af365cb188fa027a()
{
	DLLEXPORT(56, "SymEnumTypes");
	__asm { jmp functions[4 * 55] }
}

// SymEnumTypesByName
__declspec(naked) void func_982daa1bcd794427b81704fac6ce0fc7()
{
	DLLEXPORT(57, "SymEnumTypesByName");
	__asm { jmp functions[4 * 56] }
}

// SymEnumTypesByNameW
__declspec(naked) void func_be616e9bfadc4893b0958ead99be5085()
{
	DLLEXPORT(58, "SymEnumTypesByNameW");
	__asm { jmp functions[4 * 57] }
}

// SymEnumTypesW
__declspec(naked) void func_8509385d21e84899a2cdc7c320a0d5f5()
{
	DLLEXPORT(59, "SymEnumTypesW");
	__asm { jmp functions[4 * 58] }
}

// SymEnumerateModules
__declspec(naked) void func_6432ce5c8bbf42bfab84e63247fe48a8()
{
	DLLEXPORT(60, "SymEnumerateModules");
	__asm { jmp functions[4 * 59] }
}

// SymEnumerateModules64
__declspec(naked) void func_0b019bb7e1e145e58195fca1d8f6036b()
{
	DLLEXPORT(61, "SymEnumerateModules64");
	__asm { jmp functions[4 * 60] }
}

// SymEnumerateModulesW64
__declspec(naked) void func_e6571e48dab8420494a6f6af9af72a0d()
{
	DLLEXPORT(62, "SymEnumerateModulesW64");
	__asm { jmp functions[4 * 61] }
}

// SymEnumerateSymbols
__declspec(naked) void func_f7702985adbd4a34aa0e9e89e551f511()
{
	DLLEXPORT(63, "SymEnumerateSymbols");
	__asm { jmp functions[4 * 62] }
}

// SymEnumerateSymbols64
__declspec(naked) void func_ededf04ce3504935adc660970ace8a00()
{
	DLLEXPORT(64, "SymEnumerateSymbols64");
	__asm { jmp functions[4 * 63] }
}

// SymEnumerateSymbolsW
__declspec(naked) void func_11bf72596b30417a88ee694562c2efa5()
{
	DLLEXPORT(65, "SymEnumerateSymbolsW");
	__asm { jmp functions[4 * 64] }
}

// SymEnumerateSymbolsW64
__declspec(naked) void func_1aabd167bab143b4a92ee2806e158e9a()
{
	DLLEXPORT(66, "SymEnumerateSymbolsW64");
	__asm { jmp functions[4 * 65] }
}

// SymFindDebugInfoFile
__declspec(naked) void func_4b93dc491f8f497da47d5322780bceb7()
{
	DLLEXPORT(67, "SymFindDebugInfoFile");
	__asm { jmp functions[4 * 66] }
}

// SymFindDebugInfoFileW
__declspec(naked) void func_905c5ab135ee46a2b75244afeeae6da0()
{
	DLLEXPORT(68, "SymFindDebugInfoFileW");
	__asm { jmp functions[4 * 67] }
}

// SymFindExecutableImage
__declspec(naked) void func_cc343f075f2f4661abb882a767c513f0()
{
	DLLEXPORT(69, "SymFindExecutableImage");
	__asm { jmp functions[4 * 68] }
}

// SymFindExecutableImageW
__declspec(naked) void func_ebeb82ac479c41e6bfa899a830b85938()
{
	DLLEXPORT(70, "SymFindExecutableImageW");
	__asm { jmp functions[4 * 69] }
}

// SymFindFileInPath
__declspec(naked) void func_b69da3ea6177444d934c3e82a3e0c951()
{
	DLLEXPORT(71, "SymFindFileInPath");
	__asm { jmp functions[4 * 70] }
}

// SymFindFileInPathW
__declspec(naked) void func_f8c266a3d1974e40b7b39291ca801bd6()
{
	DLLEXPORT(72, "SymFindFileInPathW");
	__asm { jmp functions[4 * 71] }
}

// SymFromAddr
__declspec(naked) void func_2fff4e59e653424e931060e6aa557efe()
{
	DLLEXPORT(73, "SymFromAddr");
	__asm { jmp functions[4 * 72] }
}

// SymFromAddrW
__declspec(naked) void func_2283692ee8064611b516fad6715e8191()
{
	DLLEXPORT(74, "SymFromAddrW");
	__asm { jmp functions[4 * 73] }
}

// SymFromIndex
__declspec(naked) void func_7f09cae2f2d5428aa06759fa30738d0b()
{
	DLLEXPORT(75, "SymFromIndex");
	__asm { jmp functions[4 * 74] }
}

// SymFromIndexW
__declspec(naked) void func_c22b0a51fe7b4cb1ab6a6dbc71788fa8()
{
	DLLEXPORT(76, "SymFromIndexW");
	__asm { jmp functions[4 * 75] }
}

// SymFromName
__declspec(naked) void func_7ba6e3ed95954dc1927bcbe5fc6e6a4e()
{
	DLLEXPORT(77, "SymFromName");
	__asm { jmp functions[4 * 76] }
}

// SymFromNameW
__declspec(naked) void func_c79128c718d844378b06f8111b5926c4()
{
	DLLEXPORT(78, "SymFromNameW");
	__asm { jmp functions[4 * 77] }
}

// SymFromToken
__declspec(naked) void func_5a94e3a154444145a2842b23316a0403()
{
	DLLEXPORT(79, "SymFromToken");
	__asm { jmp functions[4 * 78] }
}

// SymFromTokenW
__declspec(naked) void func_7533b12d16aa4637b69bee62ed2455ec()
{
	DLLEXPORT(80, "SymFromTokenW");
	__asm { jmp functions[4 * 79] }
}

// SymFunctionTableAccess
__declspec(naked) void func_00aba1bf1273441f8ab83bf8ddab9e03()
{
	DLLEXPORT(81, "SymFunctionTableAccess");
	__asm { jmp functions[4 * 80] }
}

// SymFunctionTableAccess64
__declspec(naked) void func_c2771bcda16b48e484b6e8da4da8bdfc()
{
	DLLEXPORT(82, "SymFunctionTableAccess64");
	__asm { jmp functions[4 * 81] }
}

// SymGetFileLineOffsets64
__declspec(naked) void func_78c3fe5cd6704ba3bfb0eedafa4f5eab()
{
	DLLEXPORT(83, "SymGetFileLineOffsets64");
	__asm { jmp functions[4 * 82] }
}

// SymGetHomeDirectory
__declspec(naked) void func_844939bf71a94dfebe8154f9dbeaa86c()
{
	DLLEXPORT(84, "SymGetHomeDirectory");
	__asm { jmp functions[4 * 83] }
}

// SymGetHomeDirectoryW
__declspec(naked) void func_e10bd8057fda4b58b9160661d3189687()
{
	DLLEXPORT(85, "SymGetHomeDirectoryW");
	__asm { jmp functions[4 * 84] }
}

// SymGetLineFromAddr
__declspec(naked) void func_16e490a18eba4031bd704765d4b42afb()
{
	DLLEXPORT(86, "SymGetLineFromAddr");
	__asm { jmp functions[4 * 85] }
}

// SymGetLineFromAddr64
__declspec(naked) void func_295649183282437c8ab1c699568511d6()
{
	DLLEXPORT(87, "SymGetLineFromAddr64");
	__asm { jmp functions[4 * 86] }
}

// SymGetLineFromAddrW64
__declspec(naked) void func_84057702fbe3461eba174dd4373264eb()
{
	DLLEXPORT(88, "SymGetLineFromAddrW64");
	__asm { jmp functions[4 * 87] }
}

// SymGetLineFromName
__declspec(naked) void func_5a17bb5195a147caa3bde5003dfcfe6b()
{
	DLLEXPORT(89, "SymGetLineFromName");
	__asm { jmp functions[4 * 88] }
}

// SymGetLineFromName64
__declspec(naked) void func_15799570c38b4147bc1970f902b94765()
{
	DLLEXPORT(90, "SymGetLineFromName64");
	__asm { jmp functions[4 * 89] }
}

// SymGetLineFromNameW64
__declspec(naked) void func_72529a65d6e54d6f89299128357604cb()
{
	DLLEXPORT(91, "SymGetLineFromNameW64");
	__asm { jmp functions[4 * 90] }
}

// SymGetLineNext
__declspec(naked) void func_e95fefd1adc141ecb362bd646de3f6a3()
{
	DLLEXPORT(92, "SymGetLineNext");
	__asm { jmp functions[4 * 91] }
}

// SymGetLineNext64
__declspec(naked) void func_790b83a572ef4574968dd534aee50bf8()
{
	DLLEXPORT(93, "SymGetLineNext64");
	__asm { jmp functions[4 * 92] }
}

// SymGetLineNextW64
__declspec(naked) void func_08411792f81c4fa1901897dfc9468609()
{
	DLLEXPORT(94, "SymGetLineNextW64");
	__asm { jmp functions[4 * 93] }
}

// SymGetLinePrev
__declspec(naked) void func_dafde465cda7405eac780f53d5b8e8f6()
{
	DLLEXPORT(95, "SymGetLinePrev");
	__asm { jmp functions[4 * 94] }
}

// SymGetLinePrev64
__declspec(naked) void func_f9f2360a911243968b9cdb2ec321231e()
{
	DLLEXPORT(96, "SymGetLinePrev64");
	__asm { jmp functions[4 * 95] }
}

// SymGetLinePrevW64
__declspec(naked) void func_3e5099fd41b5453089802e370c5d223b()
{
	DLLEXPORT(97, "SymGetLinePrevW64");
	__asm { jmp functions[4 * 96] }
}

// SymGetModuleBase
__declspec(naked) void func_2f81cae3af204fbe8432a2ba241e360e()
{
	DLLEXPORT(98, "SymGetModuleBase");
	__asm { jmp functions[4 * 97] }
}

// SymGetModuleBase64
__declspec(naked) void func_e4d10c883ea849b48ea80775567a50e1()
{
	DLLEXPORT(99, "SymGetModuleBase64");
	__asm { jmp functions[4 * 98] }
}

// SymGetModuleInfo
__declspec(naked) void func_1f273cd2ab3a4de6ba73b5750e73355e()
{
	DLLEXPORT(100, "SymGetModuleInfo");
	__asm { jmp functions[4 * 99] }
}

// SymGetModuleInfo64
__declspec(naked) void func_0d3dae2bdc85403a80ede04446760e11()
{
	DLLEXPORT(101, "SymGetModuleInfo64");
	__asm { jmp functions[4 * 100] }
}

// SymGetModuleInfoW
__declspec(naked) void func_8c4f34293d9749e78f54fd1c307df303()
{
	DLLEXPORT(102, "SymGetModuleInfoW");
	__asm { jmp functions[4 * 101] }
}

// SymGetModuleInfoW64
__declspec(naked) void func_d8ac1ae404534685a01c28ca274897a0()
{
	DLLEXPORT(103, "SymGetModuleInfoW64");
	__asm { jmp functions[4 * 102] }
}

// SymGetOmapBlockBase
__declspec(naked) void func_c849e49d83044c89afecc2e2cc69b94a()
{
	DLLEXPORT(104, "SymGetOmapBlockBase");
	__asm { jmp functions[4 * 103] }
}

// SymGetOmaps
__declspec(naked) void func_c8d80b500ac84724b920e4c415aac41e()
{
	DLLEXPORT(105, "SymGetOmaps");
	__asm { jmp functions[4 * 104] }
}

// SymGetOptions
__declspec(naked) void func_0d853ab933b54f48b8ca3c0d51be2103()
{
	DLLEXPORT(106, "SymGetOptions");
	__asm { jmp functions[4 * 105] }
}

// SymGetScope
__declspec(naked) void func_ea07b0355c1344369b29e672ff924252()
{
	DLLEXPORT(107, "SymGetScope");
	__asm { jmp functions[4 * 106] }
}

// SymGetScopeW
__declspec(naked) void func_8a2f5c9bac2f4c9ea25a4ac1a3d36e35()
{
	DLLEXPORT(108, "SymGetScopeW");
	__asm { jmp functions[4 * 107] }
}

// SymGetSearchPath
__declspec(naked) void func_227f4b3ff4094553bed667c882f2530f()
{
	DLLEXPORT(109, "SymGetSearchPath");
	__asm { jmp functions[4 * 108] }
}

// SymGetSearchPathW
__declspec(naked) void func_f40c3c842eac4b8482320aa92f1c3ee4()
{
	DLLEXPORT(110, "SymGetSearchPathW");
	__asm { jmp functions[4 * 109] }
}

// SymGetSourceFile
__declspec(naked) void func_2a03131a7fa8400ebbddefcb2ecfa3d5()
{
	DLLEXPORT(111, "SymGetSourceFile");
	__asm { jmp functions[4 * 110] }
}

// SymGetSourceFileFromToken
__declspec(naked) void func_8ec6bcb391e04eacb0fb9038f26a367d()
{
	DLLEXPORT(112, "SymGetSourceFileFromToken");
	__asm { jmp functions[4 * 111] }
}

// SymGetSourceFileFromTokenW
__declspec(naked) void func_e48c58cf6a4b4ae38f3a372476af1bbc()
{
	DLLEXPORT(113, "SymGetSourceFileFromTokenW");
	__asm { jmp functions[4 * 112] }
}

// SymGetSourceFileToken
__declspec(naked) void func_d0345321615240948f62cdf90fea71a5()
{
	DLLEXPORT(114, "SymGetSourceFileToken");
	__asm { jmp functions[4 * 113] }
}

// SymGetSourceFileTokenW
__declspec(naked) void func_387f9f0044c4459fb65972b985879e4a()
{
	DLLEXPORT(115, "SymGetSourceFileTokenW");
	__asm { jmp functions[4 * 114] }
}

// SymGetSourceFileW
__declspec(naked) void func_17bebd62384b49ac88ce8e759c02d20a()
{
	DLLEXPORT(116, "SymGetSourceFileW");
	__asm { jmp functions[4 * 115] }
}

// SymGetSourceVarFromToken
__declspec(naked) void func_bc5e319dfd5842c7aaa44717a201acbc()
{
	DLLEXPORT(117, "SymGetSourceVarFromToken");
	__asm { jmp functions[4 * 116] }
}

// SymGetSourceVarFromTokenW
__declspec(naked) void func_a73ed1f8cf264ab1a20b126582c0dd04()
{
	DLLEXPORT(118, "SymGetSourceVarFromTokenW");
	__asm { jmp functions[4 * 117] }
}

// SymGetSymFromAddr
__declspec(naked) void func_0a9b7aa114ce4ddfba36437e6fada5bd()
{
	DLLEXPORT(119, "SymGetSymFromAddr");
	__asm { jmp functions[4 * 118] }
}

// SymGetSymFromAddr64
__declspec(naked) void func_0058b757505c4d6193a7e6e603276522()
{
	DLLEXPORT(120, "SymGetSymFromAddr64");
	__asm { jmp functions[4 * 119] }
}

// SymGetSymFromName
__declspec(naked) void func_4cd77fa26afb4c8682b0f82fdc5d5cdb()
{
	DLLEXPORT(121, "SymGetSymFromName");
	__asm { jmp functions[4 * 120] }
}

// SymGetSymFromName64
__declspec(naked) void func_5ddcaa81dd03481cb152bce499dc7e0a()
{
	DLLEXPORT(122, "SymGetSymFromName64");
	__asm { jmp functions[4 * 121] }
}

// SymGetSymNext
__declspec(naked) void func_9c2bae5b5fa2442e83f3665b56b48872()
{
	DLLEXPORT(123, "SymGetSymNext");
	__asm { jmp functions[4 * 122] }
}

// SymGetSymNext64
__declspec(naked) void func_4c5bcdca3da84e4aa5cd864d4b06df18()
{
	DLLEXPORT(124, "SymGetSymNext64");
	__asm { jmp functions[4 * 123] }
}

// SymGetSymPrev
__declspec(naked) void func_32348f71ca6e483da40d4180244dfe8b()
{
	DLLEXPORT(125, "SymGetSymPrev");
	__asm { jmp functions[4 * 124] }
}

// SymGetSymPrev64
__declspec(naked) void func_57423065b46543a29005f9148a7a9c3b()
{
	DLLEXPORT(126, "SymGetSymPrev64");
	__asm { jmp functions[4 * 125] }
}

// SymGetSymbolFile
__declspec(naked) void func_e6dc734fa725459c9f24467217cf4622()
{
	DLLEXPORT(127, "SymGetSymbolFile");
	__asm { jmp functions[4 * 126] }
}

// SymGetSymbolFileW
__declspec(naked) void func_7710d9255e1b4ceabe8376b8119f13d7()
{
	DLLEXPORT(128, "SymGetSymbolFileW");
	__asm { jmp functions[4 * 127] }
}

// SymGetTypeFromName
__declspec(naked) void func_b18e4e4d25074cdba592b591575ea6ce()
{
	DLLEXPORT(129, "SymGetTypeFromName");
	__asm { jmp functions[4 * 128] }
}

// SymGetTypeFromNameW
__declspec(naked) void func_a78fb6290d5e4a96a915cbeb3252c07d()
{
	DLLEXPORT(130, "SymGetTypeFromNameW");
	__asm { jmp functions[4 * 129] }
}

// SymGetTypeInfo
__declspec(naked) void func_6f81ebe549c2473e9bc722eaa61adc0a()
{
	DLLEXPORT(131, "SymGetTypeInfo");
	__asm { jmp functions[4 * 130] }
}

// SymGetTypeInfoEx
__declspec(naked) void func_f77ba4d44ebe422fb61d82432d76123e()
{
	DLLEXPORT(132, "SymGetTypeInfoEx");
	__asm { jmp functions[4 * 131] }
}

// SymInitialize
__declspec(naked) void func_49bd381f905e4622bf7a31c5bfe1dfb4()
{
	DLLEXPORT(133, "SymInitialize");
	__asm { jmp functions[4 * 132] }
}

// SymInitializeW
__declspec(naked) void func_15e2ab068bd548a5a2e9bc14fa820c50()
{
	DLLEXPORT(134, "SymInitializeW");
	__asm { jmp functions[4 * 133] }
}

// SymLoadModule
__declspec(naked) void func_8b1ad77da98b495b93334625c27f6e85()
{
	DLLEXPORT(135, "SymLoadModule");
	__asm { jmp functions[4 * 134] }
}

// SymLoadModule64
__declspec(naked) void func_2f8ed4f615a949a397c7c0bb877aa731()
{
	DLLEXPORT(136, "SymLoadModule64");
	__asm { jmp functions[4 * 135] }
}

// SymLoadModuleEx
__declspec(naked) void func_7d12ae4840f44c6abb5a325358342cd7()
{
	DLLEXPORT(137, "SymLoadModuleEx");
	__asm { jmp functions[4 * 136] }
}

// SymLoadModuleExW
__declspec(naked) void func_76295650e6ef464f90a1c25520b1718c()
{
	DLLEXPORT(138, "SymLoadModuleExW");
	__asm { jmp functions[4 * 137] }
}

// SymMatchFileName
__declspec(naked) void func_d02af00337fa4c92afa85aff16e798fb()
{
	DLLEXPORT(139, "SymMatchFileName");
	__asm { jmp functions[4 * 138] }
}

// SymMatchFileNameW
__declspec(naked) void func_8a4aada2f83a4e88860cf80ff9157601()
{
	DLLEXPORT(140, "SymMatchFileNameW");
	__asm { jmp functions[4 * 139] }
}

// SymMatchString
__declspec(naked) void func_18e51ea8f6fb4f429a29985e03a3ed25()
{
	DLLEXPORT(141, "SymMatchString");
	__asm { jmp functions[4 * 140] }
}

// SymMatchStringA
__declspec(naked) void func_ca51aa0893e64b6bbca9c86977412e56()
{
	DLLEXPORT(142, "SymMatchStringA");
	__asm { jmp functions[4 * 141] }
}

// SymMatchStringW
__declspec(naked) void func_ead7ce6e4ea344bcbefd77e096035a79()
{
	DLLEXPORT(143, "SymMatchStringW");
	__asm { jmp functions[4 * 142] }
}

// SymNext
__declspec(naked) void func_f2338aecdcd34b8291b70eeaac546492()
{
	DLLEXPORT(144, "SymNext");
	__asm { jmp functions[4 * 143] }
}

// SymNextW
__declspec(naked) void func_65a9bea1557941e28ff6e75664d2a3ce()
{
	DLLEXPORT(145, "SymNextW");
	__asm { jmp functions[4 * 144] }
}

// SymPrev
__declspec(naked) void func_ea18acfa46aa435aac141612e2fd8657()
{
	DLLEXPORT(146, "SymPrev");
	__asm { jmp functions[4 * 145] }
}

// SymPrevW
__declspec(naked) void func_75ca7b61c8364c15afe2db0ee48d793b()
{
	DLLEXPORT(147, "SymPrevW");
	__asm { jmp functions[4 * 146] }
}

// SymRefreshModuleList
__declspec(naked) void func_81342de50e8246f6b4e302a116254a95()
{
	DLLEXPORT(148, "SymRefreshModuleList");
	__asm { jmp functions[4 * 147] }
}

// SymRegisterCallback
__declspec(naked) void func_fb9162e2358743d893827e278b3a7c94()
{
	DLLEXPORT(149, "SymRegisterCallback");
	__asm { jmp functions[4 * 148] }
}

// SymRegisterCallback64
__declspec(naked) void func_117a0381ff044be6a8dc6754a8643d46()
{
	DLLEXPORT(150, "SymRegisterCallback64");
	__asm { jmp functions[4 * 149] }
}

// SymRegisterCallbackW64
__declspec(naked) void func_4e54519b11e445e2ac31cc69fd52d02c()
{
	DLLEXPORT(151, "SymRegisterCallbackW64");
	__asm { jmp functions[4 * 150] }
}

// SymRegisterFunctionEntryCallback
__declspec(naked) void func_c4b7efc52dac4eb3894bae086b8ca6b9()
{
	DLLEXPORT(152, "SymRegisterFunctionEntryCallback");
	__asm { jmp functions[4 * 151] }
}

// SymRegisterFunctionEntryCallback64
__declspec(naked) void func_7cbc8618514f4f12813aa5e31a2d174c()
{
	DLLEXPORT(153, "SymRegisterFunctionEntryCallback64");
	__asm { jmp functions[4 * 152] }
}

// SymSearch
__declspec(naked) void func_c8ac0c0b6f2740eca5abdcfb47fde0a2()
{
	DLLEXPORT(154, "SymSearch");
	__asm { jmp functions[4 * 153] }
}

// SymSearchW
__declspec(naked) void func_6eb3a9c6197045b48c2975ed292685be()
{
	DLLEXPORT(155, "SymSearchW");
	__asm { jmp functions[4 * 154] }
}

// SymSetContext
__declspec(naked) void func_9b4a2cb6a5a8455b83ff4c1f590d7095()
{
	DLLEXPORT(156, "SymSetContext");
	__asm { jmp functions[4 * 155] }
}

// SymSetHomeDirectory
__declspec(naked) void func_1ddadf242cfe43929a4adebdde22846a()
{
	DLLEXPORT(157, "SymSetHomeDirectory");
	__asm { jmp functions[4 * 156] }
}

// SymSetHomeDirectoryW
__declspec(naked) void func_9cf272d6806f45239659711ed91276c4()
{
	DLLEXPORT(158, "SymSetHomeDirectoryW");
	__asm { jmp functions[4 * 157] }
}

// SymSetOptions
__declspec(naked) void func_67cae52e9dd344b7baa6eb042c6043c6()
{
	DLLEXPORT(159, "SymSetOptions");
	__asm { jmp functions[4 * 158] }
}

// SymSetParentWindow
__declspec(naked) void func_f0f7203a5a5d424ea4220779f96d0f75()
{
	DLLEXPORT(160, "SymSetParentWindow");
	__asm { jmp functions[4 * 159] }
}

// SymSetScopeFromAddr
__declspec(naked) void func_8ba53a4ba0414f0eb52a620dd720734d()
{
	DLLEXPORT(161, "SymSetScopeFromAddr");
	__asm { jmp functions[4 * 160] }
}

// SymSetScopeFromIndex
__declspec(naked) void func_febfc8c3f72147b4abc9820e7a42a93f()
{
	DLLEXPORT(162, "SymSetScopeFromIndex");
	__asm { jmp functions[4 * 161] }
}

// SymSetSearchPath
__declspec(naked) void func_6a5dcf114a8c4c8287c2aab22af325a9()
{
	DLLEXPORT(163, "SymSetSearchPath");
	__asm { jmp functions[4 * 162] }
}

// SymSetSearchPathW
__declspec(naked) void func_84b497fd467e471fa60152cb346aaa3b()
{
	DLLEXPORT(164, "SymSetSearchPathW");
	__asm { jmp functions[4 * 163] }
}

// SymSrvDeltaName
__declspec(naked) void func_f34ed09571ce433abac40fdd355ada1c()
{
	DLLEXPORT(165, "SymSrvDeltaName");
	__asm { jmp functions[4 * 164] }
}

// SymSrvDeltaNameW
__declspec(naked) void func_b9bd373788104b5da76520f4bb26778d()
{
	DLLEXPORT(166, "SymSrvDeltaNameW");
	__asm { jmp functions[4 * 165] }
}

// SymSrvGetFileIndexInfo
__declspec(naked) void func_af87233b63a64ba691978f15eb2d6e16()
{
	DLLEXPORT(167, "SymSrvGetFileIndexInfo");
	__asm { jmp functions[4 * 166] }
}

// SymSrvGetFileIndexInfoW
__declspec(naked) void func_cf5fee122b3c491cafbd31efb0864055()
{
	DLLEXPORT(168, "SymSrvGetFileIndexInfoW");
	__asm { jmp functions[4 * 167] }
}

// SymSrvGetFileIndexString
__declspec(naked) void func_6b7b3b4e08254033bcfc0031f24eaabd()
{
	DLLEXPORT(169, "SymSrvGetFileIndexString");
	__asm { jmp functions[4 * 168] }
}

// SymSrvGetFileIndexStringW
__declspec(naked) void func_9cf72b21a0cc47f385d99f028c78043f()
{
	DLLEXPORT(170, "SymSrvGetFileIndexStringW");
	__asm { jmp functions[4 * 169] }
}

// SymSrvGetFileIndexes
__declspec(naked) void func_333786d986ba4ddd84ba7400baa8c2a6()
{
	DLLEXPORT(171, "SymSrvGetFileIndexes");
	__asm { jmp functions[4 * 170] }
}

// SymSrvGetFileIndexesW
__declspec(naked) void func_457ebe30ba1e4f399421228894f7d745()
{
	DLLEXPORT(172, "SymSrvGetFileIndexesW");
	__asm { jmp functions[4 * 171] }
}

// SymSrvGetSupplement
__declspec(naked) void func_eb925318448f4b568acccd51bb5e7fe9()
{
	DLLEXPORT(173, "SymSrvGetSupplement");
	__asm { jmp functions[4 * 172] }
}

// SymSrvGetSupplementW
__declspec(naked) void func_ee464a1db7514d768db48e60903be873()
{
	DLLEXPORT(174, "SymSrvGetSupplementW");
	__asm { jmp functions[4 * 173] }
}

// SymSrvIsStore
__declspec(naked) void func_a9c0331e0fd54fa18c6a65b9d4455499()
{
	DLLEXPORT(175, "SymSrvIsStore");
	__asm { jmp functions[4 * 174] }
}

// SymSrvIsStoreW
__declspec(naked) void func_2e46a4a669ab407eb35e2b85c47d2ce6()
{
	DLLEXPORT(176, "SymSrvIsStoreW");
	__asm { jmp functions[4 * 175] }
}

// SymSrvStoreFile
__declspec(naked) void func_c5c99b041ce649bda188ade0bcdbee27()
{
	DLLEXPORT(177, "SymSrvStoreFile");
	__asm { jmp functions[4 * 176] }
}

// SymSrvStoreFileW
__declspec(naked) void func_9f73acf41d4541c5af7a328e302992c4()
{
	DLLEXPORT(178, "SymSrvStoreFileW");
	__asm { jmp functions[4 * 177] }
}

// SymSrvStoreSupplement
__declspec(naked) void func_48f732c0b00d473580ef5883e18777c6()
{
	DLLEXPORT(179, "SymSrvStoreSupplement");
	__asm { jmp functions[4 * 178] }
}

// SymSrvStoreSupplementW
__declspec(naked) void func_948bffbeb6394809a1bc4e893355d676()
{
	DLLEXPORT(180, "SymSrvStoreSupplementW");
	__asm { jmp functions[4 * 179] }
}

// SymUnDName
__declspec(naked) void func_9ae37efc0cab4d3197eda6ebe3328bec()
{
	DLLEXPORT(181, "SymUnDName");
	__asm { jmp functions[4 * 180] }
}

// SymUnDName64
__declspec(naked) void func_3414c6f2c0c04a32be9587fbc6b08cbd()
{
	DLLEXPORT(182, "SymUnDName64");
	__asm { jmp functions[4 * 181] }
}

// SymUnloadModule
__declspec(naked) void func_7c2598cfadd14ca2ae420ef23abc05b4()
{
	DLLEXPORT(183, "SymUnloadModule");
	__asm { jmp functions[4 * 182] }
}

// SymUnloadModule64
__declspec(naked) void func_b110b07cdc034b548f30ff6d744d0135()
{
	DLLEXPORT(184, "SymUnloadModule64");
	__asm { jmp functions[4 * 183] }
}

// UnDecorateSymbolName
__declspec(naked) void func_3513647d37214f87a413010348cad355()
{
	DLLEXPORT(185, "UnDecorateSymbolName");
	__asm { jmp functions[4 * 184] }
}

// UnDecorateSymbolNameW
__declspec(naked) void func_e4b4dfb8789e475a9f64a828af02312e()
{
	DLLEXPORT(186, "UnDecorateSymbolNameW");
	__asm { jmp functions[4 * 185] }
}

// UnmapDebugInformation
__declspec(naked) void func_0b0b2e15ba5846af931b4648373e6612()
{
	DLLEXPORT(187, "UnmapDebugInformation");
	__asm { jmp functions[4 * 186] }
}

// WinDbgExtensionDllInit
__declspec(naked) void func_5ec50c06a9e54495b675638d4627620d()
{
	DLLEXPORT(188, "WinDbgExtensionDllInit");
	__asm { jmp functions[4 * 187] }
}

// block
__declspec(naked) void func_826095bdc3cf498680efc5b5679c91a9()
{
	DLLEXPORT(189, "block");
	__asm { jmp functions[4 * 188] }
}

// chksym
__declspec(naked) void func_ef266386b6204bbcabed372ac0a0516f()
{
	DLLEXPORT(190, "chksym");
	__asm { jmp functions[4 * 189] }
}

// dbghelp
__declspec(naked) void func_dd6f37f1708342fc96d1a4d612798c77()
{
	DLLEXPORT(191, "dbghelp");
	__asm { jmp functions[4 * 190] }
}

// dh
__declspec(naked) void func_84077d69a90a4988a5a8fdce67aa079e()
{
	DLLEXPORT(192, "dh");
	__asm { jmp functions[4 * 191] }
}

// fptr
__declspec(naked) void func_9ce9b895baf44fb98d4c3422708df518()
{
	DLLEXPORT(193, "fptr");
	__asm { jmp functions[4 * 192] }
}

// homedir
__declspec(naked) void func_3ca3eee3c06e497184b5b41114c48cac()
{
	DLLEXPORT(194, "homedir");
	__asm { jmp functions[4 * 193] }
}

// itoldyouso
__declspec(naked) void func_7045bbeee1534dbca120eb469d1bc345()
{
	DLLEXPORT(195, "itoldyouso");
	__asm { jmp functions[4 * 194] }
}

// lmi
__declspec(naked) void func_e6734f5c9afc4485a0383194bdd9aff1()
{
	DLLEXPORT(196, "lmi");
	__asm { jmp functions[4 * 195] }
}

// lminfo
__declspec(naked) void func_10ae41fe9acf4c71a89a7250eab053d3()
{
	DLLEXPORT(197, "lminfo");
	__asm { jmp functions[4 * 196] }
}

// omap
__declspec(naked) void func_84aa2a280e994f3bb77fd464b5a4198d()
{
	DLLEXPORT(198, "omap");
	__asm { jmp functions[4 * 197] }
}

// srcfiles
__declspec(naked) void func_42709efaacbd48aab81c7800a7cf515c()
{
	DLLEXPORT(199, "srcfiles");
	__asm { jmp functions[4 * 198] }
}

// stack_force_ebp
__declspec(naked) void func_dded76eaa5ee4ec98471a358d708e377()
{
	DLLEXPORT(200, "stack_force_ebp");
	__asm { jmp functions[4 * 199] }
}

// stackdbg
__declspec(naked) void func_db37820a39b84e40b61e98b1467bca4e()
{
	DLLEXPORT(201, "stackdbg");
	__asm { jmp functions[4 * 200] }
}

// sym
__declspec(naked) void func_7cb22ddb872644bdac78a63aa67208f5()
{
	DLLEXPORT(202, "sym");
	__asm { jmp functions[4 * 201] }
}

// symsrv
__declspec(naked) void func_ffcc170eeb954573a192d5a60f810e32()
{
	DLLEXPORT(203, "symsrv");
	__asm { jmp functions[4 * 202] }
}

// vc7fpo
__declspec(naked) void func_9d6bef2069e7444cb14818fbe180d082()
{
	DLLEXPORT(204, "vc7fpo");
	__asm { jmp functions[4 * 203] }
}

#pragma endregion

static decltype(&CreateFileW) originalEnumProcessModulesEx = CreateFileW;

HANDLE WINAPI hookedCreateFileA(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
) {
	// std::wcout << L"lpFileName:" << lpFileName << std::endl;
	if (std::wstring(lpFileName).find(L"dbghelp.dll") != std::string::npos)
	{
		return INVALID_HANDLE_VALUE;
	}
	else
	{
		return originalEnumProcessModulesEx(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstModule);

		/*
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE* pFile = nullptr;
		freopen_s(&pFile, "CON", "r", stdin);
		freopen_s(&pFile, "CON", "w", stdout);
		freopen_s(&pFile, "CON", "w", stderr);
		*/

		DisableThreadLibraryCalls(hinstModule);
		auto moduleHandle = LoadLibrary(L"original_dll\\dbghelp.dll");
		if (moduleHandle != nullptr)
		{
			for (auto i = 0; i < FUNCTION_COUNT; i++)
			{
				auto address = GetProcAddress(moduleHandle, functionNames[i].c_str());
				functions[i] = reinterpret_cast<uintptr_t>(address);
			}
		}

		DetourFunction(true, reinterpret_cast<PVOID*>(&originalEnumProcessModulesEx), hookedCreateFileA);
		HookLine();
	}

	return TRUE;
}
