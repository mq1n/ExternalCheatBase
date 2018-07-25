#include "HaxEngine.h"
#include "main.h"
#include "ProcessName.h"
#include "XOR.h"

std::string CProcessName::DosDevicePath2LogicalPath(LPCTSTR lpszDosPath)
{
	std::string strResult;
	TCHAR szTemp[MAX_PATH];
	szTemp[0] = '\0';

	if (lpszDosPath == NULL || strlen(lpszDosPath) == NULL || !GetLogicalDriveStringsA(_countof(szTemp) - 1, szTemp))
		return strResult;

	TCHAR szName[MAX_PATH];
	TCHAR szDrive[3] = TEXT(" :");
	BOOL bFound = FALSE;
	TCHAR* p = szTemp;

	do {
		// Copy the drive letter to the template string
		*szDrive = *p;

		// Look up each device name
		if (QueryDosDeviceA(szDrive, szName, _countof(szName))) {
			UINT uNameLen = (UINT)strlen(szName);

			if (uNameLen < MAX_PATH)
			{
				bFound = strncmp(lpszDosPath, szName, uNameLen) == 0;

				if (bFound) {
					// Reconstruct pszFilename using szTemp
					// Replace device path with DOS path
					TCHAR szTempFile[MAX_PATH];
					sprintf_s(szTempFile, XOR("%s%s"), szDrive, lpszDosPath + uNameLen);
					strResult = szTempFile;
				}
			}
		}

		// Go to the next NULL character.
		while (*p++);
	} while (!bFound && *p); // end of string

	return strResult;
}

std::string CProcessName::GetProcessFullName(HANDLE hProcess)
{
	CHAR processPath[MAX_PATH];
	if (GetProcessImageFileNameA(hProcess, processPath, _countof(processPath)))
		return processPath;

	memset(&processPath, 0, MAX_PATH);
	if (GetModuleFileNameExA(hProcess, NULL, processPath, _countof(processPath)))
		return processPath;

	return std::string("");
}

std::string CProcessName::GetProcessName(HANDLE hProcess)
{
	auto szDosName = GetProcessFullName(hProcess);
	if (szDosName.empty()) return std::string("");

	auto szProcessName = DosDevicePath2LogicalPath(szDosName.c_str());
	if (szProcessName.empty()) return std::string("");

	std::transform(szProcessName.begin(), szProcessName.end(), szProcessName.begin(), tolower);
	return szProcessName;
}

