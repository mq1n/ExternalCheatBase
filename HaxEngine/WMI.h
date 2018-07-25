#pragma once

#include "main.h"

bool dumpClassObject(IWbemClassObject* pClassObject, int indentationLevel = 0);

class EventSink : public IWbemEventSink
{
	private:
		LONG m_lRef;
		bool bDone;
		bool m_processCreated;

	public:
		EventSink(bool processCreated) {
			m_processCreated = processCreated;
		}

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) {
			if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
				*ppvObject = (IWbemEventSink*) this;
				AddRef();
				return WBEM_S_NO_ERROR;
			}
			else return E_NOINTERFACE;
		}

		virtual ULONG STDMETHODCALLTYPE AddRef(void) {
			return InterlockedIncrement(&m_lRef);
		}

		virtual ULONG STDMETHODCALLTYPE Release(void) {
			LONG lRef = InterlockedDecrement(&m_lRef);
			if (lRef == 0)
				delete this;
			return lRef;
		}

		virtual HRESULT STDMETHODCALLTYPE Indicate(long lObjectCount, __RPC__in_ecount_full(lObjectCount) IWbemClassObject **apObjArray) {
			for (long i = 0; i < lObjectCount; i++)
			{
				IWbemClassObject* pObj = apObjArray[i];
				dumpClassObject(pObj);
			}
			return WBEM_S_NO_ERROR;
		};

		virtual HRESULT STDMETHODCALLTYPE SetStatus(long lFlags, HRESULT hResult, __RPC__in_opt BSTR strParam, __RPC__in_opt IWbemClassObject *pObjParam) {
			return WBEM_S_NO_ERROR;
		};

		virtual HRESULT STDMETHODCALLTYPE SetSinkSecurity(long lSDLength, __RPC__in_ecount_full(lSDLength) BYTE *pSD) {
			return WBEM_S_NO_ERROR;
		}

		virtual HRESULT STDMETHODCALLTYPE IsActive(void) {
			return WBEM_S_NO_ERROR;
		}

		virtual HRESULT STDMETHODCALLTYPE GetRestrictedSink(long lNumQueries, __RPC__in_ecount_full(lNumQueries) const LPCWSTR *awszQueries, __RPC__in_opt IUnknown *pCallback, __RPC__deref_out_opt IWbemEventSink **ppSink) {
			return WBEM_S_NO_ERROR;
		}

		virtual HRESULT STDMETHODCALLTYPE SetBatchingParameters(LONG lFlags, DWORD dwMaxBufferSize, DWORD dwMaxSendLatency) {
			return WBEM_S_NO_ERROR;
		}
};

class CWMI : public std::enable_shared_from_this<CWMI>
{
	public:
		CWMI(const std::string & szTargetAppName);
		~CWMI();

		bool CreateWatcher();
		void TerminateWatcher();

		void CheckProcessEvent(wchar_t * wszEventContent);

	private:
		bool		m_bWatcherIsTerminated;
		std::string m_szTargetAppName;
};

