#include "HaxEngine.h"
#include "main.h"
#include "WMI.h"
#include "ProcessName.h"
#include "Defines.h"

CWMI::CWMI(const std::string & szTargetAppName) : 
	m_szTargetAppName(szTargetAppName), m_bWatcherIsTerminated(false)
{
}

CWMI::~CWMI()
{
	m_szTargetAppName.clear();
	m_bWatcherIsTerminated = false;
}

void CWMI::CheckProcessEvent(wchar_t * wszEventContent)
{
	std::wstring wszContent(wszEventContent);
	std::string szContent(wszContent.begin(), wszContent.end());
	DWORD dwTriggeredPID = std::strtoul(szContent.c_str(), NULL, 0);

	auto bAlive = g_haxApp->GetFunctionsInstance()->ProcessIsItAlive(dwTriggeredPID);

	std::string szProcessName;
	if (bAlive == false)
	{
		auto safeProcHandle = std::make_unique<CSafeProcessHandle>(dwTriggeredPID);
		HANDLE hHandle = safeProcHandle->CreateProcessHandle();

		if (IS_VALID_HANDLE(hHandle)) {
			szProcessName = CProcessName::GetProcessName(hHandle);
			CloseHandle(hHandle);
		}

		safeProcHandle->ClearVeriables();
	}
	if (szProcessName.empty())
		szProcessName = g_haxApp->GetFunctionsInstance()->GetProcessNameFromProcessId(dwTriggeredPID);

	if (szProcessName.empty() == false && szProcessName.find(m_szTargetAppName) != std::string::npos)
		g_haxApp->HaxAppWatcher(dwTriggeredPID, szProcessName, bAlive);
}

bool dumpClassObject(IWbemClassObject* pClassObject, int indentationLevel)
{
	WCHAR __TargetInstance[] = { L'T', L'a', L'r', L'g', L'e', L't', L'I', L'n', L's', L't', L'a', L'n', L'c', L'e', L'\0' }; // TargetInstance
	WCHAR __Handle[] = { L'H', L'a', L'n', L'd', L'l', L'e', L'\0' }; // Handle

	SAFEARRAY* pStrNames;
	HRESULT hError;

	hError = pClassObject->GetNames(NULL, WBEM_FLAG_ALWAYS | WBEM_FLAG_NONSYSTEM_ONLY, NULL, &pStrNames);
	if (FAILED(hError))
		return false;

	long lowerBound, upperBound;
	SafeArrayGetLBound(pStrNames, 1, &lowerBound);
	SafeArrayGetUBound(pStrNames, 1, &upperBound);
	long nbElements = upperBound - lowerBound + 1;

	BSTR name;
	VARIANT varVal;
	CIMTYPE cymType;

	for (int i = 0; i < nbElements; i++)
	{
		name = ((BSTR*)pStrNames->pvData)[i];

		hError = pClassObject->Get(name, 0, &varVal, &cymType, NULL);
		if (SUCCEEDED(hError))
		{		
			if (wcscmp(name, __TargetInstance) == 0) {
				dumpClassObject((IWbemClassObject*)varVal.uintVal, indentationLevel + 1);
			}
			
			else if (cymType == CIM_STRING) {
				if (varVal.bstrVal) {
					//wprintf(L"%*s%s : %s\n", indentationLevel * 4, L"", name, varVal.bstrVal);

					if (wcsstr(name, __Handle))
						g_haxApp->GetWMIInstance()->CheckProcessEvent(varVal.bstrVal);
				}
			}

		}
	}

	return true;
}

void CWMI::TerminateWatcher()
{
	m_bWatcherIsTerminated = true;
}

bool CWMI::CreateWatcher()
{
	WCHAR __format[] = { L'%', L'l', L's', L' ', L'%', L'l', L's', L' ', L'%', L'l', L's', L' ', L'\'', L'%', L'l', L's', L'\'', L'\0' }; // %ls %ls %ls '%ls'

	WCHAR __rootcimv2[] = { L'r', L'o', L'o', L't', L'\\', L'c', L'i', L'm', L'v', L'2', L'\0' }; // root\cimv2
	WCHAR __WQL[] = { L'W', L'Q', L'L', L'\0' }; // WQL

	WCHAR __queryFirst[] = { L'S', L'E', L'L', L'E', L'C', L'T', L' ', L'*', L' ', L'F', L'R', L'O', L'M', L'\0' }; // SELECT * FROM
	WCHAR __querySecond[] = { L'W', L'I', L'T', L'H', L'I', L'N', L' ', L'1', L' ', L'W', L'H', L'E', L'R', L'E', L' ', L'T', L'a', L'r', L'g', L'e', L't', L'I', L'n', L's', L't', L'a', L'n', L'c', L'e', L' ', L'I', L'S', L'A', L'\0' }; // WITHIN 1 WHERE TargetInstance ISA
	WCHAR ____InstanceDeletionEvent[] = { L'_', L'_', L'I', L'n', L's', L't', L'a', L'n', L'c', L'e', L'D', L'e', L'l', L'e', L't', L'i', L'o', L'n', L'E', L'v', L'e', L'n', L't', L'\0' }; // __InstanceDeletionEvent
	WCHAR ____InstanceCreationEvent[] = { L'_', L'_', L'I', L'n', L's', L't', L'a', L'n', L'c', L'e', L'C', L'r', L'e', L'a', L't', L'i', L'o', L'n', L'E', L'v', L'e', L'n', L't', L'\0' }; // __InstanceCreationEvent
	WCHAR __Win32_Process[] = { L'W', L'i', L'n', L'3', L'2', L'_', L'P', L'r', L'o', L'c', L'e', L's', L's', L'\0' }; // Win32_Process


	wchar_t wcBuffer_Creation[2048];
	swprintf(wcBuffer_Creation, __format, __queryFirst, ____InstanceCreationEvent, __querySecond, __Win32_Process);

	wchar_t wcBuffer_Deletion[2048];
	swprintf(wcBuffer_Deletion, __format, __queryFirst, ____InstanceDeletionEvent, __querySecond, __Win32_Process);


	HRESULT hError = S_OK;


	hError = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hError) return false;

	hError = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (hError) return false;

	IWbemLocator* pLoc = NULL;
	hError = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
	if (hError) return false;

	IWbemServices* pWbemServices = NULL;
	hError = pLoc->ConnectServer(__rootcimv2, NULL, NULL, NULL, WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &pWbemServices);
	if (hError) return false;

	DEBUG_LOG(LL_SYS, "WMI Watcher started! Target: %s", m_szTargetAppName.c_str());

	EventSink eventCreated(true);
	pWbemServices->ExecNotificationQueryAsync(__WQL, wcBuffer_Creation, 0, NULL, &eventCreated);

	EventSink eventTerminated(false);
	pWbemServices->ExecNotificationQueryAsync(__WQL, wcBuffer_Deletion, 0, NULL, &eventTerminated);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) && !m_bWatcherIsTerminated) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return false;
}

