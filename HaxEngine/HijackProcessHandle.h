#pragma once

class CHijackProcessHandle
{
	public:
		CHijackProcessHandle(DWORD dwProcessId);
		~CHijackProcessHandle();

		HANDLE	CreateProcessHandle();
		void	ClearVeriables();

	private:
		DWORD	m_dwTargetProcessId;
		HANDLE	m_hTargetProcessHandle;
};

