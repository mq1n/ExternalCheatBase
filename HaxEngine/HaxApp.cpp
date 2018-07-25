#include "HaxEngine.h"
#include "main.h"
#include "HaxApp.h"

#include "../HaxEngine/MiniDump.h"

CHaxApp::CHaxApp(const std::string & szTargetProcessName) :
	m_bAppIsInitiliazed(false), m_szTargetProcName(szTargetProcessName)
{
	m_vHandledProcessList.clear();
}

CHaxApp::~CHaxApp()
{
	m_vHandledProcessList.clear();
}

void CHaxApp::PreInitialize()
{
	// Init Debug Console
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "a", stdout);
	freopen("CONIN$", "r", stdin);
#endif

	// Init classes
	logHelper		= std::make_shared<CLogger>(XOR("H4x"), CUSTOM_LOG_FILENAME);
	WMI				= std::make_shared<CWMI>(m_szTargetProcName.c_str());
	Functions		= std::make_shared<CFunctions>();
	winAPIHelper	= std::make_shared<CDynamicWinapi>();

	if (ClassInitIsFailed()) {
		Logf(CUSTOM_LOG_FILENAME, XOR("Classes allocation failed!\n"));
		abort();
	}
	m_bAppIsInitiliazed = true;
}

bool CHaxApp::ClassInitIsFailed()
{
	if (logHelper && logHelper.get()) 
	{
		DEBUG_LOG(LL_SYS, "logHelper:\t\t%p-%p",		logHelper, logHelper.get());
		DEBUG_LOG(LL_SYS, "WMI:\t\t%p-%p",				WMI, WMI.get());
		DEBUG_LOG(LL_SYS, "Functions:\t\t%p-%p",		Functions, Functions.get());
		DEBUG_LOG(LL_SYS, "winAPIHelper:\t%p-%p",		winAPIHelper, winAPIHelper.get());
	}
#ifdef _DEBUG
	else {
		Logf(CUSTOM_LOG_FILENAME, "Log helper initilization failed!\n");
	}
#endif

	return (
		logHelper		== nullptr	|| logHelper.get()		== nullptr	|| 
		WMI				== nullptr	|| WMI.get()			== nullptr	||
		Functions		== nullptr	|| Functions.get()		== nullptr  ||
		winAPIHelper	== nullptr	|| winAPIHelper.get()	== nullptr
	);
}

void CHaxApp::HaxAppWatcher(DWORD dwProcessId, const std::string & szProcessName, bool bIsCreated)
{
	DEBUG_LOG(LL_SYS, "PID: %u triggered! IsCreated: %d Process Name: %s", dwProcessId, bIsCreated, szProcessName.c_str());

	if (bIsCreated == false) // It's a terminated process
	{
		if (m_vHandledProcessList.empty() == false) // Handled process list isn't empty
		{
			for (auto & curProcData : m_vHandledProcessList) // Iterate handled processes
			{
				if (curProcData->dwProcessId == dwProcessId)
				{
					m_pAppCallback(false);

					for (auto iter = m_vHandledProcessList.begin(); iter != m_vHandledProcessList.end(); ++iter) // Remove hax process datas from handled process list
					{
						if (*iter == curProcData)
						{
							m_vHandledProcessList.erase(iter);
							break;
						}
					}
				}
			}
		}

		DEBUG_LOG(LL_SYS, "Process: %u removed from handled processes!", dwProcessId);
		return;
	}

	// useless if already handled pid
	if (m_vHandledProcessList.empty() == false)
		for (auto & curProcData : m_vHandledProcessList)
			if (curProcData->dwProcessId == dwProcessId)
				return;

	m_pAppCallback(true);

	auto curProcData				= std::make_shared<SHandledProcessData>();
		 curProcData->dwProcessId	= dwProcessId;

	m_vHandledProcessList.push_back(curProcData);
}

void CHaxApp::Initialize(TAppCallback pAppCallback)
{
	m_pAppCallback = pAppCallback;

	// Create mini dump handler
	CMiniDump::InitMiniDump();

	// Init WinAPI Table
	winAPIHelper->Initialize();

	// Init self security
	InitializeSelfSecurity();

	// Init process watcher
	WMI->CreateWatcher();
}

