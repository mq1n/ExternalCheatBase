#include "HaxEngine.h"
#include "main.h"
#include "ProcessHelper.h"
#include "Defines.h"
#include "ModuleHelper.h"
#include "SafeHandle.h"
#include "DynamicWinAPI.h"
#include "ProcessName.h"
#include "XOR.h"

CProcessHelper::CProcessHelper() : 
	m_cSafeProcessHandleHelper(nullptr), m_cHijackProcessHandleHelper(nullptr), m_dwProcessId(0), m_hProcessHandle(INVALID_HANDLE_VALUE),
	m_szProcessName(""), m_szClassName(""), m_szTitle(""), m_szProcessPath(""), m_bSuspended(false), m_bIsSonicHandle(false)
{
	m_openType = EOPenTypes::OPEN_STANDART;
	m_listModules.clear();
}
CProcessHelper::CProcessHelper(DWORD dwProcessId) : 
	m_cSafeProcessHandleHelper(nullptr), m_cHijackProcessHandleHelper(nullptr), m_dwProcessId(dwProcessId), m_hProcessHandle(INVALID_HANDLE_VALUE),
	m_szProcessName(""), m_szClassName(""), m_szTitle(""), m_szProcessPath(""), m_bSuspended(false), m_bIsSonicHandle(false)
{
	m_openType = EOPenTypes::OPEN_STANDART;
	m_listModules.clear();
}
CProcessHelper::CProcessHelper(const char* c_szProcessName) : 
	m_cSafeProcessHandleHelper(nullptr), m_cHijackProcessHandleHelper(nullptr), m_dwProcessId(0), m_hProcessHandle(INVALID_HANDLE_VALUE),
	m_szProcessName(c_szProcessName), m_szClassName(""), m_szTitle(""), m_szProcessPath(""), m_bSuspended(false), m_bIsSonicHandle(false)
{
	m_openType = EOPenTypes::OPEN_STANDART;
	m_listModules.clear();
}
CProcessHelper::CProcessHelper(const char* c_szClassName, const char* c_szTitle) :
	m_cSafeProcessHandleHelper(nullptr), m_cHijackProcessHandleHelper(nullptr), m_dwProcessId(0), m_hProcessHandle(INVALID_HANDLE_VALUE),
	m_szProcessName(""), m_szClassName(c_szClassName), m_szTitle(c_szTitle), m_szProcessPath(""), m_bSuspended(false), m_bIsSonicHandle(false)
{
	m_openType = EOPenTypes::OPEN_STANDART;
	m_listModules.clear();
}
CProcessHelper::CProcessHelper(const char* c_szPath, bool bSuspended) :
	m_cSafeProcessHandleHelper(nullptr), m_cHijackProcessHandleHelper(nullptr), m_dwProcessId(0), m_hProcessHandle(INVALID_HANDLE_VALUE),
	m_szProcessName(""), m_szClassName(""), m_szTitle(""), m_szProcessPath(c_szPath), m_bSuspended(bSuspended), m_bIsSonicHandle(false)
{
	m_openType = EOPenTypes::OPEN_STANDART;
	m_listModules.clear();
}

CProcessHelper::~CProcessHelper()
{
	Close();
}


void CProcessHelper::SetProcessId(DWORD dwProcessId)
{
	m_dwProcessId = dwProcessId;
}

void CProcessHelper::SetProcessName(const char* c_szProcessName)
{
	m_szProcessName = c_szProcessName;
}

void CProcessHelper::SetWindowInformations(const char* c_szClassName, const char* c_szTitle)
{
	m_szClassName	= c_szClassName;
	m_szTitle		= c_szTitle;
}

void CProcessHelper::SetProcessPath(const char * c_szProcessPath, bool bSuspended)
{
	m_szProcessPath = c_szProcessPath;
	m_bSuspended	= bSuspended;
}


void CProcessHelper::SetOpenType(EOPenTypes openType)
{
	m_openType = openType;
}

