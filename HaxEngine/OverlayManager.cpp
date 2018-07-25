#include "HaxEngine.h"
#include "main.h"
#include "Defines.h"
#include "OverlayManager.h"

static std::string g_szRandomClass = "AwzumH4x";

COverlay::COverlay(TOnDrawCallback pOnDraw) :
	m_pOnDraw(pOnDraw),
	m_iScreenWidth(1366), m_iScreenHeight(768),
	m_pDirect3DEx(nullptr), m_pD3DDeviceEx(nullptr), m_pD3Font(nullptr), m_pD3Line(nullptr),
	m_hOwnProcessWnd(NULL), m_hOverlayWnd(NULL)
{
	ZeroMemory(&m_Parameters,	sizeof(m_Parameters));
	ZeroMemory(&m_margins,		sizeof(m_margins));
}

bool COverlay::OnUpdate()
{
	MSG	msg;

	while (TRUE) 
	{
		ZeroMemory(&msg, sizeof(msg));

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				DEBUG_LOG(LL_CRI, "Quit message handled!\n");
				Sleep(2000);
				exit(0);
				return false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		static int iFrameCounter = 1;
		if ((iFrameCounter % 10) == 0)
		{
			if (!m_hOverlayWnd || !m_hOwnProcessWnd || !IsWindow(m_hOverlayWnd) || !IsWindow(m_hOwnProcessWnd))
			{
				DEBUG_LOG(LL_ERR, "OnUpdate fail! Overlay: %p(%d) Own proc: %p(%d)", m_hOverlayWnd, IsWindow(m_hOverlayWnd), m_hOwnProcessWnd, IsWindow(m_hOwnProcessWnd));
				Shutdown();
				return false;
			}

//			ScaleOverlayWindow();
			iFrameCounter = 0;
		}

		OnRender();

		iFrameCounter++;

		Sleep(1);
	}
	return true;
}

void COverlay::OnRender()
{
	if (m_pD3DDeviceEx)
	{
		if (SUCCEEDED(m_pD3DDeviceEx->Clear(0, nullptr, D3DCLEAR_TARGET, 0x00000000, 1.f, 0)))
		{
			if (m_hOwnProcessWnd == GetForegroundWindow())
			{
				if (SUCCEEDED(m_pD3DDeviceEx->BeginScene()))
				{
					m_pD3DDeviceEx->GetViewport(&m_Viewport);
					m_pOnDraw(); // callback

					m_pD3DDeviceEx->EndScene();
				}
			}

			m_pD3DDeviceEx->PresentEx(nullptr, nullptr, NULL, nullptr, 0);
		}
	}
}

bool COverlay::InitializeD3D(HWND hWnd)
{
	auto hRet = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pDirect3DEx);
	if (FAILED(hRet))
	{
		printf("Direct3DCreate9Ex fail! Error: %u Ret: %p\n", GetLastError(), (void*)hRet);
		return false;
	}

	RECT rect;
	if (!GetWindowRect(m_hOwnProcessWnd, &rect))
	{
		printf("GetWindowRect fail! Error: %u\n", GetLastError());
		return false;
	}

	m_iScreenWidth	= rect.right - rect.left;
	m_iScreenHeight = rect.bottom - rect.top;

	ZeroMemory(&m_Parameters, sizeof(m_Parameters));

	m_Parameters.Windowed = TRUE;
	m_Parameters.hDeviceWindow = hWnd;
	m_Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
//	m_Parameters.BackBufferCount = 1;
	m_Parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_Parameters.BackBufferWidth = m_iScreenWidth;
	m_Parameters.BackBufferHeight = m_iScreenHeight;
	m_Parameters.EnableAutoDepthStencil = TRUE;
	m_Parameters.AutoDepthStencilFormat = D3DFMT_D16;
	m_Parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_Parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	DWORD dwMSQAAQuality = 0;
	if (SUCCEEDED(m_pDirect3DEx->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, true, D3DMULTISAMPLE_NONMASKABLE, &dwMSQAAQuality)))
	{
		m_Parameters.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
		m_Parameters.MultiSampleQuality = dwMSQAAQuality - 1;
	}

	hRet = m_pDirect3DEx->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_Parameters, NULL, &m_pD3DDeviceEx);
	if (FAILED(hRet))
	{
		printf("CreateDevice fail! Error: %u Ret: %p\n", GetLastError(), (void*)hRet);
		return false;
	}

	if (!m_pD3DDeviceEx)
	{
		printf("m_Direct3DDevice not found\n");
		return false;
	}

	hRet = D3DXCreateFontA(m_pD3DDeviceEx, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_pD3Font);
	if (FAILED(hRet))
	{
		printf("D3DXCreateFontA fail! Error: %u Ret: %p\n", GetLastError(), (void*)hRet);
		return false;
	}

	hRet = D3DXCreateLine(m_pD3DDeviceEx, &m_pD3Line);
	if (FAILED(hRet))
	{
		printf("D3DXCreateLine fail! Error: %u Ret: %p\n", GetLastError(), (void*)hRet);
		return false;
	}

	return true;
}

