#pragma once
#include <d3d/d3d9.h>
#include <d3d/d3dx9.h>
#include <Dwmapi.h> 

class COverlay
{
	typedef std::function<void()> TOnDrawCallback;

	public:
		COverlay(TOnDrawCallback pOnDraw);
		~COverlay() = default;

		bool InitializeD3D(HWND hWnd);
		void ResetD3D();
		void Shutdown();

		void SetupWindow(HWND hWnd, WNDPROC WindowProc);
		bool ScaleOverlayWindow();

		bool OnUpdate();
		void OnRender();

	public:
		void FillArea(float x, float y, float width, float height, D3DCOLOR color);
		void FillRectangle(double x, double y, double w, double h, D3DCOLOR color);

		void DrawCircle(int X, int Y, int radius, int numSides, DWORD Color);
		void DrawBox(float x, float y, float width, float height, D3DCOLOR color);
		void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char * fmt, ...);

		void DrawLine(int x, int y, int xx, int yy, DWORD color, float flThickness = 0.f);

		void Rect(int x, int y, int w, int h, DWORD color);
		void RectOutlined(int x, int y, int w, int h, int border_thickness, DWORD color, DWORD outlined);

		void BorderBox(int x, int y, int w, int h, int thickness, DWORD color);
		void BorderBoxOutlined(int x, int y, int w, int h, int thickness, int border_thickness, DWORD color, DWORD outlined);

		void FillRGB(float x, float y, float w, float h, int r, int g, int b, int a);
		void DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa);
		void DrawHealthBar(float x, float y, float w, float h, int r, int g);
		void DrawHealthBarBack(float x, float y, float w, float h);

	public:
		auto GetMargins() { return m_margins; };
		auto GetD3DFont() { return m_pD3Font; };
		auto GetViewPort() { return m_Viewport; };

	private:
		TOnDrawCallback		m_pOnDraw;

		int					m_iScreenWidth;
		int					m_iScreenHeight;

		LPDIRECT3D9EX		m_pDirect3DEx;
		LPDIRECT3DDEVICE9EX	m_pD3DDeviceEx;
		LPD3DXFONT			m_pD3Font;
		LPD3DXLINE			m_pD3Line;
		D3DVIEWPORT9		m_Viewport;

		D3DPRESENT_PARAMETERS m_Parameters;

		HWND				m_hOwnProcessWnd;
		HWND				m_hOverlayWnd;

		MARGINS				m_margins;
};

