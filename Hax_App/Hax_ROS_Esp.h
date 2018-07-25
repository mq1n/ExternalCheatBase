#pragma once
#include "Overlay.h"
#include "../HaxEngine/Vector2.h"
#include "../HaxEngine/Vector3.h"
#include "../HaxEngine/Vector4.h"

class CROS_ESP_Hax
{
	public:
		CROS_ESP_Hax() = default;
		~CROS_ESP_Hax() = default;

		void Initialize();
		void Finalize();

		void OnDraw();

		bool WorldToScreen(Vector3 pos, Vector2& screen, int windowWidth, int windowHeight);

	public:
		auto GetOverlayMgr() { return m_pOverlayMgr; };

	private:
		std::shared_ptr <COverlayMgr>		m_pOverlayMgr;
		std::shared_ptr <CProcessHelper>	m_pProcessHelper;
		std::shared_ptr <CMemoryHelper>		m_pMemoryHelper;

		DWORD	m_dwBaseModule;

		HWND	m_hWnd;
		int		m_iWidth;
		int		m_iHeight;
		int		m_iTop;
		int		m_iLeft;
};