void COverlay::ResetD3D()
{
	if (m_pD3DDeviceEx)
	{
		if (m_pD3Font)
		{
			m_pD3Font->Release();
			m_pD3Font = NULL;
		}

		if (m_pD3Line)
		{
			m_pD3Line->Release();
			m_pD3Line = NULL;
		}

		ScaleOverlayWindow();

		m_Parameters.BackBufferWidth = m_iScreenWidth;
		m_Parameters.BackBufferHeight = m_iScreenHeight;
		m_pD3DDeviceEx->Reset(&m_Parameters);
	}
}

void COverlay::Shutdown()
{
	if (m_pDirect3DEx)
	{
		m_pDirect3DEx->Release();
		m_pDirect3DEx = NULL;
	}

	if (m_pD3DDeviceEx)
	{
		m_pD3DDeviceEx->Release();
		m_pD3DDeviceEx = NULL;
	}

	DestroyWindow(m_hOverlayWnd);
	m_hOverlayWnd = NULL;

	UnregisterClassA(g_szRandomClass.c_str(), GetModuleHandleA(NULL));
}

bool COverlay::ScaleOverlayWindow()
{
	if (!IsWindow(m_hOwnProcessWnd))
	{
		printf("Window is not valid!\n");
		return false;
	}

	static RECT check_rect;
	if (!GetWindowRect(m_hOwnProcessWnd, &check_rect))
	{
		if (GetLastError() == ERROR_INVALID_WINDOW_HANDLE)
		{
			printf("Own process window not found!\n");
			return false;
		}

		printf("GetWindowRect(loop) fail! Error: %u\n", GetLastError());
		return false;
	}

	if (!IsWindowVisible(m_hOwnProcessWnd))
		return false;

	static RECT rect_client;
	GetClientRect(m_hOwnProcessWnd, &rect_client);

	m_iScreenWidth = rect_client.right - rect_client.left;
	m_iScreenHeight = rect_client.bottom - rect_client.top;

	static RECT rect_window;
	GetWindowRect(m_hOwnProcessWnd, &rect_window);

	static POINT ptDifference;
	ptDifference.x = ptDifference.y = 0;

	ClientToScreen(m_hOwnProcessWnd, &ptDifference);

	auto x = rect_window.left + (ptDifference.x - rect_window.left);
	auto y = rect_window.top + (ptDifference.y - rect_window.top);

	if (x == 0)
	{
		x--;
		m_iScreenWidth++;
	}

	if (y == 0)
	{
		y--;
		m_iScreenHeight++;
	}

//	m_margins = { 0, 0, m_iScreenWidth, m_iScreenHeight };
	m_margins = { -1 };

	MoveWindow(m_hOverlayWnd, x, y, m_iScreenWidth, m_iScreenHeight, TRUE);
	return true;
}

