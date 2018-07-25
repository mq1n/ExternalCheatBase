#pragma once

class CFunctions : public std::enable_shared_from_this<CFunctions>
{
	public:
		const char* GetTime();
		DWORD GetCurrentTimestamp();
		DWORD SystemTimeToTimestamp(SYSTEMTIME st);
		std::string CreateFormattedString(const char* c_szArgFormat, ...);
		std::string szLower(std::string szSource);
		std::string ExeNameWithPath();
		bool ProcessIsItAlive(DWORD dwProcessId);
		std::string GetProcessNameFromProcessId(DWORD dwProcessId);
		DWORD GetProcessParentProcessId(DWORD dwMainProcessId);
		std::vector <DWORD> GetProcessIdsFromProcessName(const char* c_szProcessName);
		bool ProcessHasThread(DWORD dwProcessID);
		float GetEntropy(BYTE* byBuffer, DWORD dwLength);
		bool IsPackedProcess(std::string szName);
		const char* GetGUID();
		DWORD GetStringHash(LPVOID lpBuffer, BOOL bUnicode, UINT uLen);
		bool IsFileExist(const std::string & szName);
		std::string GetFileMd5(const char* c_szFileName);
		std::string GetMd5(BYTE *memchunk, int len);
		ULONG GetRandomInt(ULONG uMin, ULONG uMax);
		std::string GetRandomString(int iLength);
};

