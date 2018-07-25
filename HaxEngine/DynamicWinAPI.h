#pragma once
#include "main.h"
#include "WinAPI_Typedefs.h"

typedef struct _WINAPI_MODULE_TABLE
{
	HMODULE hBaseModule;
	HMODULE hKernel32;
	HMODULE hNtdll;
	HMODULE hUser32;
	HMODULE hPsapi;
	HMODULE hDbghelp;
}WINAPI_MODULE_TABLE, *PWINAPI_MODULE_TABLE;
extern WINAPI_MODULE_TABLE* WinModuleTable;


typedef struct _WINAPI_API_TABLE
{
	lpIsDebuggerPresent IsDebuggerPresent;
	lpSleep Sleep;
	lpNtGetNextThread NtGetNextThread;
	lpNtGetNextProcess NtGetNextProcess;
	lpNtQuerySystemInformation NtQuerySystemInformation;
	lpNtDuplicateObject NtDuplicateObject;
	lpNtClose NtClose;
	lpNtGetContextThread NtGetContextThread;
	lpNtSetContextThread NtSetContextThread;
	lpNtReadVirtualMemory NtReadVirtualMemory;
	lpNtWaitForSingleObject NtWaitForSingleObject;
	lpNtUnmapViewOfSection NtUnmapViewOfSection;
	lpNtWriteVirtualMemory NtWriteVirtualMemory;
	lpNtResumeThread NtResumeThread;
	lpNtTerminateProcess NtTerminateProcess;
	lpRtlAdjustPrivilege RtlAdjustPrivilege;
	lpNtQueryInformationProcess NtQueryInformationProcess;
	lpZwSetInformationThread ZwSetInformationThread;
	lpNtQueryVirtualMemory NtQueryVirtualMemory;
	lpNtAdjustPrivilegesToken NtAdjustPrivilegesToken;
	lpNtOpenProcessToken NtOpenProcessToken;
	lpRtlGetVersion RtlGetVersion;
	lpSetProcessMitigationPolicy SetProcessMitigationPolicy;
	lpCsrGetProcessId CsrGetProcessId;
	lpNtSetInformationDebugObject NtSetInformationDebugObject;
	lpNtRemoveProcessDebug NtRemoveProcessDebug;
	lpNtSuspendProcess NtSuspendProcess;
	lpNtResumeProcess NtResumeProcess;
	lpLdrLoadDll LdrLoadDll;
	lpRtlInitUnicodeString RtlInitUnicodeString;
	lpNtCreateThreadEx NtCreateThreadEx;
}WINAPI_API_TABLE, *PWINAPI_API_TABLE;
extern WINAPI_API_TABLE* WinAPITable;

class CDynamicWinapi : public std::enable_shared_from_this<CDynamicWinapi>
{
	public:
		CDynamicWinapi();
		~CDynamicWinapi();

		void Initialize();
		bool HasInitialized();
		void Finalize();

	protected:
		bool BindModules();
		bool BindAPIs();

	private:
		bool m_bHasInitialized;
};

