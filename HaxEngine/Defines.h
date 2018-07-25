#pragma once

#ifndef USE_SELF_SECURITY
	#define USE_SELF_SECURITY false
#endif

#ifndef DEBUG_LOG
	#ifdef _DEBUG
		#define DEBUG_LOG(level, log, ...) g_haxApp->LogHelperInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define DEBUG_LOG(level, log, ...)
	#endif
#endif

