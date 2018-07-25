#include "HaxEngine.h"
#include "main.h"
#include "ModuleHelper.h"

// TODO: Dump module        ReadProcessMemory( m_hProcess, module_entry.modBaseAddr, module_dump.data(), module_dump.size(), nullptr );

CModuleHelper::CModuleHelper(HANDLE hProcess, LPVOID pModuleBase, DWORD dwModuleSize, const char* c_szModuleName, const char* c_szModulePath) :
	m_hProcess(hProcess), m_pModuleBase(pModuleBase), m_dwModuleSize(dwModuleSize), m_szModuleName(c_szModuleName), m_szModulePath(c_szModulePath)
{
}

bool CModuleHelper::IsValid() const
{
	return (m_pModuleBase && m_dwModuleSize);
}


const char* CModuleHelper::GetName() const
{
	return m_szModuleName.c_str();
}

const char* CModuleHelper::GetPath() const
{
	return m_szModulePath.c_str();
}

LPVOID CModuleHelper::GetAddress() const
{
	return m_pModuleBase;
}

DWORD CModuleHelper::GetSize() const
{
	return m_dwModuleSize;
}


DWORD CModuleHelper::Read(DWORD dwOffset, LPVOID pBuffer, DWORD dwSize)
{
	if (!IS_VALID_HANDLE(m_hProcess))
		return 0;

	SIZE_T dwReadByteCount = 0;
	if (!ReadProcessMemory(m_hProcess, reinterpret_cast<BYTE*>(m_pModuleBase) + dwOffset, pBuffer, dwSize, &dwReadByteCount))
		dwReadByteCount = 0;

	return dwReadByteCount;
}

DWORD CModuleHelper::Write(DWORD dwOffset, LPVOID pBuffer, DWORD dwSize)
{
	if (!IS_VALID_HANDLE(m_hProcess))
		return 0;

	SIZE_T dwWriteByteCount = 0;
	if (!WriteProcessMemory(m_hProcess, reinterpret_cast<BYTE*>(m_pModuleBase) + dwOffset, pBuffer, dwSize, &dwWriteByteCount))
		dwWriteByteCount = 0;

	return dwWriteByteCount;
}


bool CModuleHelper::operator==(const CModuleHelper& o) const
{
	return (m_hProcess == o.m_hProcess && m_pModuleBase == o.m_pModuleBase && m_dwModuleSize == o.m_dwModuleSize);
}

bool CModuleHelper::operator!=(const CModuleHelper& o) const
{
	return (m_hProcess != o.m_hProcess || m_pModuleBase != o.m_pModuleBase || m_dwModuleSize != o.m_dwModuleSize);
}

