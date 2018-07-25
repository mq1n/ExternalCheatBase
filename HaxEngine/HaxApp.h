#pragma once

#include "../HaxEngine/DynamicWinAPI.h"
#include "../HaxEngine/Functions.h"
#include "../HaxEngine/spdlog_wrapper.h"
#include "../HaxEngine/WMI.h"
#include "../HaxEngine/ProcessHelper.h"
#include "../HaxEngine/XOR.h"

typedef struct _handled_process_data
{
	DWORD dwProcessId;
} SHandledProcessData, *PHandledProcessData;

class CHaxApp : public std::enable_shared_from_this <CHaxApp>
{
	typedef std::function<void(bool bCreated)> TAppCallback;

	public:
		CHaxApp(const std::string & szTargetProcessName);
		~CHaxApp();
	
		void	PreInitialize();
		void	Initialize(TAppCallback pAppCallback);
		void	InitializeSelfSecurity();

		void	HaxAppWatcher(DWORD dwProcessId, const std::string & szProcessName, bool bIsCreated);

	public:
		auto	LogHelperInstance()			{ return logHelper;				};
		auto	GetWMIInstance()			{ return WMI;					};
		auto	GetFunctionsInstance()		{ return Functions;				};
		auto	GetWinApiHelperInstance()	{ return winAPIHelper;			};

		auto	AppIsInitiliazed()			{ return m_bAppIsInitiliazed;	};

		auto	GetHandledProcesses()		{ return m_vHandledProcessList; };

	protected:
		inline bool		ClassInitIsFailed();

	private:
		TAppCallback	m_pAppCallback;

		bool			m_bAppIsInitiliazed;
		std::string		m_szTargetProcName;

		std::shared_ptr <CLogger>			logHelper;
		std::shared_ptr <CWMI>				WMI;
		std::shared_ptr <CFunctions>		Functions;
		std::shared_ptr <CDynamicWinapi>	winAPIHelper;

		std::vector < std::shared_ptr <SHandledProcessData> >	m_vHandledProcessList;
};
