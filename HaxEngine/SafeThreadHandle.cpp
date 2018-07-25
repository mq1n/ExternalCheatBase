#include "HaxEngine.h"
#include "main.h"
#include "SafeHandle.h"
#include "DynamicWinAPI.h"

CSafeThreadHandle::CSafeThreadHandle(HANDLE hOwnerProcess, DWORD dwThreadId) :
	m_dwTargetThreadId(dwThreadId), m_hOwnerProcessHandle(hOwnerProcess)
{
	ClearVeriables();
}
CSafeThreadHandle::~CSafeThreadHandle()
{
	ClearVeriables();
}

void CSafeThreadHandle::ClearVeriables()
{
	CloseUselessHandles();
	m_dwTargetThreadId = 0;
	m_hOwnerProcessHandle = INVALID_HANDLE_VALUE;

	if (IS_VALID_HANDLE(m_hTargetThreadHandle))
		CloseHandle(m_hTargetThreadHandle);
	m_hTargetThreadHandle = INVALID_HANDLE_VALUE;
}


HANDLE CSafeThreadHandle::CreateThreadHandle()
{
	HANDLE hCurr = nullptr;

	while (WinAPITable->NtGetNextThread(m_hOwnerProcessHandle, hCurr, MAXIMUM_ALLOWED, 0, 0, &hCurr) == STATUS_SUCCESS)
	{
		if (GetThreadId(hCurr) == m_dwTargetThreadId) {
			m_hTargetThreadHandle = hCurr;
			break;
		}
		else {
			m_vHandleList.push_back(hCurr);
		}
	}

	CloseUselessHandles();
	return m_hTargetThreadHandle;
}

void CSafeThreadHandle::CloseUselessHandles()
{
	for (std::size_t i = 0; i < m_vHandleList.size(); i++)
	{
		__try { CloseHandle(m_vHandleList[i]); }
		__except (1) { }
	}
	m_vHandleList.clear();
}

