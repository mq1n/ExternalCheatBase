#include "HaxEngine.h"
#include "main.h"
#include "defines.h"
#include "VersionHelpers.h"
#include "Elevation.h"
#include "XOR.h"
#include "DACLRules.h"
#include "Antis.h"
#include "Defines.h"

__forceinline void CheckHWID()
{
	std::string szGUID = g_haxApp->GetFunctionsInstance()->GetGUID();
	szGUID = szGUID.substr(11, szGUID.size());
	szGUID.resize(szGUID.size() - 2);

	DWORD dwCurrentHash = g_haxApp->GetFunctionsInstance()->GetStringHash((LPVOID)szGUID.c_str(), FALSE, szGUID.size());
	DEBUG_LOG(LL_SYS, "Current hwid: %s hash: %u", szGUID.c_str(), dwCurrentHash);

	if (dwCurrentHash != 0xC0ED12CA /* my hwid hash */)
	{
		DEBUG_LOG(LL_CRI, "Is not valid HWID!");
		abort();
	}
}

__forceinline void CheckOS()
{
	// OS check
	if (IsWindowsVistaOrGreater() == false || IsWindowsServer()) {
		DEBUG_LOG(LL_CRI, "Not supported OS!");
		abort();
	}

	CHAR __NtCreateThreadEx[] = { 'N', 't', 'C', 'r', 'e', 'a', 't', 'e', 'T', 'h', 'r', 'e', 'a', 'd', 'E', 'x', 0x0 }; // NtCreateThreadEx
	if (IsWindowsVistaOrGreater() == false && GetProcAddress(WinModuleTable->hNtdll, __NtCreateThreadEx) /* Any not XP supported windows api*/) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on FakeOS!");
		abort();
	}
}

__forceinline void CheckSafeMode()
{
	// Safe mode check
	if (GetSystemMetrics(SM_CLEANBOOT) > 0) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on GetSystemMetrics!");
		abort();
	}
}

__forceinline void CheckElevation()
{
	// Check elevate
	if (IsWindowsVistaOrGreater())
	{
		try {
			if (CElevation::IsProcessElevated() == FALSE) {
				DEBUG_LOG(LL_CRI, "Process is not elevated!");
				abort();
			}
		}
		catch (DWORD dwError) { UNREFERENCED_PARAMETER(dwError); }
		catch (...) {}

		try {
			if (CElevation::IsRunAsAdmin() == FALSE) {
				DEBUG_LOG(LL_CRI, "Please run this process as administrator");
				abort();
			}
		}
		catch (DWORD dwError) { UNREFERENCED_PARAMETER(dwError); }
		catch (...) {}
	}
}

__forceinline void CheckHaxDisk()
{
	auto szCurrentFileName = g_haxApp->GetFunctionsInstance()->ExeNameWithPath();
	auto szMyDisk = g_haxApp->GetFunctionsInstance()->ExeNameWithPath();
	auto pos = szMyDisk.find_first_of("\\/");

	TCHAR fs[MAX_PATH + 1];
	if (GetVolumeInformationA(szMyDisk.substr(0, pos).c_str(), NULL, 0, NULL, NULL, NULL, fs, MAX_PATH + 1) &&
		!strcmp(g_haxApp->GetFunctionsInstance()->szLower(fs).c_str(), XOR("ntfs")))
	{
		DEBUG_LOG(LL_CRI, "Error! Run this app just in a Non-NTFS disk!");
		abort();
	}
}

__forceinline void CheckHaxProcessPack()
{
	if (g_haxApp->GetFunctionsInstance()->IsPackedProcess(g_haxApp->GetFunctionsInstance()->ExeNameWithPath()) == false) {
		DEBUG_LOG(LL_CRI, "Error! Re-pack this app!");
		abort();
	}
}