void COverlay::SetupWindow(HWND hWnd, WNDPROC WindowProc) 
{
	m_hOwnProcessWnd = hWnd;

	if (!IsWindow(m_hOwnProcessWnd))
	{
		printf("Own process window not valid! Error: %u\n", GetLastError());
		return;
	}

	RECT rect;
	if (!GetWindowRect(m_hOwnProcessWnd, &rect))
	{
		printf("GetWindowRect fail! Error: %u\n", GetLastError());
		return;
	}

	if (!WindowProc)
	{
		printf("Null window routine!\n");
		return;
	}

	WNDCLASSEX WindowClassEx;
	ZeroMemory(&WindowClassEx, sizeof(WNDCLASSEX));

	WindowClassEx.cbSize		= sizeof(WNDCLASSEX);
	WindowClassEx.style			= CS_HREDRAW | CS_VREDRAW;
	WindowClassEx.lpfnWndProc	= WindowProc;
	WindowClassEx.hInstance		= GetModuleHandle(0);
	WindowClassEx.hCursor		= LoadCursor(0, IDC_ARROW);
	WindowClassEx.hIcon			= LoadIcon(0, IDI_APPLICATION);
	WindowClassEx.hIconSm		= LoadIcon(0, IDI_APPLICATION);
	WindowClassEx.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));

	auto iRandomSize			= g_haxApp->GetFunctionsInstance()->GetRandomInt(6, 18);
	g_szRandomClass				= g_haxApp->GetFunctionsInstance()->GetRandomString(iRandomSize);;
	WindowClassEx.lpszClassName = g_szRandomClass.c_str();

	if (!RegisterClassExA(&WindowClassEx))
	{
		printf("RegisterClassExA fail! Error: %u\n", GetLastError());
		return;
	}

	m_hOverlayWnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, WindowClassEx.lpszClassName, "", WS_POPUP, rect.left, rect.top, m_iScreenWidth, m_iScreenHeight, NULL, NULL, WindowClassEx.hInstance, NULL);
	if (!m_hOverlayWnd)
	{
		printf("m_hOverlayWnd fail! Error: %u\n", GetLastError());
		return;
	}
	printf("Overlay: %p\n", m_hOverlayWnd);

	if (!SetLayeredWindowAttributes(m_hOverlayWnd, RGB(0, 0, 0), 255, ULW_COLORKEY | LWA_ALPHA))
	{
		printf("SetLayeredWindowAttributes fail! Error: %u\n", GetLastError());
		return;
	}

	if (FAILED(DwmExtendFrameIntoClientArea(m_hOverlayWnd, &m_margins)))
	{
		printf("DwmExtendFrameIntoClientArea fail! Error: %u\n", GetLastError());
		return;
	}

	if (!ShowWindow(m_hOverlayWnd, SW_SHOW))
	{
		printf("ShowWindow fail! Error: %u\n", GetLastError());
		return;
	}

	if (!UpdateWindow(m_hOverlayWnd))
	{
		printf("UpdateWindow fail! Error: %u\n", GetLastError());
		return;
	}

	ScaleOverlayWindow();

	InitializeD3D(m_hOverlayWnd);
}


