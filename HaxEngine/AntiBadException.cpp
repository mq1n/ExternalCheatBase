#include "HaxEngine.h"
#include "main.h"
#include "Defines.h"
#include "Antis.h"

LONG WINAPI vehFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo && ExceptionInfo->ContextRecord)
	{
		if (ExceptionInfo->ContextRecord->Dr0 != 0 || ExceptionInfo->ContextRecord->Dr1 != 0 || ExceptionInfo->ContextRecord->Dr2 != 0 ||
			ExceptionInfo->ContextRecord->Dr3 != 0 || ExceptionInfo->ContextRecord->Dr7 != 0)
		{
			DEBUG_LOG(LL_ERR, "VehFilter: Cleaned debug registers!");

			ExceptionInfo->ContextRecord->Dr0 = 0;
			ExceptionInfo->ContextRecord->Dr1 = 0;
			ExceptionInfo->ContextRecord->Dr2 = 0;
			ExceptionInfo->ContextRecord->Dr3 = 0;
			ExceptionInfo->ContextRecord->Dr7 = 0;
		}
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord && ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		DEBUG_LOG(LL_CRI, "VehFilter: EXCEPTION_SINGLE_STEP detected!!");
		abort();
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord && ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		DEBUG_LOG(LL_CRI, "VehFilter: EXCEPTION_BREAKPOINT detected!!");
		abort();
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord && ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
	{
		DEBUG_LOG(LL_CRI, "VehFilter: EXCEPTION_GUARD_PAGE detected!!");
		abort();
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI sehFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo && ExceptionInfo->ContextRecord)
	{
		if (ExceptionInfo->ContextRecord->Dr0 != 0 || ExceptionInfo->ContextRecord->Dr1 != 0 || ExceptionInfo->ContextRecord->Dr2 != 0 ||
			ExceptionInfo->ContextRecord->Dr3 != 0 || ExceptionInfo->ContextRecord->Dr7 != 0)
		{
			DEBUG_LOG(LL_ERR, "SehFilter: Cleaned debug registers!");

			ExceptionInfo->ContextRecord->Dr0 = 0;
			ExceptionInfo->ContextRecord->Dr1 = 0;
			ExceptionInfo->ContextRecord->Dr2 = 0;
			ExceptionInfo->ContextRecord->Dr3 = 0;
			ExceptionInfo->ContextRecord->Dr7 = 0;
		}
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord && ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		DEBUG_LOG(LL_CRI, "SehFilter: EXCEPTION_SINGLE_STEP detected!!");
		abort();
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord && ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		DEBUG_LOG(LL_CRI, "SehFilter: EXCEPTION_BREAKPOINT detected!!");
		abort();
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord && ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
	{
		DEBUG_LOG(LL_CRI, "SehFilter: EXCEPTION_GUARD_PAGE detected!!");
		abort();
	}
	return EXCEPTION_CONTINUE_SEARCH;
}


static bool InitializeSEH()
{
	return (SetUnhandledExceptionFilter(sehFilter) != NULL);
}

static bool InitializeVEH()
{
	return (AddVectoredExceptionHandler(0, vehFilter) != NULL);
}


bool CAntis::InitExceptionHandlers()
{
	if (!InitializeSEH() || !(InitializeVEH())) {
		DEBUG_LOG(LL_ERR, "InitExceptionHandlers: Exception handlers can not created!");
		return false;
	}
	return true;
}

