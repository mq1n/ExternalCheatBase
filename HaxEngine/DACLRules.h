#pragma once

class CDaclRules
{
	public:
		static BOOL SetDACLRulesToProcess();
		static BOOL SetDACLRulesToThread(HANDLE hThread);

		static void SetPermissions();

		static bool BlockAccess();
};

