#include "HaxEngine.h"
#include "main.h"
#include "Defines.h"
#include "DynamicWinAPI.h"
#include "VersionHelpers.h"
#include "XOR.h"

WINAPI_MODULE_TABLE *	WinModuleTable;
WINAPI_API_TABLE	*	WinAPITable;

CDynamicWinapi::CDynamicWinapi()
{
	memset(&WinModuleTable, 0, sizeof(WINAPI_MODULE_TABLE));
	memset(&WinAPITable, 0, sizeof(WINAPI_API_TABLE));

	m_bHasInitialized = false;
}

CDynamicWinapi::~CDynamicWinapi()
{
	memset(&WinModuleTable, 0, sizeof(WINAPI_MODULE_TABLE));
	memset(&WinAPITable, 0, sizeof(WINAPI_API_TABLE));

	m_bHasInitialized = false;
}


bool CDynamicWinapi::BindModules()
{
	WinModuleTable->hBaseModule = GetModuleHandleA(0);
	if (!WinModuleTable->hBaseModule) {
		DEBUG_LOG(LL_CRI, "WinModuleTable->hBaseModule bind fail!");
		return false;
	}
	WinModuleTable->hKernel32 = LoadLibraryA(XOR("kernel32.dll"));
	if (!WinModuleTable->hKernel32) {
		DEBUG_LOG(LL_CRI, "WinModuleTable->hKernel32 bind fail!");
		return false;
	}
	WinModuleTable->hNtdll = LoadLibraryA(XOR("ntdll.dll"));
	if (!WinModuleTable->hNtdll) {
		DEBUG_LOG(LL_CRI, "WinModuleTable->hNtdll bind fail!");
		return false;
	}
	WinModuleTable->hUser32 = LoadLibraryA(XOR("user32.dll"));
	if (!WinModuleTable->hUser32) {
		DEBUG_LOG(LL_CRI, "WinModuleTable->hUser32 bind fail!");
		return false;
	}
	WinModuleTable->hPsapi = LoadLibraryA(XOR("psapi.dll"));
	if (!WinModuleTable->hPsapi) {
		DEBUG_LOG(LL_CRI, "WinModuleTable->hPsapi bind fail!");
		return false;
	}
	WinModuleTable->hDbghelp = LoadLibraryA(XOR("dbghelp.dll"));
	if (!WinModuleTable->hDbghelp) {
		DEBUG_LOG(LL_CRI, "WinModuleTable->hDbghelp bind fail!");
		return false;
	}
	return true;
}

