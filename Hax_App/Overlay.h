#pragma once
#include "../HaxEngine/OverlayManager.h"
#include "../HaxEngine/OverlayColorCodes.h"

class COverlayMgr
{
	typedef std::function<void()> TOnDrawCallback;

	public:
		COverlayMgr(HWND hWnd, TOnDrawCallback pOnDraw);
		~COverlayMgr() = default;

		bool InitOverlayBase();
		auto GetOverlayBase() { return m_pOverlayBase; };
	
		bool CreateOverlayThread();

	protected:
		DWORD					ThreadRoutine(void);
		static DWORD WINAPI		StartThreadRoutine(LPVOID lpParam);

	private:
		HWND						m_hWnd;
		TOnDrawCallback				m_pOnDrawCallback;
		std::shared_ptr<COverlay>	m_pOverlayBase;
		HANDLE						m_hThread;
};

