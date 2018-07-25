#pragma once

class CProcessName
{
	public:
		static std::string DosDevicePath2LogicalPath(LPCTSTR lpszDosPath);
		static std::string GetProcessFullName(HANDLE hProcess);
		static std::string GetProcessName(HANDLE hProcess);
};

