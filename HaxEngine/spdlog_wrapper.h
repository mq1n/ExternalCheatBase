#pragma once
#include "BasicLog.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>

enum ELogLevels
{
	LL_SYS,
	LL_ERR,
	LL_CRI,
	LL_WARN,
	LL_DEV,
	LL_TRACE
};

class CLogger : public std::enable_shared_from_this<CLogger>
{
	public:
		CLogger(const std::string & szLoggerName, const std::string & szFileName);
		~CLogger();

		void sys_log(const char* c_szFunction, int iLevel, const char* c_szFormat, ...);

	private:
		mutable std::recursive_mutex m_mutex;

		std::string m_szLoggerName;
		std::string m_szFileName;
};