bool CProcessHelper::OpenTarget()
{
	if (m_dwProcessId)
		return OpenWithProcessId();
	else if (m_szProcessName.empty() == false)
		return OpenWithProcessName();
	else if (m_szClassName.empty() == false || m_szTitle.empty() == false)
		return OpenWithWindowInformations();
	else if (m_szProcessPath.empty() == false)
		return OpenWithCreateProcess(m_bSuspended);

	DEBUG_LOG(LL_ERR, "u wot m8?");
	return false;
}


bool CProcessHelper::OpenWithProcessId()
{
	// Check it (etw only)
	/* // TESTME
	auto vHandledProcesses = g_haxApp->GetHandledProcesses();
	for (auto & curProc : vHandledProcesses)
	{
		if (curProc->dwProcessId == m_dwProcessId)
		{
			if (curProc->haxController)
			{
				if (IS_VALID_HANDLE(curProc->haxController->GetProcessHandle()))
				{
					DEBUG_LOG(LL_SYS, "Sonic process open event detected! Pre-created handle switched.");
					m_hProcessHandle = curProc->haxController->GetProcessHandle();
					m_bIsSonicHandle = true;
					goto skip;
				}
			}
		}
	}
	*/

	// Common
	if (m_openType == EOPenTypes::OPEN_STANDART)
	{
		m_hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwProcessId); // TODO: Nt
	}

	else if (m_openType == EOPenTypes::OPEN_FORCE)
	{
		if (!m_cSafeProcessHandleHelper || !m_cSafeProcessHandleHelper.get())
			m_cSafeProcessHandleHelper = std::make_unique<CSafeProcessHandle>(m_dwProcessId);
		m_hProcessHandle = m_cSafeProcessHandleHelper->CreateProcessHandle();
	}

	else if (m_openType == EOPenTypes::OPEN_DUPLICATE_HANDLE)
	{
		if (!m_cHijackProcessHandleHelper || !m_cHijackProcessHandleHelper.get())
			m_cHijackProcessHandleHelper = std::make_unique<CHijackProcessHandle>(m_dwProcessId);
		m_hProcessHandle = m_cHijackProcessHandleHelper->CreateProcessHandle();
	}

	else
	{
		DEBUG_LOG(LL_CRI, "*** This open method not implemented yet *** ");
		abort();
		return false;
	}

//skip:
	if (IS_VALID_HANDLE(m_hProcessHandle)) {
		CreateModuleList();
		m_szProcessName = CProcessName::GetProcessName(m_hProcessHandle);
	}
	return IS_VALID_HANDLE(m_hProcessHandle);
}
bool CProcessHelper::OpenWithProcessName()
{
	auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!IS_VALID_HANDLE(hSnapshot)) return false;

	PROCESSENTRY32 pe = { 0 };
	memset(&pe, 0, sizeof(pe));
	pe.dwSize = sizeof(pe);

	do
	{
		if (!strcmp(pe.szExeFile, m_szProcessName.c_str()))
			m_dwProcessId = pe.th32ProcessID;

	} while (m_dwProcessId == 0 && Process32Next(hSnapshot, &pe));

	CloseHandle(hSnapshot);

	bool bWrapToOpenWithPid = OpenWithProcessId();
	return bWrapToOpenWithPid;
}
bool CProcessHelper::OpenWithWindowInformations()
{
	auto hWnd = FindWindowA(m_szClassName.c_str(), m_szTitle.c_str());
	if (!hWnd)
		return false;

	DWORD dwProcessId = 0;
	auto dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
	if (!dwThreadId || !dwProcessId)
		return false;

	m_dwProcessId = dwProcessId;

	bool bWrapToOpenWithPid = OpenWithProcessId();
	return bWrapToOpenWithPid;
}
bool CProcessHelper::OpenWithCreateProcess(bool bSuspended)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));

	GetStartupInfo(&si);

	auto dwAttrib = GetFileAttributesA(m_szProcessPath.c_str());
	if (dwAttrib == INVALID_FILE_ATTRIBUTES || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		return false;

	if (!CreateProcessA(m_szProcessPath.c_str(), NULL, NULL, NULL, FALSE, bSuspended ? CREATE_SUSPENDED : NULL, NULL, NULL, &si, &pi))
		return false;

	m_hProcessHandle = pi.hProcess;
	m_dwProcessId = pi.dwProcessId;

	if (IS_VALID_HANDLE(m_hProcessHandle)) {
		CreateModuleList();
		m_szProcessName = CProcessName::GetProcessName(m_hProcessHandle);
	}

	return IS_VALID_HANDLE(m_hProcessHandle);
}

