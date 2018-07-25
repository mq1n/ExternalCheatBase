#include "HaxEngine.h"
#include "main.h"
#include "Antis.h"
#include "DynamicWinAPI.h"

void CAntis::SetFakeImageSize()
{
	__asm
	{
		mov eax, fs:[0x30]				// PEB
		mov eax, [eax + 0x0c]			 // PEB_LDR_DATA
		mov eax, [eax + 0x0c]			// InOrderModuleList
		mov dword ptr[eax + 20h], 20000h // SizeOfImage
	}
}

void CAntis::CrashDebugger()
{
	CHAR OLLY_CRASH_TEXT[] = { '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', 0x0 }; //%s%s%s%s%s%s%s%s%s%s%s
	OutputDebugStringA(TEXT(OLLY_CRASH_TEXT));
}

bool CAntis::DetachFromDebuggerProcess()
{
	int iErrorCode = 0;
	ULONG dwFlags = 0;
	HANDLE hDebugObject = 0;

	if (NT_SUCCESS(WinAPITable->NtQueryInformationProcess(NtCurrentProcess, ProcessDebugObjectHandle, &hDebugObject, sizeof(HANDLE), NULL)))
	{
		if (!NT_SUCCESS(WinAPITable->NtSetInformationDebugObject(hDebugObject, DebugObjectFlags, &dwFlags, sizeof(ULONG), NULL)))
			iErrorCode = 2;

		if (!NT_SUCCESS(WinAPITable->NtRemoveProcessDebug(NtCurrentProcess, hDebugObject)))
			iErrorCode = 3;

		if (!NT_SUCCESS(WinAPITable->NtClose(hDebugObject)))
			iErrorCode = 4;
	}
	else
		iErrorCode = 1;

	return (iErrorCode == 0);
}


bool CAntis::CheckSeDebugPriv()
{
	auto dwCsrssPid = WinAPITable->CsrGetProcessId();
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwCsrssPid);

	if (IS_VALID_HANDLE(hProcess)) {
		CloseHandle(hProcess);
		return true;
	}

	return false;
}

bool CAntis::CheckCloseProtectedHandle()
{
	CHAR __kernel4[] = { 'k', 'e', 'r', 'n', 'e', 'l', '4',  0x0 }; // kernel4
	HANDLE hMutex = CreateMutexA(NULL, FALSE, __kernel4);

	if (IS_VALID_HANDLE(hMutex))
	{
		if (SetHandleInformation(hMutex, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE))
		{
			__try {
				CloseHandle(hMutex);
			}

			__except (HANDLE_FLAG_PROTECT_FROM_CLOSE) {
				return true;
			}
		}
	}

	return false;
}

