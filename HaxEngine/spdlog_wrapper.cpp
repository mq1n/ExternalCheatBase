#include "HaxEngine.h"
#include "main.h"
#include "spdlog_wrapper.h"
#include "XOR.h"

static void LogErrorHandler(const std::string & szMessage)
{
	Logf(CUSTOM_LOG_FILENAME, XOR("Log error handled: %s\n"), szMessage.c_str());
}

CLogger::CLogger(const std::string & szLoggerName, const std::string & szFileName) :
	m_szLoggerName(szLoggerName), m_szFileName(szFileName)
{
	try
	{
		std::vector<spdlog::sink_ptr> sinks;

		sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(m_szFileName.c_str()));

		auto logger = std::make_shared<spdlog::logger>(m_szLoggerName.c_str(), sinks.begin(), sinks.end());
		logger->set_error_handler(LogErrorHandler);
		spdlog::register_logger(logger);
	}
	catch (const spdlog::spdlog_ex & ex)
	{
		Logf(CUSTOM_LOG_FILENAME, XOR("There has to be an exception throw on CLogger::InitLogger(spdlog::spdlog_ex): %s\n"), ex.what());
		abort();
	}
	catch (DWORD dwNumber)
	{
		Logf(CUSTOM_LOG_FILENAME, XOR("There has to be an exception throw on CLogger::InitLogger(w/ number): %p\n"), dwNumber);
		abort();
	}
	catch (...)
	{
		Logf(CUSTOM_LOG_FILENAME, XOR("There has to be an exception throw on CLogger::InitLogger(w/o information!)\n"));
		abort();
	}
}

CLogger::~CLogger()
{
}

void CLogger::sys_log(const char* c_szFunction, int iLevel, const char* c_szFormat, ...)
{
	std::lock_guard <std::recursive_mutex> __lock(m_mutex);

	auto logger = spdlog::get(m_szLoggerName.c_str());
	if (!logger) {
		Logf(CUSTOM_LOG_FILENAME, XOR("Logger class not found!\n"));
		return;
	}

	char cTmpString[8192];
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf(cTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	char cFinalString[9000];
	sprintf(cFinalString, XOR("%s | %s"), c_szFunction, cTmpString);

	try
	{
		switch (iLevel)
		{
			case LL_SYS: logger->info(cFinalString); break;
			case LL_CRI: logger->critical(cFinalString); break;
			case LL_ERR: logger->error(cFinalString); break;
			case LL_DEV: logger->debug(cFinalString); break;
			case LL_TRACE: logger->trace(cFinalString); break;
			case LL_WARN: logger->warn(cFinalString); break;
		}
		logger->flush();
	}
	catch (const spdlog::spdlog_ex & ex)
	{
		Logf(CUSTOM_LOG_FILENAME, XOR("There has to be an exception throw on CLogger::sys_log(spdlog::spdlog_ex): %s\n"), ex.what());
		abort();
	}
	catch (DWORD dwNumber)
	{
		Logf(CUSTOM_LOG_FILENAME, XOR("There has to be an exception throw on CLogger::sys_log(w/ number): %p\n"), dwNumber);
		abort();
	}
	catch (...)
	{
		Logf(CUSTOM_LOG_FILENAME, XOR("There has to be an exception throw on CLogger::sys_log(w/o information!)\n"));
		abort();
	}
}

