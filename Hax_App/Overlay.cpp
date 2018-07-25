#include "HaxAppMain.h"
#include "Overlay.h"
#include "Hax_ROS_Esp.h"

extern std::shared_ptr <CROS_ESP_Hax> g_pROSHax;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_ACTIVATE:
		case WM_KEYDOWN:
		case WM_PAINT:
			return 0;

		case WM_ERASEBKGND:
			SendMessage(hWnd, WM_PAINT, NULL, NULL);
			return TRUE;

		case WM_SIZE:
			g_pROSHax->GetOverlayMgr()->GetOverlayBase()->ResetD3D();
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

COverlayMgr::COverlayMgr(HWND hWnd, TOnDrawCallback pOnDraw) :
	m_hWnd(hWnd), m_pOnDrawCallback(pOnDraw)
{
}

bool COverlayMgr::InitOverlayBase()
{
	m_pOverlayBase = std::make_shared<COverlay>(m_pOnDrawCallback);
	if (!m_pOverlayBase || !m_pOverlayBase.get())
	{
		printf("g_pOverlayHelper can not allocated.\n");
		return false;
	}
	DEBUG_LOG(LL_SYS, "g_pOverlayHelper: %p->%p", m_pOverlayBase, m_pOverlayBase.get());
	return true;
}

DWORD WINAPI COverlayMgr::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (COverlayMgr*)lpParam;
	return This->ThreadRoutine();
}

DWORD COverlayMgr::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "Overlay routine started!");

	m_pOverlayBase->SetupWindow(m_hWnd, WindowProc);
	m_pOverlayBase->OnUpdate();

	return 0;
}

bool COverlayMgr::CreateOverlayThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "Overlay thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

