#include "HaxEngine.h"
#include "main.h"
#include "MemoryHelper.h"
#include "Defines.h"
#include "WinAPI_Typedefs.h"
#include "VersionHelpers.h"

// TODO: SafeMemory class, https://www.unknowncheats.me/forum/anti-cheat-bypass/245740-umpmlib-wrapper-dynamic-offset-scan-wrapper.html

CMemoryHelper::CMemoryHelper(HANDLE hProcess) :
	m_hProcess(hProcess)
{
}

LPVOID CMemoryHelper::Alloc(std::size_t size, DWORD dwProtection, LPVOID pPreferredLoc)
{
	return VirtualAllocEx(m_hProcess, pPreferredLoc, size, MEM_COMMIT | MEM_RESERVE, dwProtection);
}

LPVOID CMemoryHelper::Commit(LPVOID data, std::size_t size)
{
	LPVOID AllocatedPointer = Alloc(size);
	if (!AllocatedPointer)
		return nullptr;

	if (WriteProcessMemory(m_hProcess, AllocatedPointer, data, size, NULL))
		return AllocatedPointer;

	Free(AllocatedPointer, size);
	return nullptr;
}

bool CMemoryHelper::Protect(LPVOID address, std::size_t size, DWORD dwProtectFlag)
{
	DWORD dwOldProtect = 0;
	return (VirtualProtectEx(m_hProcess, address, size, dwProtectFlag, &dwOldProtect) == TRUE);
}

bool CMemoryHelper::Free(LPVOID address, std::size_t size)
{
	// return (VirtualFreeEx(m_hProcess, address, size, MEM_RELEASE) == TRUE);
	return (VirtualFreeEx(m_hProcess, address, NULL, MEM_RELEASE) == TRUE);
}

bool CMemoryHelper::ReadAddress(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwReadByteCount = 0;
	if (!ReadProcessMemory(m_hProcess, address, buffer, size, &dwReadByteCount))
		dwReadByteCount = 0;
	return dwReadByteCount != 0;
}
bool CMemoryHelper::ReadAddressSafe(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwReadByteCount = 0;
	__try {
		if (!ReadProcessMemory(m_hProcess, address, buffer, size, &dwReadByteCount))
			dwReadByteCount = 0;
	}
	__except(1) { }
	return (dwReadByteCount && dwReadByteCount == size);
}

bool CMemoryHelper::WriteAddress(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwWriteByteCount = 0;
	if (!WriteProcessMemory(m_hProcess, address, buffer, size, &dwWriteByteCount))
		dwWriteByteCount = 0;
	return (dwWriteByteCount != 0);
}
bool CMemoryHelper::WriteAddressSafe(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwWriteByteCount = 0;
	__try {
		if (!WriteProcessMemory(m_hProcess, address, buffer, size, &dwWriteByteCount))
			dwWriteByteCount = 0;
	}
	__except (1) { }
	return (dwWriteByteCount && dwWriteByteCount == size);
}

bool CMemoryHelper::Patch(LPVOID address, LPVOID buffer, std::size_t size)
{
	DWORD dwOldProtect = 0;
	if (VirtualProtectEx(m_hProcess, address, size, PAGE_EXECUTE_READWRITE, &dwOldProtect) == FALSE)
		return false;

	if (!WriteAddressSafe(address, buffer, size))
		return false;

	if (VirtualProtectEx(m_hProcess, address, size, dwOldProtect, &dwOldProtect) == FALSE)
		return false;

	return true;
}

bool CMemoryHelper::IsSafePage(LPVOID address)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQueryEx(m_hProcess, address, &mbi, sizeof(mbi)))
		return false;

	if (!(mbi.State & MEM_COMMIT))
		return false;

	if (mbi.State & MEM_RELEASE)
		return false;

	if (mbi.Protect == PAGE_NOACCESS || mbi.Protect & PAGE_GUARD)
		return false;

	if (mbi.Protect != PAGE_READONLY && mbi.Protect != PAGE_READWRITE && mbi.Protect != PAGE_EXECUTE_READ && mbi.Protect != PAGE_EXECUTE_READWRITE)
		return false;

	return true;
}


