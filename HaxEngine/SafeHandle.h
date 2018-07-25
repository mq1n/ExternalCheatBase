#pragma once

class CSafeProcessHandle
{
	public:
		CSafeProcessHandle(DWORD dwProcessId);
		~CSafeProcessHandle();

		HANDLE	CreateProcessHandle();
		void	ClearVeriables();

	protected:
		void	CloseUselessHandles();

	private:
		std::vector<HANDLE> m_vHandleList;

		DWORD				m_dwTargetProcessId;
		HANDLE				m_hTargetProcessHandle;
};

class CSafeThreadHandle
{
	public:
		CSafeThreadHandle(HANDLE hOwnerProcess, DWORD dwThreadId);
		~CSafeThreadHandle();

		HANDLE	CreateThreadHandle();
		void	ClearVeriables();

	protected:
		void	CloseUselessHandles();

	private:
		std::vector <HANDLE>	m_vHandleList;

		DWORD					m_dwTargetThreadId;
		HANDLE					m_hTargetThreadHandle;

		HANDLE					m_hOwnerProcessHandle;
};