void CProcessHelper::Close()
{
	m_openType = EOPenTypes::OPEN_STANDART;
	m_dwProcessId = 0;
	m_szProcessName.clear();

	m_listModules.clear();

	if (IS_VALID_HANDLE(m_hProcessHandle) && m_bIsSonicHandle == false)
	{
		CloseHandle(m_hProcessHandle);
		m_hProcessHandle = INVALID_HANDLE_VALUE;
	}

	if (m_cSafeProcessHandleHelper && m_cSafeProcessHandleHelper.get())
		m_cSafeProcessHandleHelper->ClearVeriables();

	if (m_cHijackProcessHandleHelper && m_cHijackProcessHandleHelper.get())
		m_cHijackProcessHandleHelper->ClearVeriables();
}

bool CProcessHelper::Terminate() const
{
	return (NT_SUCCESS(WinAPITable->NtTerminateProcess(m_hProcessHandle, STATUS_SUCCESS)));
}

bool CProcessHelper::ProtectHandle() const
{
	auto bRet = SetHandleInformation(m_hProcessHandle, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE);
	return (bRet == TRUE);
}

void CProcessHelper::SendKey(HWND hWnd, DWORD dwKey, DWORD dwDelay, EKeyTypes type)
{
	if (!hWnd)
		return;
	
	if (type == EKeyTypes::KEY_DOWN || type == EKeyTypes::KEY_BOTH)
		SendMessageA(hWnd, WM_KEYDOWN, dwKey, 0x00000001 | (LPARAM)(MapVirtualKey((int)dwKey, 0) << 16));

	if (dwDelay)
		Sleep(dwDelay);

	if (type == EKeyTypes::KEY_UP || type == EKeyTypes::KEY_BOTH)
		SendMessageA(hWnd, WM_KEYUP, dwKey, 0x00000001 | (LPARAM)(MapVirtualKey((int)dwKey, 0) << 16));
}

void CProcessHelper::CreateModuleList()
{
	m_listModules.clear();

	auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);
	if (!IS_VALID_HANDLE(hSnapshot)) return;

	MODULEENTRY32 me;
	memset(&me, 0, sizeof(me));
	me.dwSize = sizeof(me);

	char szPath[MAX_PATH];
	do
	{
		GetModuleFileNameExA(m_hProcessHandle, me.hModule, szPath, MAX_PATH);
		m_listModules.push_back(CModuleHelper(m_hProcessHandle, me.hModule, me.modBaseSize, me.szModule, szPath));
	} while (Module32Next(hSnapshot, &me));

	CloseHandle(hSnapshot);
}

const CModuleHelper CProcessHelper::GetModule(const char* c_szModuleName) const
{
	for (const auto & curModule : m_listModules)
		if (curModule.GetName() && !strcmp(curModule.GetName(), c_szModuleName))
			return curModule;

	return CModuleHelper(INVALID_HANDLE_VALUE, nullptr, 0, "", "");
}

std::size_t CProcessHelper::ModuleCount() const
{
	return m_listModules.size();
}

bool CProcessHelper::CheckCSGOModulesIsLoaded() const
{
	auto pClientModule = GetModule(XOR("client.dll"));
	if (false == pClientModule.IsValid()) {
		return false;
	}

	auto pEngineModule = GetModule(XOR("engine.dll"));
	if (false == pEngineModule.IsValid()) {
		return false;
	}

	auto pPhonon3dModule = GetModule(XOR("phonon.dll"));
	if (false == pPhonon3dModule.IsValid()) {
		return false;
	}

	auto pVstdlibModule = GetModule(XOR("vstdlib.dll"));
	if (false == pVstdlibModule.IsValid()) {
		return false;
	}

	auto pTier0Module = GetModule(XOR("tier0.dll"));
	if (false == pTier0Module.IsValid()) {
		return false;
	}

	auto pSteamAPIModule = GetModule(XOR("steam_api.dll"));
	if (false == pSteamAPIModule.IsValid()) {
		return false;
	}

	return true;
}

