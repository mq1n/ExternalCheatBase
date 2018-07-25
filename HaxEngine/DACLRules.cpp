#include "HaxEngine.h"
#include "main.h"
#include "DACLRules.h"
#include "XOR.h"

BOOL CDaclRules::SetDACLRulesToProcess()
{
	BYTE    abyBuffer[0x200];
	PACL    pACL;
	SID_IDENTIFIER_AUTHORITY stIdentifierAuthority = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSid = NULL;
	BOOL bRet = FALSE;
	DWORD dwSize = 0;
	HANDLE hToken = NULL;
	PTOKEN_USER pUserInfo = NULL;

	if (AllocateAndInitializeSid(&stIdentifierAuthority, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSid) == FALSE)
		goto Cleanup;
	if (OpenProcessToken(NtCurrentProcess, TOKEN_QUERY, &hToken) == FALSE)
		goto Cleanup;
	GetTokenInformation(hToken, TokenUser, NULL, NULL, &dwSize);
	if (dwSize > 1024)
		goto Cleanup;
	pUserInfo = (PTOKEN_USER)GlobalAlloc(GPTR, dwSize);
	if (pUserInfo == NULL)
		goto Cleanup;
	if (GetTokenInformation(hToken, TokenUser, pUserInfo, dwSize, &dwSize) == FALSE)
		goto Cleanup;
	pACL = (PACL)&abyBuffer;
	if (InitializeAcl(pACL, 0x200, ACL_REVISION) == FALSE)
		goto Cleanup;
	// Deny except PROCESS_TERMINATE and PROCESS_SET_SESSIONID
	if (AddAccessDeniedAce(pACL, ACL_REVISION, PROCESS_CREATE_PROCESS | PROCESS_DUP_HANDLE | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, pSid) == FALSE)
		goto Cleanup;
	// Allow SYNCHRONIZE, PROCESS_QUERY_INFORMATION, PROCESS_SET_INFORMATION, PROCESS_SET_QUOTA and PROCESS_TERMINATE
	if (AddAccessAllowedAce(pACL, ACL_REVISION, SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION | PROCESS_SET_QUOTA | PROCESS_TERMINATE, pUserInfo->User.Sid) == FALSE)
		goto Cleanup;
	//if (SetSecurityInfo(NtCurrentProcess, SE_KERNEL_OBJECT, PROTECTED_DACL_SECURITY_INFORMATION /* Restart problem after close process | DACL_SECURITY_INFORMATION */, 0, 0, pACL, 0) != ERROR_SUCCESS)
	if (SetSecurityInfo(NtCurrentProcess, SE_KERNEL_OBJECT, PROTECTED_DACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, 0, 0, pACL, 0) != ERROR_SUCCESS)
		goto Cleanup;

	bRet = TRUE;

Cleanup:
	if (hToken)
		CloseHandle(hToken);
	if (pSid)
		FreeSid(pSid);

	return bRet;
}


BOOL CDaclRules::SetDACLRulesToThread(HANDLE hThread)
{
	BYTE abyBuffer[0x200];
	PACL pACL;
	SID_IDENTIFIER_AUTHORITY stIdentifierAuthority = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSid = NULL;
	BOOL bRet = FALSE;
	DWORD dwSize = 0;
	HANDLE hToken = NULL;
	PTOKEN_USER pUserInfo = NULL;
	DWORD dwLastErr = 0;
	DWORD dwErrStep = 0;

	if (AllocateAndInitializeSid(&stIdentifierAuthority, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSid) == FALSE) {
		dwLastErr = GetLastError();
		dwErrStep = 1;
		goto Cleanup;
	}
	if (OpenThreadToken(hThread, TOKEN_QUERY, TRUE, &hToken) == FALSE) {
		dwLastErr = GetLastError();
		dwErrStep = 2;
		goto Cleanup;
	}
	GetTokenInformation(hToken, TokenUser, NULL, NULL, &dwSize);
	if (dwSize > 1024) {
		dwLastErr = GetLastError();
		dwErrStep = 3;
		goto Cleanup;
	}
	pUserInfo = (PTOKEN_USER)GlobalAlloc(GPTR, dwSize);
	if (pUserInfo == NULL) {
		dwLastErr = GetLastError();
		dwErrStep = 4;
		goto Cleanup;
	}
	if (GetTokenInformation(hToken, TokenUser, pUserInfo, dwSize, &dwSize) == FALSE) {
		dwLastErr = GetLastError();
		dwErrStep = 5;
		goto Cleanup;
	}
	pACL = (PACL)&abyBuffer;
	if (InitializeAcl(pACL, 0x200, ACL_REVISION) == FALSE) {
		dwLastErr = GetLastError();
		dwErrStep = 6;
		goto Cleanup;
	}
	// Deny except PROCESS_TERMINATE and PROCESS_SET_SESSIONID
	if (AddAccessDeniedAce(pACL, ACL_REVISION, PROCESS_CREATE_PROCESS | PROCESS_DUP_HANDLE | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD | PROCESS_ALL_ACCESS, pSid) == FALSE) {
		dwLastErr = GetLastError();
		dwErrStep = 7;
		goto Cleanup;
	}
	if (SetSecurityInfo(hThread, SE_KERNEL_OBJECT, PROTECTED_DACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, 0, 0, pACL, 0) != ERROR_SUCCESS) {
		dwLastErr = GetLastError();
		dwErrStep = 8;
		goto Cleanup;
	}

	bRet = TRUE;

Cleanup:

	if (hToken)
		CloseHandle(hToken);
	if (pSid)
		FreeSid(pSid);

	return bRet;
}

void CDaclRules::SetPermissions()
{
	HANDLE ProcessHandle = NtCurrentProcess;

	EXPLICIT_ACCESS denyAccess = { PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME };
	DWORD dwAccessPermissions = GENERIC_WRITE | PROCESS_ALL_ACCESS | WRITE_DAC | DELETE | WRITE_OWNER | READ_CONTROL;
	
	CHAR __CURRENT_USER[] = { 'C', 'U', 'R', 'R', 'E', 'N', 'T', '_', 'U', 'S', 'E', 'R', 0x0 }; // CURRENT_USER
	BuildExplicitAccessWithNameA(&denyAccess, __CURRENT_USER, dwAccessPermissions, DENY_ACCESS, NO_INHERITANCE);

	PACL Pacl = NULL;
	SetEntriesInAclA(1, &denyAccess, NULL, &Pacl);
	SetSecurityInfo(ProcessHandle, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, Pacl, NULL);
	LocalFree(Pacl);

	SetPriorityClass(ProcessHandle, REALTIME_PRIORITY_CLASS);
}

bool CDaclRules::BlockAccess()
{
	HANDLE hProcess = NtCurrentProcess;
	SECURITY_ATTRIBUTES sa;

	const char * szSD = 
		XOR("D:P"
		"(D;OICI;GA;;;BG)"  /* Deny access to built-in guests */
		"(D;OICI;GA;;;AN)"  /*		  ^		 anonymous logon */
		"(D;OICI;GA;;;AU)"  /*		  ^		 authenticated users */
		"(D;OICI;GA;;;BA)"  /*		  ^		 administrators */
		"(D;OICI;GA;;;LA)")  /*		  ^		 Built-in Administrator */
	; 


	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = false;

	if (!ConvertStringSecurityDescriptorToSecurityDescriptorA(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL))
		return false;

	if (!SetKernelObjectSecurity(hProcess, DACL_SECURITY_INFORMATION, sa.lpSecurityDescriptor))
		return true;

	return true;
}