// Draw functions
void COverlay::DrawCircle(int X, int Y, int radius, int numSides, DWORD color)
{
	if (!m_pD3Line)
		D3DXCreateLine(m_pD3DDeviceEx, &m_pD3Line);

	D3DXVECTOR2 Line[128];
	double Step = M_PI * 2.0 / numSides;
	int Count = 0;
	for (double a = 0; a < M_PI*2.0; a += Step)
	{
		float X1 = radius * cos(a) + X;
		float Y1 = radius * sin(a) + Y;
		double X2 = radius * cos(a + Step) + X;
		double Y2 = radius * sin(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	m_pD3Line->Draw(Line, Count, color);
}

void COverlay::FillArea(float x, float y, float width, float height, D3DCOLOR color)
{
	if (!m_pD3DDeviceEx)
		return;

	if (!m_pD3Line)
		D3DXCreateLine(m_pD3DDeviceEx, &m_pD3Line);

	D3DXVECTOR2 vectorLine[2];
	m_pD3Line->SetWidth(width);

	vectorLine[0].x = x + width / 2;
	vectorLine[0].y = y;
	vectorLine[1].x = x + width / 2;
	vectorLine[1].y = y + height;

	m_pD3Line->Draw(vectorLine, 2, color);
}

void COverlay::FillRectangle(double x, double y, double w, double h, D3DCOLOR color)
{
	if (!m_pD3DDeviceEx)
		return;
	
	struct Vertex
	{
		double x, y, z, h;
		D3DCOLOR color;
	};

	Vertex vertices[4] =
	{
		x,  y, 0.0f, 1.0f, color,
		x + w, y, 0.0f, 1.0f, color,

		x + w, y + h, 0.0f, 1.0f, color,
		x,  y + h, 0.0f, 1.0f, color,
	};

	m_pD3DDeviceEx->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pD3DDeviceEx->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3DDeviceEx->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_pD3DDeviceEx->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
}

void COverlay::DrawBox(float x, float y, float width, float height, D3DCOLOR color)
{
	if (!m_pD3DDeviceEx)
		return;

	if (!m_pD3Line)
		D3DXCreateLine(m_pD3DDeviceEx, &m_pD3Line);

	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	m_pD3Line->SetWidth(1);
	m_pD3Line->Draw(points, 5, color);
}

void COverlay::DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)
{
	if (!m_pD3DDeviceEx)
		return;

	if (!m_pD3Font) {
		D3DXCreateFontA(m_pD3DDeviceEx, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_pD3Font);
		return;
	}

	char buf[1024] = { 0 };

	va_list va_alist;
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);

	RECT FontPos = { x, y, x + 120, y + 16 };
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void COverlay::DrawLine(int x, int y, int xx, int yy, DWORD color, float flThickness)
{
	if (!m_pD3DDeviceEx)
		return;

	if (!m_pD3Line)
		D3DXCreateLine(m_pD3DDeviceEx, &m_pD3Line);

	m_pD3Line->SetWidth(1.f);
	m_pD3Line->SetPattern(0xFFFFFFFF);
	m_pD3Line->SetAntialias(TRUE);

	if (flThickness)
		m_pD3Line->SetWidth(flThickness);

	D3DXVECTOR2 points[2];
	points[0].x = (FLOAT)x;
	points[0].y = (FLOAT)y;
	points[1].x = (FLOAT)xx;
	points[1].y = (FLOAT)yy;
	m_pD3Line->Draw(points, 2, color);
}

void COverlay::Rect(int x, int y, int w, int h, DWORD color)
{
	if (!m_pD3DDeviceEx)
		return;

	static D3DRECT rect_angle;
	rect_angle = { x,y, x + w, y + h };

	m_pD3DDeviceEx->Clear(1, &rect_angle, D3DCLEAR_TARGET, color, 1.f, 0);
}

void COverlay::RectOutlined(int x, int y, int w, int h, int border_thickness, DWORD color, DWORD outlined)
{
	Rect(x, y, w, h, color);
	BorderBox(x - border_thickness, y - border_thickness, w + border_thickness, h + border_thickness, border_thickness, outlined);
}

void COverlay::BorderBox(int x, int y, int w, int h, int thickness, DWORD color)
{
	Rect(x, y, w, thickness, color);
	Rect(x, y, thickness, h, color);
	Rect(x + w, y, thickness, h, color);
	Rect(x, y + h, w + thickness, thickness, color);
}

void COverlay::BorderBoxOutlined(int x, int y, int w, int h, int thickness, int border_thickness, DWORD color, DWORD outlined)
{
	BorderBox(x, y, w, h, thickness, color);
	BorderBox(x - border_thickness, y - border_thickness, w + border_thickness + thickness, h + border_thickness + thickness, border_thickness, outlined);
	BorderBox(x + thickness, y + thickness, w - border_thickness - thickness, h - border_thickness - thickness, border_thickness, outlined);
}

void COverlay::FillRGB(float x, float y, float w, float h, int r, int g, int b, int a)
{
	if (!m_pD3DDeviceEx)
		return;

	if (!m_pD3Line)
		D3DXCreateLine(m_pD3DDeviceEx, &m_pD3Line);

	m_pD3Line->SetWidth(w);

	D3DXVECTOR2 vLine[2];
	vLine[0].x = x + w / 2;
	vLine[0].y = y;
	vLine[1].x = x + w / 2;
	vLine[1].y = y + h;

	m_pD3Line->Begin();
	m_pD3Line->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
	m_pD3Line->End();
}

void COverlay::DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa)
{
	DrawBox(x, y, w, h, D3DCOLOR_RGBA(r, g, b, a));
	FillRGB(x, y, w, h, rr, gg, bb, a);
}
void COverlay::DrawHealthBar(float x, float y, float w, float h, int r, int g)
{
	FillRGB(x, y, w, h, r, g, 0, 255);
}
void COverlay::DrawHealthBarBack(float x, float y, float w, float h)
{
	FillRGB(x, y, w, h, 0, 0, 0, 255);
}

