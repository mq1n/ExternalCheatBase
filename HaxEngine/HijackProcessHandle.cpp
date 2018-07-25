#include "HaxEngine.h"
#include "main.h"
#include "Defines.h"
#include "HijackProcessHandle.h"
#include "DynamicWinAPI.h"

CHijackProcessHandle::CHijackProcessHandle(DWORD dwProcessId) :
	m_dwTargetProcessId(dwProcessId)
{
	ClearVeriables();
}
CHijackProcessHandle::~CHijackProcessHandle()
{
	ClearVeriables();
}

void CHijackProcessHandle::ClearVeriables()
{
	m_dwTargetProcessId = 0;

	if (IS_VALID_HANDLE(m_hTargetProcessHandle))
		CloseHandle(m_hTargetProcessHandle);
	m_hTargetProcessHandle = INVALID_HANDLE_VALUE;
}


HANDLE CHijackProcessHandle::CreateProcessHandle()
{
	HANDLE processHandle = nullptr;
	NTSTATUS ntStat = 0;

	ULONG handleInfoSize = 0x10000;
	auto handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);
	ZeroMemory(handleInfo, handleInfoSize);

	while ((ntStat = WinAPITable->NtQuerySystemInformation(SystemHandleInformation, handleInfo, handleInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		handleInfoSize *= 2;
		handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize);
	}

	if (!NT_SUCCESS(ntStat)) {
		free(handleInfo);
		DEBUG_LOG(LL_ERR, "NtQuerySystemInformation fail! Ntstatus: %p", (void*)ntStat);
		return INVALID_HANDLE_VALUE;
	}

	for (ULONG i = 0; i < handleInfo->HandleCount; i++)
	{
		auto handle = handleInfo->Handles[i];
		HANDLE dupHandle = NULL;

		if (handle.ObjectTypeNumber != 0x5 && handle.ObjectTypeNumber != 0x7) /* Just process handles */
			continue;


		if (IS_VALID_HANDLE(processHandle))
			CloseHandle(processHandle);

		processHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, handle.ProcessId);
		if (!processHandle || processHandle == INVALID_HANDLE_VALUE)
			continue;


		ntStat = WinAPITable->NtDuplicateObject(processHandle, (HANDLE)handle.Handle, NtCurrentProcess, &dupHandle, PROCESS_ALL_ACCESS, 0, 0);
		if (!NT_SUCCESS(ntStat)) {
			DEBUG_LOG(LL_ERR, "NtDuplicateObject fail! Ntstatus: %p", (void*)ntStat);
			continue;
		}


		if (GetProcessId(dupHandle) != m_dwTargetProcessId) {
			if (IS_VALID_HANDLE(dupHandle))
				CloseHandle(dupHandle);
			continue;
		}


		m_hTargetProcessHandle = dupHandle;
		DEBUG_LOG(LL_SYS, "Available handle found on: %u", handle.ProcessId);
		break;
	}

	free(handleInfo);
	if (IS_VALID_HANDLE(processHandle))
		CloseHandle(processHandle);

	return m_hTargetProcessHandle;
}