bool CDynamicWinapi::BindAPIs()
{
	WinAPITable->IsDebuggerPresent = (lpIsDebuggerPresent)GetProcAddress(WinModuleTable->hKernel32, XOR("IsDebuggerPresent"));
	if (!WinAPITable->IsDebuggerPresent) {
		DEBUG_LOG(LL_CRI, "WinAPITable->IsDebuggerPresent bind fail!");
		return false;
	}
	WinAPITable->Sleep = (lpSleep)GetProcAddress(WinModuleTable->hKernel32, XOR("Sleep"));
	if (!WinAPITable->Sleep) {
		DEBUG_LOG(LL_CRI, "WinAPITable->Sleep bind fail!");
		return false;
	}
	if (IsWindowsVistaOrGreater())
	{
		WinAPITable->NtGetNextThread = (lpNtGetNextThread)GetProcAddress(WinModuleTable->hNtdll, XOR("NtGetNextThread"));
		if (!WinAPITable->NtGetNextThread) {
			DEBUG_LOG(LL_CRI, "WinAPITable->NtGetNextThread bind fail!");
			return false;
		}
		WinAPITable->NtGetNextProcess = (lpNtGetNextProcess)GetProcAddress(WinModuleTable->hNtdll, XOR("NtGetNextProcess"));
		if (!WinAPITable->NtGetNextProcess) {
			DEBUG_LOG(LL_CRI, "WinAPITable->NtGetNextProcess bind fail!");
			return false;
		}
		WinAPITable->NtCreateThreadEx = (lpNtCreateThreadEx)GetProcAddress(WinModuleTable->hNtdll, XOR("NtCreateThreadEx"));
		if (!WinAPITable->NtCreateThreadEx) {
			DEBUG_LOG(LL_CRI, "WinAPITable->NtCreateThreadEx bind fail!");
			return false;
		}
	}
	WinAPITable->NtQuerySystemInformation = (lpNtQuerySystemInformation)GetProcAddress(WinModuleTable->hNtdll, XOR("NtQuerySystemInformation"));
	if (!WinAPITable->NtQuerySystemInformation) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtQuerySystemInformation bind fail!");
		return false;
	}
	WinAPITable->NtDuplicateObject = (lpNtDuplicateObject)GetProcAddress(WinModuleTable->hNtdll, XOR("NtDuplicateObject"));
	if (!WinAPITable->NtDuplicateObject) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtDuplicateObject bind fail!");
		return false;
	}
	WinAPITable->NtClose = (lpNtClose)GetProcAddress(WinModuleTable->hNtdll, XOR("NtClose"));
	if (!WinAPITable->NtClose) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtClose bind fail!");
		return false;
	}
	WinAPITable->NtGetContextThread = (lpNtGetContextThread)GetProcAddress(WinModuleTable->hNtdll, XOR("NtGetContextThread"));
	if (!WinAPITable->NtGetContextThread) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtGetContextThread bind fail!");
		return false;
	}
	WinAPITable->NtSetContextThread = (lpNtSetContextThread)GetProcAddress(WinModuleTable->hNtdll, XOR("NtSetContextThread"));
	if (!WinAPITable->NtSetContextThread) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtSetContextThread bind fail!");
		return false;
	}
	WinAPITable->NtReadVirtualMemory = (lpNtReadVirtualMemory)GetProcAddress(WinModuleTable->hNtdll, XOR("NtReadVirtualMemory"));
	if (!WinAPITable->NtReadVirtualMemory) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtReadVirtualMemory bind fail!");
		return false;
	}
	WinAPITable->NtWaitForSingleObject = (lpNtWaitForSingleObject)GetProcAddress(WinModuleTable->hNtdll, XOR("NtWaitForSingleObject"));
	if (!WinAPITable->NtWaitForSingleObject) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtWaitForSingleObject bind fail!");
		return false;
	}
	WinAPITable->NtUnmapViewOfSection = (lpNtUnmapViewOfSection)GetProcAddress(WinModuleTable->hNtdll, XOR("NtUnmapViewOfSection"));
	if (!WinAPITable->NtUnmapViewOfSection) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtUnmapViewOfSection bind fail!");
		return false;
	}
	WinAPITable->NtWriteVirtualMemory = (lpNtWriteVirtualMemory)GetProcAddress(WinModuleTable->hNtdll, XOR("NtWriteVirtualMemory"));
	if (!WinAPITable->NtWriteVirtualMemory) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtWriteVirtualMemory bind fail!");
		return false;
	}
	WinAPITable->NtResumeThread = (lpNtResumeThread)GetProcAddress(WinModuleTable->hNtdll, XOR("NtResumeThread"));
	if (!WinAPITable->NtResumeThread) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtResumeThread bind fail!");
		return false;
	}
	WinAPITable->NtTerminateProcess = (lpNtTerminateProcess)GetProcAddress(WinModuleTable->hNtdll, XOR("NtTerminateProcess"));
	if (!WinAPITable->NtTerminateProcess) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtTerminateProcess bind fail!");
		return false;
	}
	WinAPITable->RtlAdjustPrivilege = (lpRtlAdjustPrivilege)GetProcAddress(WinModuleTable->hNtdll, XOR("RtlAdjustPrivilege"));
	if (!WinAPITable->RtlAdjustPrivilege) {
		DEBUG_LOG(LL_CRI, "WinAPITable->RtlAdjustPrivilege bind fail!");
		return false;
	}
	WinAPITable->NtQueryInformationProcess = (lpNtQueryInformationProcess)GetProcAddress(WinModuleTable->hNtdll, XOR("NtQueryInformationProcess"));
	if (!WinAPITable->NtQueryInformationProcess) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtQueryInformationProcess bind fail!");
		return false;
	}
	WinAPITable->ZwSetInformationThread = (lpZwSetInformationThread)GetProcAddress(WinModuleTable->hNtdll, XOR("ZwSetInformationThread"));
	if (!WinAPITable->ZwSetInformationThread) {
		DEBUG_LOG(LL_CRI, "WinAPITable->ZwSetInformationThread bind fail!");
		return false;
	}
	WinAPITable->NtQueryVirtualMemory = (lpNtQueryVirtualMemory)GetProcAddress(WinModuleTable->hNtdll, XOR("NtQueryVirtualMemory"));
	if (!WinAPITable->NtQueryVirtualMemory) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtQueryVirtualMemory bind fail!");
		return false;
	}
	WinAPITable->NtAdjustPrivilegesToken = (lpNtAdjustPrivilegesToken)GetProcAddress(WinModuleTable->hNtdll, XOR("NtAdjustPrivilegesToken"));
	if (!WinAPITable->NtAdjustPrivilegesToken) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtAdjustPrivilegesToken bind fail!");
		return false;
	}
	WinAPITable->NtOpenProcessToken = (lpNtOpenProcessToken)GetProcAddress(WinModuleTable->hNtdll, XOR("NtOpenProcessToken"));
	if (!WinAPITable->NtOpenProcessToken) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtOpenProcessToken bind fail!");
		return false;
	}
	WinAPITable->RtlGetVersion = (lpRtlGetVersion)GetProcAddress(WinModuleTable->hNtdll, XOR("RtlGetVersion"));
	if (!WinAPITable->RtlGetVersion) {
		DEBUG_LOG(LL_CRI, "WinAPITable->RtlGetVersion bind fail!");
		return false;
	}
	if (IsWindows8OrGreater())
	{
		WinAPITable->SetProcessMitigationPolicy = (lpSetProcessMitigationPolicy)GetProcAddress(WinModuleTable->hKernel32, XOR("SetProcessMitigationPolicy"));
		if (!WinAPITable->SetProcessMitigationPolicy) {
			DEBUG_LOG(LL_CRI, "WinAPITable->SetProcessMitigationPolicy bind fail!");
			return false;
		}
	}
	if (IsWindowsVistaOrGreater())
	{
		WinAPITable->CsrGetProcessId = (lpCsrGetProcessId)GetProcAddress(WinModuleTable->hNtdll, XOR("CsrGetProcessId"));
		if (!WinAPITable->CsrGetProcessId) {
			DEBUG_LOG(LL_CRI, "WinAPITable->CsrGetProcessId bind fail!");
			return false;
		}
	}
	WinAPITable->NtSetInformationDebugObject = (lpNtSetInformationDebugObject)GetProcAddress(WinModuleTable->hNtdll, XOR("NtSetInformationDebugObject"));
	if (!WinAPITable->NtSetInformationDebugObject) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtSetInformationDebugObject bind fail!");
		return false;
	}
	WinAPITable->NtRemoveProcessDebug = (lpNtRemoveProcessDebug)GetProcAddress(WinModuleTable->hNtdll, XOR("NtRemoveProcessDebug"));
	if (!WinAPITable->NtRemoveProcessDebug) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtRemoveProcessDebug bind fail!");
		return false;
	}
	WinAPITable->NtSuspendProcess = (lpNtSuspendProcess)GetProcAddress(WinModuleTable->hNtdll, XOR("NtSuspendProcess"));
	if (!WinAPITable->NtSuspendProcess) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtSuspendProcess bind fail!");
		return false;
	}
	WinAPITable->NtResumeProcess = (lpNtResumeProcess)GetProcAddress(WinModuleTable->hNtdll, XOR("NtResumeProcess"));
	if (!WinAPITable->NtResumeProcess) {
		DEBUG_LOG(LL_CRI, "WinAPITable->NtResumeProcess bind fail!");
		return false;
	}
	WinAPITable->RtlInitUnicodeString = (lpRtlInitUnicodeString)GetProcAddress(WinModuleTable->hNtdll, XOR("RtlInitUnicodeString"));
	if (!WinAPITable->RtlInitUnicodeString) {
		DEBUG_LOG(LL_CRI, "WinAPITable->RtlInitUnicodeString bind fail!");
		return false;
	}
	WinAPITable->LdrLoadDll = (lpLdrLoadDll)GetProcAddress(WinModuleTable->hNtdll, XOR("LdrLoadDll"));
	if (!WinAPITable->LdrLoadDll) {
		DEBUG_LOG(LL_CRI, "WinAPITable->LdrLoadDll bind fail!");
		return false;
	}

	return true;
}


void CDynamicWinapi::Initialize()
{
	WinModuleTable = (PWINAPI_MODULE_TABLE)malloc(sizeof(WINAPI_MODULE_TABLE));
	WinAPITable = (PWINAPI_API_TABLE)malloc(sizeof(WINAPI_API_TABLE));

	BindModules();
	BindAPIs();

	m_bHasInitialized = true;
}

void CDynamicWinapi::Finalize()
{
	if (WinModuleTable) {
		free(WinModuleTable);
		WinModuleTable = nullptr;
	}
	if (WinAPITable) {
		free(WinAPITable);
		WinAPITable = nullptr;
	}

	m_bHasInitialized = false;
}

bool CDynamicWinapi::HasInitialized()
{
	return m_bHasInitialized;
}

