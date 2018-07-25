#include "HaxEngine.h"
#include "main.h"
#include "SafeHandle.h"
#include "DynamicWinAPI.h"

CSafeProcessHandle::CSafeProcessHandle(DWORD dwProcessId) :
	m_dwTargetProcessId(dwProcessId)
{
	ClearVeriables();
}
CSafeProcessHandle::~CSafeProcessHandle()
{
	m_dwTargetProcessId = 0;

	ClearVeriables();
}

void CSafeProcessHandle::ClearVeriables()
{
	CloseUselessHandles();

	if (IS_VALID_HANDLE(m_hTargetProcessHandle))
		CloseHandle(m_hTargetProcessHandle);
	m_hTargetProcessHandle = INVALID_HANDLE_VALUE;
}


HANDLE CSafeProcessHandle::CreateProcessHandle()
{
	//DWORD dwExitCode = 0;
	HANDLE hCurr = nullptr;

	while (WinAPITable->NtGetNextProcess(hCurr, MAXIMUM_ALLOWED, 0, 0, &hCurr) == STATUS_SUCCESS)
	{
		//if (!GetExitCodeProcess(hCurr, &dwExitCode) || dwExitCode != STILL_ACTIVE)
		//	continue;

		if (GetProcessId(hCurr) == m_dwTargetProcessId)
			m_hTargetProcessHandle = hCurr;
		else
			m_vHandleList.push_back(hCurr);
	}

	CloseUselessHandles();
	return m_hTargetProcessHandle;
}

void CSafeProcessHandle::CloseUselessHandles()
{
	for (size_t i = 0; i < m_vHandleList.size(); i++)
	{
		__try { CloseHandle(m_vHandleList[i]); }
		__except (1) { }
	}
	m_vHandleList.clear();
}

