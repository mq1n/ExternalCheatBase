#include "HaxEngine.h"
#include "main.h"
#include "Defines.h"
#include "Antis.h"
#include "DynamicWinAPI.h"
#include "XOR.h"

void CAntis::AntiSandboxie()
{
	CHAR _SandboxDll[] = { 'S', 'b', 'i', 'e', 'D', 'l', 'l', '.', 'd', 'l', 'l', 0x0 }; //"SbieDll.dll"
	HMODULE sandbox = GetModuleHandleA(_SandboxDll);

	if (sandbox) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on AntiSandboxie!");
		abort();
	}
}

void CAntis::AntiVirtualMachine()
{
	unsigned int reax = 0;

	__asm
	{
		mov eax, 0xCCCCCCCC;
		smsw eax;
		mov DWORD PTR[reax], eax;
	}

	if ((((reax >> 24) & 0xFF) == 0xcc) && (((reax >> 16) & 0xFF) == 0xcc)) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on AntiVirtualMachine!");
		abort();
	}
}

void CAntis::AntiSunbeltSandBox()
{
	CHAR szFileName[MAX_PATH];
	GetModuleFileNameA(NULL, szFileName, MAX_PATH);

	CHAR sandboxfile[] = { 'C', ':', '\\', '\\', 'f', 'i', 'l', 'e', '.', 'e', 'x', 'e', 0x0 };
	CHAR __pstorecdll[] = { 'p', 's', 't', 'o', 'r', 'e', 'c', '.', 'd', 'l', 'l', 0x0 }; // pstorec.dll
	if (!strcmp(szFileName, sandboxfile) || GetModuleHandleA(__pstorecdll)) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on AntiSunbeltSandBox!");
		abort();
	}
}

void CAntis::AntiWine()
{
	CHAR __wine_get_unix_file_name[] = { 'w', 'i', 'n', 'e', '_', 'g', 'e', 't', '_', 'u', 'n', 'i', 'x', '_', 'f', 'i', 'l', 'e', '_', 'n', 'a', 'm', 'e', 0x0 }; // wine_get_unix_file_name
	if (GetProcAddress(WinModuleTable->hKernel32, __wine_get_unix_file_name)) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on AntiWine!");
		abort();
	}
}

void CAntis::Anticuckoomon()
{
	CHAR __cuckoomondll[] = { 'c', 'u', 'c', 'k', 'o', 'o', 'm', 'o', 'n', '.', 'd', 'l', 'l', 0x0 }; // cuckoomon.dll
	if (GetModuleHandleA(__cuckoomondll)) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on Anticuckoomon!");
		abort();
	}
}

void CAntis::AntiSandbox()
{
	HWND hwSandbox = FindWindowExA(0, 0, XOR("Progman"), XOR("Program Manager"));
	if (!hwSandbox) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on AntiSandbox!");
		abort();
	}
}