// Remote
HMODULE CMemoryHelper::GetModule(const char* c_szModule)
{
	auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(m_hProcess));
	if (!IS_VALID_HANDLE(hSnapshot))
		return nullptr;

	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &modEntry)) {
		do {
			if (strstr(modEntry.szModule, c_szModule)) {
				CloseHandle(hSnapshot);
				return modEntry.hModule;
			}
		} while (Module32Next(hSnapshot, &modEntry));
	}

	CloseHandle(hSnapshot);
	return nullptr;
}

// TODO: Remote GetProcAddress

HANDLE CMemoryHelper::CreateThread(LPTHREAD_START_ROUTINE lpThreadRoutine, LPVOID lpParam, DWORD dwFlag)
{
#ifdef _DEBUG
	dwFlag = 0;
#endif
	HANDLE hThread = INVALID_HANDLE_VALUE;

	if (IsWindowsVistaOrGreater())
	{
		auto ntStat = WinAPITable->NtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, 0, m_hProcess, lpThreadRoutine, lpParam, dwFlag, 0, 0, 0, 0);
		if (!NT_SUCCESS(ntStat) || !IS_VALID_HANDLE(hThread)) {
			DEBUG_LOG(LL_ERR, "NtCreateThreadEx fail! NTSTATUS: %p\n", (void*)ntStat);
			return INVALID_HANDLE_VALUE;
		}
	}
	else /* XP Only */
	{
		hThread = CreateRemoteThread(m_hProcess, NULL, NULL, lpThreadRoutine, lpParam, NULL, NULL); // TODO: NtCreateThread
		if (!IS_VALID_HANDLE(hThread)) {
			DEBUG_LOG(LL_ERR, "CreateRemoteThread fail! Error: %u\n", GetLastError());
			return INVALID_HANDLE_VALUE;
		}

		if (dwFlag) {
			auto ntStat = WinAPITable->ZwSetInformationThread(hThread, ThreadHideFromDebugger, 0, 0);
			if (!NT_SUCCESS(ntStat)) {
				DEBUG_LOG(LL_ERR, "ZwSetInformationThread fail! NTSTATUS: %p\n", (void*)ntStat);
				TerminateThread(hThread, EXIT_SUCCESS);
				CloseHandle(hThread);
				return INVALID_HANDLE_VALUE;
			}
		}
	}

	return hThread;
}

bool CMemoryHelper::LoadShellCode(LPVOID lpShellCode, std::size_t uiSize, LPVOID lpParam, std::size_t uiParamSize, DWORD dwTimeoutInterval)
{
	auto lpAddr = lpShellCode;
	if (uiSize != -1) { /* if size == (DWORD)-1 use already commited address */
		lpAddr = Commit(lpShellCode, uiSize);
	}
	if (!lpAddr) {
		DEBUG_LOG(LL_ERR, "Shell code commit can not created! Error code: %u", GetLastError());
		return false;
	}

	LPVOID lpCommitedParam = nullptr;
	if (lpParam) {
		lpCommitedParam = Commit(lpParam, uiParamSize);
		if (!lpCommitedParam) {
			DEBUG_LOG(LL_ERR, "Shell code param commit can not created! Error code: %u", GetLastError());
			return false;
		}
	}

	auto hThread = CreateThread(reinterpret_cast<LPTHREAD_START_ROUTINE>(lpAddr), lpCommitedParam, 0x00000004);
	if (!IS_VALID_HANDLE(hThread)) {
		DEBUG_LOG(LL_ERR, "Shell code thread can not created! Error code: %u", GetLastError());
		return false;
	}

	auto dwWaitRet = WaitForSingleObject(hThread, dwTimeoutInterval);
	if (dwWaitRet != WAIT_TIMEOUT) {
		DEBUG_LOG(LL_ERR, "Shell code wait fail! Error code: %u Wait ret: %u", GetLastError(), dwWaitRet);
		return false;
	}

	if (uiSize != -1 && !Free(lpAddr, uiSize)) {
		DEBUG_LOG(LL_ERR, "Shell code memory deallocate fail! Error code: %u", GetLastError());
		return false;
	}

	if (lpParam && !Free(lpCommitedParam, uiParamSize)) {
		DEBUG_LOG(LL_ERR, "Shell code param deallocate fail! Error code: %u", GetLastError());
		return false;
	}

	return true;
}


