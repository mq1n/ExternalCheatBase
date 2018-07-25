#include "HaxEngine.h"
#include "main.h"
#include "XOR.h"
#include "DynamicWinAPI.h"
#include "ProcessName.h"
#include "Antis.h"

static void SafeCloseHandle(HANDLE hProc)
{
	__try {
		if (IS_VALID_HANDLE(hProc))
			CloseHandle(hProc);
	}
	__except (1) { }
}

std::vector<DWORD> g_vWhiteList;
inline void BlockOpenedHandles()
{
	ULONG handleInfoSize = 0x10000;
	HANDLE hProcess = nullptr;
	HANDLE hOwnerProcHandle = nullptr;
	NTSTATUS ntStat = 0;

	auto handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);
	ZeroMemory(handleInfo, handleInfoSize);

	while ((ntStat = WinAPITable->NtQuerySystemInformation(SystemHandleInformation, handleInfo, handleInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		handleInfoSize *= 2;
		handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize);
	}

	if (!NT_SUCCESS(ntStat)) {
		free(handleInfo);
		return;
	}

	for (ULONG i = 0; i < handleInfo->HandleCount; i++)
	{
		auto handle = handleInfo->Handles[i];
		HANDLE dupHandle = nullptr;

		if (handle.ProcessId == GetCurrentProcessId())
			continue;

		if (handle.ProcessId < 5) /* System */
			continue;
		
		// TODO: Scan file handles for hax process file
		// (https://bitbucket.org/evolution536/crysearch-memory-scanner/src/fc589ad6584ca4830e9689c518c170d607aa56f2/ProcessUtil.cpp?at=master&fileviewer=file-view-default#ProcessUtil.cpp-206)

		if (handle.ObjectTypeNumber != 0x5 && handle.ObjectTypeNumber != 0x7) /* Just process handles */
			continue;

		if (IS_VALID_HANDLE(hProcess))
			SafeCloseHandle(hProcess);

		hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, handle.ProcessId);
		if (!IS_VALID_HANDLE(hProcess))
			continue;

		ntStat = WinAPITable->NtDuplicateObject(hProcess, (HANDLE)handle.Handle, NtCurrentProcess, &dupHandle, PROCESS_QUERY_INFORMATION, 0, 0);
		if (!NT_SUCCESS(ntStat)) {
			SafeCloseHandle(dupHandle);
			continue;
		}

		if (GetProcessId(dupHandle) != GetCurrentProcessId()) {
			SafeCloseHandle(dupHandle);
			continue;
		}

		if (std::find(g_vWhiteList.begin(), g_vWhiteList.end(), handle.ProcessId) != g_vWhiteList.end()) { 
			SafeCloseHandle(dupHandle);
			continue;
		}

		hOwnerProcHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)handle.ProcessId);
		if (!IS_VALID_HANDLE(hOwnerProcHandle)) {
			SafeCloseHandle(dupHandle);
			continue;
		}

		auto szOwnerProcName = CProcessName::GetProcessName(hOwnerProcHandle);
		if (szOwnerProcName.empty()) {
			SafeCloseHandle(dupHandle);
			SafeCloseHandle(hOwnerProcHandle);
			continue;
		}
		auto szLowerOwnerProcName = g_haxApp->GetFunctionsInstance()->szLower(szOwnerProcName);

		CHAR __csrssexe[] = { 'c', 's', 'r', 's', 's', 's','.', 'e', 'x', 'e', 0x0 }; // csrss.exe
		CHAR __svchostexe[] = { 's', 'v', 'c', 'h', 'o', 's', 't', '.', 'e', 'x', 'e', 0x0 }; // svchost.exe
		CHAR __lsassexe[] = { 'l', 's', 'a', 's', 's', '.', 'e', 'x', 'e', 0x0 }; // lsass.exe
		if (strstr(szLowerOwnerProcName.c_str(), __lsassexe) || strstr(szLowerOwnerProcName.c_str(), __svchostexe) ||
			strstr(szLowerOwnerProcName.c_str(), __csrssexe))
		{
			g_vWhiteList.push_back((DWORD)handle.ProcessId);
			SafeCloseHandle(hOwnerProcHandle);
			SafeCloseHandle(dupHandle);
			continue;
		}

#ifdef _DEBUG
		if (strstr(szLowerOwnerProcName.c_str(), XOR("conhost.exe")) || strstr(szLowerOwnerProcName.c_str(), XOR("devenv.exe")))
		{
			g_vWhiteList.push_back((DWORD)handle.ProcessId);
			SafeCloseHandle(hOwnerProcHandle);
			SafeCloseHandle(dupHandle);
			continue;
		}
#endif

		WinAPITable->NtDuplicateObject(hProcess, (HANDLE)handle.Handle, NULL, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);

		SafeCloseHandle(hOwnerProcHandle);
		SafeCloseHandle(dupHandle);
	}

	free(handleInfo);
	if (IS_VALID_HANDLE(hProcess))
		SafeCloseHandle(hProcess);
}


DWORD WINAPI InitializeBlockHandlesEx(LPVOID)
{
	while (1)
	{
		BlockOpenedHandles();
		Sleep(2000);
	}

	return 0;
}

bool CAntis::InitializeBlockHandles()
{
	auto hThread = CreateThread(nullptr, 0, InitializeBlockHandlesEx, nullptr, 0, nullptr);
	return (IS_VALID_HANDLE(hThread));
}

