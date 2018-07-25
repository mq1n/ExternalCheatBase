#include "HaxEngine.h"
#include "main.h"
#include "Functions.h"
#include "Md5.h"

const char* CFunctions::GetTime()
{
	time_t rawtime;
	time(&rawtime);

	struct tm* timeinfo;
	timeinfo = localtime(&rawtime);

	return asctime(timeinfo);
}

DWORD CFunctions::GetCurrentTimestamp()
{
	time_t curTime;
	time(&curTime);
	return (DWORD)curTime;
}

DWORD CFunctions::SystemTimeToTimestamp(SYSTEMTIME st)
{
	std::tm tm;
	tm.tm_sec = st.wSecond;
	tm.tm_min = st.wMinute;
	tm.tm_hour = st.wHour;
	tm.tm_mday = st.wDay;
	tm.tm_mon = st.wMonth - 1;
	tm.tm_year = st.wYear - 1900;
	tm.tm_isdst = -1;

	std::time_t fileCreatedTime = std::mktime(&tm);
	return (DWORD)fileCreatedTime;
}

std::string CFunctions::CreateFormattedString(const char* c_szArgFormat, ...)
{
	char cTmpString[8096];

	va_list vaArgList;
	va_start(vaArgList, c_szArgFormat);
	vsprintf(cTmpString, c_szArgFormat, vaArgList);
	va_end(vaArgList);

	return cTmpString;
}

std::string CFunctions::szLower(std::string szSource)
{
	std::string szLowerCopy(szSource.begin(), szSource.end());
	std::transform(szLowerCopy.begin(), szLowerCopy.end(), szLowerCopy.begin(), tolower);
	return szLowerCopy;
}

std::string CFunctions::ExeNameWithPath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}

bool CFunctions::ProcessIsItAlive(DWORD dwProcessId)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return false;

	PROCESSENTRY32 pt;
	pt.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pt)) {
		do {
			if (pt.th32ProcessID == dwProcessId) {
				CloseHandle(hSnapshot);
				return true;
			}
		} while (Process32Next(hSnapshot, &pt));
	}

	CloseHandle(hSnapshot);
	return false;
}

std::string CFunctions::GetProcessNameFromProcessId(DWORD dwProcessId)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return std::string("");

	std::string szResult;

	PROCESSENTRY32 pt;
	pt.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pt)) {
		do {
			if (dwProcessId == pt.th32ProcessID) {
				szResult = pt.szExeFile;
				CloseHandle(hSnapshot);
				return szResult;
			}
		} while (Process32Next(hSnapshot, &pt));
	}

	CloseHandle(hSnapshot);
	return szResult;
}

DWORD CFunctions::GetProcessParentProcessId(DWORD dwMainProcessId)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

	DWORD dwProcessId = 0;

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pe)) {
		do {
			if (pe.th32ProcessID == dwMainProcessId) {
				dwProcessId = pe.th32ParentProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &pe));
	}

	CloseHandle(hSnapshot);
	return (dwProcessId);
}

std::vector <DWORD> CFunctions::GetProcessIdsFromProcessName(const char* c_szProcessName)
{
	std::vector <DWORD> vPIDs;

	auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapshot || hSnapshot == INVALID_HANDLE_VALUE)
		return vPIDs;

	PROCESSENTRY32 pt;
	pt.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pt)) {
		do {
			std::string szProcessName = pt.szExeFile;
			std::transform(szProcessName.begin(), szProcessName.end(), szProcessName.begin(), tolower);

			if (strstr(szProcessName.c_str(), c_szProcessName))
				vPIDs.emplace_back(pt.th32ProcessID);

		} while (Process32Next(hSnapshot, &pt));
	}

	CloseHandle(hSnapshot);
	return vPIDs;
}

bool CFunctions::ProcessHasThread(DWORD dwProcessID)
{
	auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (hSnap == INVALID_HANDLE_VALUE) return false;

	THREADENTRY32 ti = { 0 };
	ti.dwSize = sizeof(ti);

	if (Thread32First(hSnap, &ti))
	{
		do {
			if (dwProcessID == ti.th32OwnerProcessID) {
				CloseHandle(hSnap);
				return true;
			}
		} while (Thread32Next(hSnap, &ti));
	}

	CloseHandle(hSnap);
	return false;
}

float CFunctions::GetEntropy(BYTE* byBuffer, DWORD dwLength)
{
	DWORD dwSize = 0;
	long lBuff[0xFF + 1] = { 0 };
	float fTemp, fEntropy = 0;

	for (DWORD i = 0; i < dwLength; i++)
	{
		lBuff[byBuffer[i]]++;
		dwSize++;
	}

	for (DWORD i = 0; i < 256; i++)
	{
		if (lBuff[i])
		{
			fTemp = (float)lBuff[i] / (float)dwSize;
			fEntropy += (-fTemp * log2(fTemp));
		}
	}

	return fEntropy;
}

bool CFunctions::IsPackedProcess(std::string szName)
{
	bool bIsPacked = false;

	HANDLE hFile = CreateFileA(szName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return bIsPacked;

	DWORD dwFileLen = GetFileSize(hFile, NULL);
	if (!dwFileLen || dwFileLen == INVALID_FILE_SIZE)
		return bIsPacked;

	BYTE* byImage = new BYTE[dwFileLen];
	if (!byImage)
		return bIsPacked;

	DWORD dwReadedBytes;
	BOOL readRet = ReadFile(hFile, byImage, dwFileLen, &dwReadedBytes, NULL);
	if (!readRet || !dwReadedBytes) {
		delete[] byImage;
		return bIsPacked;
	}

	float fEntropy = GetEntropy(byImage, dwFileLen);

	delete[] byImage;
	CloseHandle(hFile);

	return (fEntropy > 7.5f);
}

const char* CFunctions::GetGUID()
{
	TCHAR szMainDisc[MAX_PATH];
	if (!GetLogicalDriveStringsA(_countof(szMainDisc) - 1, szMainDisc))
		return "";

	CHAR cGuid[1024];
	if (!GetVolumeNameForVolumeMountPointA(szMainDisc, cGuid, 1024))
		return "";

	return cGuid;
}

DWORD CFunctions::GetStringHash(LPVOID lpBuffer, BOOL bUnicode, UINT uLen)
{
	DWORD dwHash = 0;
	LPSTR strBuffer = (LPSTR)lpBuffer;

	while (uLen--)
	{
		dwHash = (dwHash >> 13) | (dwHash << 19);
		dwHash += (DWORD)*strBuffer++;

		if (bUnicode)
			strBuffer++;
	}
	return dwHash;
}

bool CFunctions::IsFileExist(const std::string & szName)
{
	DWORD dwAttrib = GetFileAttributesA(szName.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string CFunctions::GetFileMd5(const char* c_szFileName)
{
	MD5 md5;
	auto szResult = md5.digestFile(c_szFileName);
	return szResult;
}

std::string CFunctions::GetMd5(BYTE *memchunk, int len)
{
	MD5 md5;
	auto szResult = md5.digestMemory(memchunk, len);
	return szResult;
}

ULONG CFunctions::GetRandomInt(ULONG uMin, ULONG uMax)
{
	if (uMax < (ULONG)0xFFFFFFFF)
		uMax++;

	return (rand() % (uMax - uMin)) + uMin;
}

std::string CFunctions::GetRandomString(int iLength)
{
	CHAR __alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0x0 }; // abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890
	static std::string charset = __alphabet;
	std::string result;
	result.resize(iLength);

	for (int i = 0; i < iLength; i++)
		result[i] = charset[rand() % charset.length()];

	return result;
}

