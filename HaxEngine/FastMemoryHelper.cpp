#include "HaxEngine.h"
#include "main.h"
#include "FastMemoryHelper.h"
#include "MemoryHelper.h"
#include "ProcessHelper.h"
#include "DynamicWinAPI.h"

#if 0 
/// POC
auto fastMemHelper = std::make_unique<CFastMemory>(m_dwProcessId, FAST_MEMORY_WITHOUT_SUSPEND);
fastMemHelper->Initialize();

// Mem works..
auto dwBaseAddr = fastMemHelper->GetProcessHelperInstance()->GetBaseAddress();
if (!dwBaseAddr) {
	printf("Can not find target process base address!\n");
	return;
}

fastMemHelper->Finalize();
#endif

CFastMemory::CFastMemory(DWORD dwProcessId, bool bIsSuspend) :
	m_dwProcessId(dwProcessId), m_hProcessHandle(INVALID_HANDLE_VALUE), m_bIsSuspend(bIsSuspend)
{
	//printf("CFastMemory::CFastMemory\n");
	m_cProcHelper = std::make_shared<CProcessHelper>(dwProcessId);
}

CFastMemory::~CFastMemory()
{
	//printf("CFastMemory::~CFastMemory\n");
	if (IS_VALID_HANDLE(m_hProcessHandle))
		CloseHandle(m_hProcessHandle);
	m_hProcessHandle = INVALID_HANDLE_VALUE;

	m_dwProcessId = 0;

	m_bIsSuspend = false;
}


bool CFastMemory::Initialize()
{
	m_cProcHelper->SetOpenType(EOPenTypes::OPEN_STANDART);
	if (false == m_cProcHelper->OpenTarget())
		return false;

	m_hProcessHandle = m_cProcHelper->GetHandle();
	if (!IS_VALID_HANDLE(m_hProcessHandle))
		return false;

	if (m_bIsSuspend)
		if (!NT_SUCCESS(WinAPITable->NtSuspendProcess(m_hProcessHandle)))
			return false;

	return true;
}

bool CFastMemory::Finalize()
{
	if (m_bIsSuspend)
		if (!NT_SUCCESS(WinAPITable->NtResumeProcess(m_hProcessHandle)))
			return false;

	m_cProcHelper->Close();
	return true;
}

