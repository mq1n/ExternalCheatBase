#pragma once

#ifndef FAST_MEMORY_WITH_SUSPEND
	#define FAST_MEMORY_WITH_SUSPEND	 true
#endif
#ifndef FAST_MEMORY_WITHOUT_SUSPEND
	#define FAST_MEMORY_WITHOUT_SUSPEND	 false
#endif

class CFastMemory
{
	public:
		CFastMemory(DWORD dwProcessId, bool bIsSuspend);
		~CFastMemory();

		bool Initialize();
		bool Finalize();

		auto GetProcessHelperInstance() { return m_cProcHelper; };

	private:
		std::shared_ptr <CProcessHelper>	m_cProcHelper;

		bool								m_bIsSuspend;
		HANDLE								m_hProcessHandle;
		DWORD								m_dwProcessId;
};