void CProcessHelper::WaitLoadCSGOModules(DWORD dwDelay)
{
	bool bIsLoaded = false;
	do {
		CreateModuleList();

		bIsLoaded = CheckCSGOModulesIsLoaded();
		Sleep(dwDelay);
	} while (!bIsLoaded);
}

DWORD CProcessHelper::GetBaseAddress() const
{
	auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);
	if (!IS_VALID_HANDLE(hSnapshot)) return 0;

	MODULEENTRY32 me;
	memset(&me, 0, sizeof(me));
	me.dwSize = sizeof(me);

	DWORD dwBaseAddress = 0;
	if (Module32First(hSnapshot, &me))
		dwBaseAddress = reinterpret_cast<DWORD>(me.modBaseAddr);

	CloseHandle(hSnapshot);
	return dwBaseAddress;
}

DWORD CProcessHelper::GetBaseSize() const
{
	auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);
	if (!IS_VALID_HANDLE(hSnapshot)) return 0;

	MODULEENTRY32 me;
	memset(&me, 0, sizeof(me));
	me.dwSize = sizeof(me);

	DWORD dwSize = 0;
	if (Module32First(hSnapshot, &me))
		dwSize = me.modBaseSize;

	CloseHandle(hSnapshot);
	return dwSize;
}

HANDLE CProcessHelper::GetHandle() const
{
	return m_hProcessHandle;
}

DWORD CProcessHelper::GetProcessId() const
{
	return m_dwProcessId;
}

const char * CProcessHelper::GetName() const
{
	return m_szProcessName.c_str();
}

std::string CProcessHelper::GetPath() const
{
	auto szProcessPath = CProcessName::GetProcessName(m_hProcessHandle);
	return szProcessPath;
}

std::vector<HWND> CProcessHelper::GetWindows() const
{
	std::vector<HWND> vWindowList;

	HWND hWnd = nullptr;
	do
	{
		hWnd = FindWindowExA(NULL, hWnd, NULL, NULL);

		DWORD dwPID = 0;
		GetWindowThreadProcessId(hWnd, &dwPID);

		if (dwPID == m_dwProcessId)
			vWindowList.push_back(hWnd);

	} while (hWnd);
	return vWindowList;
}

HWND CProcessHelper::GetWindow() const
{
	auto vWindowList = GetWindows();
	HWND hTargetWnd = nullptr;

	for (auto & hWnd : vWindowList)
	{
		if (IsWindowVisible(hWnd) && ::GetWindow(hWnd, GW_OWNER) == HWND(NULL)) {
			hTargetWnd = hWnd;
			break;
		}
	}

	return hTargetWnd;
}

HWND CProcessHelper::GetWindow(const std::string & szTargetClassname) const
{
	auto vWindowList = GetWindows();
	HWND hTargetWnd = nullptr;

	for (auto & hWnd : vWindowList)
	{
		char szClass[MAX_PATH] = { 0x0 };
		GetClassNameA(hWnd, szClass, MAX_PATH);

		if (strstr(szClass, szTargetClassname.c_str()))
			hTargetWnd = hWnd;
	}

	return hTargetWnd;
}

bool CProcessHelper::IsFocused() const
{
	auto vWindowList = GetWindows();
	for (auto & hWnd : vWindowList)
	{
		if (GetForegroundWindow() == hWnd)
			return true;
	}
	return false;
}

bool CProcessHelper::IsRunning() const 
{
	/*
	DWORD dwExitCode = 0;
	return (GetExitCodeProcess(m_hProcessHandle, &dwExitCode) && dwExitCode == STILL_ACTIVE);
	*/
	return (WaitForSingleObject(m_hProcessHandle, 0) == WAIT_TIMEOUT);
}
