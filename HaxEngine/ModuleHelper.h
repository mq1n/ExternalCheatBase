#pragma once

class CModuleHelper
{
	public:
		CModuleHelper(HANDLE hProcess, LPVOID pModuleBase, DWORD dwModuleSize, const char* c_szModuleName, const char* c_szModulePath);
		~CModuleHelper(void) = default;

		bool IsValid() const;

		LPVOID GetAddress() const;
		DWORD GetSize() const;

		const char* GetName() const;
		const char* GetPath() const;

		DWORD Read(DWORD dwOffset, LPVOID pBuffer, DWORD dwSize);
		DWORD Write(DWORD dwOffset, LPVOID pBuffer, DWORD dwSize);

		bool operator==(const CModuleHelper& o) const;
		bool operator!=(const CModuleHelper& o) const;

	private:
		HANDLE			m_hProcess;

		LPVOID			m_pModuleBase;
		DWORD			m_dwModuleSize;

		std::string		m_szModuleName;
		std::string		m_szModulePath;
};