__forceinline void CheckHaxProcessDate()
{
	auto hFile = CreateFileA(g_haxApp->GetFunctionsInstance()->ExeNameWithPath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (IS_VALID_HANDLE(hFile))
	{
		FILETIME lpCreationTime, lpLastAccessTime, lpLastWriteTime;
		if (GetFileTime(hFile, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
		{
			SYSTEMTIME st;
			if (FileTimeToSystemTime(&lpCreationTime, &st))
			{
				auto dwFileCreatedTime = g_haxApp->GetFunctionsInstance()->SystemTimeToTimestamp(st);
				auto dwCurrentTimestamp = g_haxApp->GetFunctionsInstance()->GetCurrentTimestamp();

				if (dwCurrentTimestamp - dwFileCreatedTime > 172800 /* Expire date */) {
					DEBUG_LOG(LL_CRI, "Error! This version is outdated! Change path and re-pack this app!");
					abort();
				}
			}
		}
		CloseHandle(hFile);
	}
}

__forceinline void SetDACLRules()
{
	CDaclRules::SetPermissions();

	CDaclRules::SetDACLRulesToProcess();
	CDaclRules::SetDACLRulesToThread(NtCurrentThread);

	CDaclRules::BlockAccess();
}

__forceinline void InitAntiVM()
{
	CAntis::AntiCuckoo();
	CAntis::AntiSandboxie();
	CAntis::AntiVirtualMachine();
	CAntis::AntiSunbeltSandBox();
	CAntis::AntiWine();
	CAntis::Anticuckoomon();
	CAntis::AntiSandbox();
}

__forceinline void InitAntiEmulation()
{
	CAntis::CheckRandomApiExport();
	if (CAntis::CheckStdChronoTimeRange()) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on CheckStdChronoTimeRange!");
		abort();
	}
}

__forceinline void InitAntiDebug()
{
	CAntis::CrashDebugger();
	CAntis::DetachFromDebuggerProcess();
	WinAPITable->ZwSetInformationThread(NtCurrentThread, ThreadHideFromDebugger, 0, 0);

	if (IsWindowsVistaOrGreater())
		CAntis::SetFakeImageSize();

	if (CAntis::CheckCloseProtectedHandle()) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on CheckCloseProtectedHandle!");
		abort();
	}

	if (CAntis::CheckSeDebugPriv()) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on CheckSeDebugPriv!");
		abort();
	}
}

__forceinline void GetDebugPriv()
{
	if (IsWindowsVistaOrGreater())
	{
		BOOLEAN boAdjustPrivRet;
		NTSTATUS ntStat = WinAPITable->RtlAdjustPrivilege(20, TRUE, FALSE, &boAdjustPrivRet);
		if (!NT_SUCCESS(ntStat)) {
			DEBUG_LOG(LL_CRI, "Can not get debug privs! Stat: %p", ntStat);
			abort();
		}
	}
}

void CHaxApp::InitializeSelfSecurity()
{
#ifdef _DEBUG
	DEBUG_LOG(LL_CRI, "Self security passed on debug build!");
	return;
#elif USE_SELF_SECURITY == false
	return;
#endif

	DEBUG_LOG(LL_SYS, "Self security started!");

	CAntis::AntiCuckoo();
	CAntis::InitExceptionHandlers();
	InitAntiVM();
#ifndef _DEBUG
	InitAntiEmulation();
	InitAntiDebug();
#endif

	DEBUG_LOG(LL_SYS, "Self security part 1 completed!");

	// CheckHWID();
	CheckOS();
	CheckSafeMode();

	DEBUG_LOG(LL_SYS, "Self security part 2 completed!");

#ifndef _DEBUG
	GetDebugPriv();
	CheckElevation();
#endif

	DEBUG_LOG(LL_SYS, "Self security part 3 completed!");

	CheckHaxDisk();
	// CheckHaxProcessPack();
	CheckHaxProcessDate();

	DEBUG_LOG(LL_SYS, "Self security part 4 completed!");

	SetDACLRules();

	DEBUG_LOG(LL_SYS, "Self security part 5 completed!");

	CAntis::InitializeBlockHandles();

	DEBUG_LOG(LL_SYS, "Self security completed!");
}

