#include "HaxEngine.h"
#include "main.h"
#include "MiniDump.h"
#include "Defines.h"

// Custom minidump callback 
BOOL CALLBACK MyMiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
	BOOL bRet = FALSE;

	// Check parameters 
	if (!pInput)
		return FALSE;

	if (!pOutput)
		return FALSE;

	// Process the callbacks 
	switch (pInput->CallbackType)
	{
	case IncludeModuleCallback:
	{
		// Include the module into the dump 
		bRet = TRUE;
	}
	break;

	case IncludeThreadCallback:
	{
		// Include the thread into the dump 
		bRet = TRUE;
	}
	break;

	case ModuleCallback:
	{
		// Does the module have ModuleReferencedByMemory flag set ? 
		if (!(pOutput->ModuleWriteFlags & ModuleReferencedByMemory)) {
			// No, it does not - exclude it 
			// wprintf(L"Excluding module: %s \n", pInput->Module.FullPath);
			pOutput->ModuleWriteFlags &= (~ModuleWriteModule);
		}
		bRet = TRUE;
	}
	break;

	case ThreadCallback:
	{
		// Include all thread information into the minidump 
		bRet = TRUE;
	}
	break;

	case ThreadExCallback:
	{
		// Include this information 
		bRet = TRUE;
	}
	break;

	case MemoryCallback:
	{
		// We do not include any information here -> return FALSE 
		bRet = FALSE;
	}
	break;

	case CancelCallback:
		break;
	}

	return bRet;
}

void CreateMiniDump(EXCEPTION_POINTERS* pep)
{
	time_t t;
	time(&t);
	struct tm *tinfo;
	tinfo = localtime(&t);

	WCHAR __warn[] = { L'H', L'a', L'x', L'D', L'u', L'm', L'p', L'%', L'Y', L'%', L'm', L'%', L'd', L'_', L'%', L'H', L'%', L'M', L'%', L'S', L'.', L'd', L'm', L'p', L'\0' }; // HaxDump%Y%m%d_%H%M%S.dmp
	wchar_t dump_name[128];
	wcsftime(dump_name, 128, __warn, tinfo);

	// file format MiniDump[YYYYMMDD][HH_MM_SEC]
	HANDLE hFile = CreateFileW(dump_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hFile || hFile == INVALID_HANDLE_VALUE) {
		DEBUG_LOG(LL_SYS, "Exception dump file is not created. Error code: %u", GetLastError());
		return;
	}

	// Create the minidump 
	MINIDUMP_EXCEPTION_INFORMATION mdei;
	mdei.ThreadId = GetCurrentThreadId();
	mdei.ExceptionPointers = pep;
	mdei.ClientPointers = FALSE;

	MINIDUMP_CALLBACK_INFORMATION mci;
	mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback;
	mci.CallbackParam = 0;

	MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);

	BOOL rv = MiniDumpWriteDump(NtCurrentProcess, GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci);
	if (!rv) {
		DEBUG_LOG(LL_SYS, "Exception dump is not created.Error code : %u", GetLastError());
	}
	else {
		DEBUG_LOG(LL_SYS, "Exception dump successfully created.");
	}

	// Close the file 
	CloseHandle(hFile);
}

LONG WINAPI ExceptionFilterSeh(EXCEPTION_POINTERS* pExceptionInfo)
{
	if (pExceptionInfo && pExceptionInfo->ExceptionRecord)
	{
		if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
		{
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CreateMiniDump, pExceptionInfo, 0, NULL);
			if (IS_VALID_HANDLE(hThread)) {
				WaitForSingleObject(hThread, INFINITE);
				CloseHandle(hThread);
			}
		}
		else {
			CreateMiniDump(pExceptionInfo);
		}
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

void CMiniDump::InitMiniDump()
{
	if (SetUnhandledExceptionFilter(ExceptionFilterSeh)) {
		DEBUG_LOG(LL_SYS, "Mini dump generator Exception handler is succesfully created!");
	}
	else {
		DEBUG_LOG(LL_ERR, "Mini dump generator Exception handler is NOT created!");
	}
}