// DLL Injection
typedef struct _REMOTE_THREAD_PARAM
{
	// _OutputDebugStringA OutputDebugStringA;
	lpLdrLoadDll		LdrLoadDll;
	PUNICODE_STRING		unicodeStrDLLName;
}REMOTE_THREAD_PARAM, *PREMOTE_THREAD_PARAM;

DWORD WINAPI RemoteThreadRoutine(LPVOID lpParam)
{
	auto param = (PREMOTE_THREAD_PARAM)lpParam;

	ULONG flags = LOAD_WITH_ALTERED_SEARCH_PATH;

	HMODULE hModule;
	NTSTATUS ntStatus = param->LdrLoadDll(NULL, &flags, param->unicodeStrDLLName, &hModule);
	/*
	std::string szDebugText = "LdrLoadDll result: " + ntStatus;
	param->OutputDebugStringA(szDebugText.c_str());
	*/
	return 0;
}

static int UselessFunction()
{
	return 0;
}


bool CMemoryHelper::LoadDLL(const std::string & szPath)
{
	// Veriables
	DWORD dwThreadSize;
	PREMOTE_THREAD_PARAM localThreadParam = nullptr;
	bool bShellCodeRet;

	// Output
	auto bRet = false;

	// DLL Info
	std::wstring wszPath(szPath.begin(), szPath.end());
	auto c_wszFileName = wszPath.c_str();
	std::size_t uiSize = wcslen(c_wszFileName) + 1;

	auto uniStrDllName = new UNICODE_STRING();
	WinAPITable->RtlInitUnicodeString(uniStrDllName, c_wszFileName);

	// Check dll
	if (g_haxApp->GetFunctionsInstance()->IsFileExist(szPath) == false) {
		DEBUG_LOG(LL_ERR, "File is not exist!");
		goto clean;
	}

	// Thread shell size
	dwThreadSize = (DWORD)UselessFunction - (DWORD)RemoteThreadRoutine;
	DEBUG_LOG(LL_SYS, "Thread shellcode size: %u\n", dwThreadSize);
	if (dwThreadSize > 100) {
		DEBUG_LOG(LL_ERR, "Unknown thread size!");
		goto clean;
	}

	// Thread param
	localThreadParam = reinterpret_cast<PREMOTE_THREAD_PARAM>(VirtualAlloc(NULL, sizeof(REMOTE_THREAD_PARAM), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (!localThreadParam) {
		DEBUG_LOG(LL_ERR, "VirtualAlloc fail! Error code: %u\n", GetLastError());
		goto clean;
	}

	// localThreadParam->OutputDebugStringA = OutputDebugStringA;
	localThreadParam->LdrLoadDll			= WinAPITable->LdrLoadDll;
	localThreadParam->unicodeStrDLLName		= uniStrDllName;

	// Load shell code
	bShellCodeRet = LoadShellCode(RemoteThreadRoutine, dwThreadSize, localThreadParam, sizeof(REMOTE_THREAD_PARAM), 5000);
	if (!bShellCodeRet) {
		DEBUG_LOG(LL_ERR, "Shellcode load fail! Error code: %u\n", GetLastError());
		goto clean;
	}

	// Mark as completed
	bRet = true;

	// Clean
clean:
	if (uniStrDllName)
		delete uniStrDllName;
	uniStrDllName = nullptr;

	if (localThreadParam)
		VirtualFree(localThreadParam, sizeof(REMOTE_THREAD_PARAM), MEM_FREE);

	return bRet;
}

