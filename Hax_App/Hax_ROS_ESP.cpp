#include "HaxAppMain.h"
#include "../HaxEngine/HaxApp.h"
#include "Hax_ROS_Esp.h"
#include "Overlay.h"

#include "../HaxEngine/MemoryHelper.h"

std::shared_ptr <CROS_ESP_Hax> g_pROSHax = nullptr;

std::map<int /* iCode */, std::string /* szName */> mItemNames =
{
    { 0, "Nothing" },
    { 1, "Lv 1 helmet" },
    { 2, "Lv 2 helmet" },
    { 3, "Lv 3 helmet" },
    { 5, "Lv 1 Armor" },
    { 6, "Lv 2 Armor" },
    { 7, "Lv 3 Armor" },
    { 8, "Lv 1 Backpack" },
    { 9, "Lv 2 Backpack" },
    { 10, "Lv 3 Backpack" }
};

#define ESP_PLAYER_DISTANCE	1000
#define ESP_LOOT_DISTANCE	200
#define CLIENT_ADR			0x1EF8AF8
#define LOCAL_PLAYER		0x1EFA7F4
#define OBJECTS				0xE9C
#define VIEW_MATRIX			0x1CF6E18
#define GRASS				0x01C3F19C

void CROS_ESP_Hax::Initialize()
{
	BOOL bEnabled = false;
	if (DwmIsCompositionEnabled(&bEnabled) != S_OK || !bEnabled)
	{
		DEBUG_LOG(LL_ERR, "Aero not enabled.");
		return;
	}

	m_pProcessHelper = std::make_shared<CProcessHelper>("ros.exe");
	if (!m_pProcessHelper || !m_pProcessHelper.get())
	{
		DEBUG_LOG(LL_ERR, "pProcessHelper allocation fail!");
		return;
	}
	DEBUG_LOG(LL_SYS, "m_pProcessHelper: %p->%p", m_pProcessHelper, m_pProcessHelper.get());

	m_pProcessHelper->SetOpenType(EOPenTypes::OPEN_STANDART);

	if (!m_pProcessHelper->OpenTarget() || !IS_VALID_HANDLE(m_pProcessHelper->GetHandle()))
	{
		DEBUG_LOG(LL_ERR, "Target process can not opened!");
		return;
	}
	DEBUG_LOG(LL_SYS, "Target process(%u) handle created: %p", m_pProcessHelper->GetProcessId(), m_pProcessHelper->GetHandle());

	m_hWnd = m_pProcessHelper->GetWindow();
	if (!m_hWnd)
	{
		DEBUG_LOG(LL_ERR, "Target process window not found!");
		return;
	}
	DEBUG_LOG(LL_SYS, "Target window: %p", m_hWnd);

	RECT rect;
	if (!GetWindowRect(m_hWnd, &rect))
	{
		DEBUG_LOG(LL_ERR, "Target process window informations can not handled!");
		return;
	}
	m_iWidth = rect.right - rect.left;
	m_iHeight = rect.bottom - rect.top;
	m_iTop = rect.top;
	m_iLeft = rect.left;

	m_pMemoryHelper = std::make_shared<CMemoryHelper>(m_pProcessHelper->GetHandle());
	if (!m_pMemoryHelper || !m_pMemoryHelper.get())
	{
		DEBUG_LOG(LL_ERR, "pMemoryHelper allocation fail!");
		return;
	}
	DEBUG_LOG(LL_SYS, "pMemoryHelper: %p->%p", m_pMemoryHelper, m_pMemoryHelper.get());

	auto pBaseModule = m_pProcessHelper->GetModule("ros.exe");
	if (!pBaseModule.IsValid())
	{
		DEBUG_LOG(LL_ERR, "pBaseModule not valid!");
		return;
	}
	m_dwBaseModule = (DWORD)pBaseModule.GetAddress();

	m_pOverlayMgr = std::make_shared<COverlayMgr>(m_hWnd, std::bind(&CROS_ESP_Hax::OnDraw, this));
	if (!m_pOverlayMgr || !m_pOverlayMgr.get())
	{
		DEBUG_LOG(LL_ERR, "m_pOverlayMgr allocation fail!");
		return;
	}
	DEBUG_LOG(LL_SYS, "m_pOverlayMgr: %p->%p", m_pOverlayMgr, m_pOverlayMgr.get());

	if (!m_pOverlayMgr->InitOverlayBase())
	{
		DEBUG_LOG(LL_ERR, "InitOverlayBase fail!");
		return;
	}

	if (!m_pOverlayMgr->CreateOverlayThread())
	{
		DEBUG_LOG(LL_ERR, "CreateOverlayThread fail!");
		return;
	}
}

void CROS_ESP_Hax::Finalize()
{
	m_pOverlayMgr->GetOverlayBase()->Shutdown();
}


bool CROS_ESP_Hax::WorldToScreen(Vector3 pos, Vector2& screen, int windowWidth, int windowHeight)
{
	auto dwViewMatrixBase	= m_pMemoryHelper->Read<DWORD>(m_dwBaseModule + VIEW_MATRIX, { 4 });
	auto pViewMatrix		= m_pMemoryHelper->ReadArray<float>(dwViewMatrixBase + 196, sizeof(Vector4) * sizeof(float));
	auto ViewMatrix			= pViewMatrix.get();

	Vector4 vector4;
	vector4.x = (float)((double)pos.x * (double)ViewMatrix[0] + (double)pos.y * (double)ViewMatrix[4] + (double)pos.z * (double)ViewMatrix[8]) + ViewMatrix[12];
	vector4.y = (float)((double)pos.x * (double)ViewMatrix[1] + (double)pos.y * (double)ViewMatrix[5] + (double)pos.z * (double)ViewMatrix[9]) + ViewMatrix[13];
	vector4.z = (float)((double)pos.x * (double)ViewMatrix[2] + (double)pos.y * (double)ViewMatrix[6] + (double)pos.z * (double)ViewMatrix[10]) + ViewMatrix[14];
	vector4.w = (float)((double)pos.x * (double)ViewMatrix[3] + (double)pos.y * (double)ViewMatrix[7] + (double)pos.z * (double)ViewMatrix[11]) + ViewMatrix[15];

	if ((double)vector4.w < 0.100000001490116)
		return false;

	Vector3 vector3;
	vector3.x = vector4.x / vector4.w;
	vector3.y = vector4.y / vector4.w;
	vector3.z = vector4.z / vector4.w;
	screen.x = (float)((double)(windowWidth / 2) * (double)vector3.x + ((double)vector3.x + (double)(windowWidth / 2)));
	screen.y = (float)(-((double)(windowHeight / 2) * (double)vector3.y) + ((double)vector3.y + (double)(windowHeight / 2)));
	return true;
}

double GetDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

int GetDistance(Vector3 v1, Vector3 v2, int Divide)
{
	Vector3 vector3;
	vector3.x = v1.x - v2.x;
	vector3.y = v1.y - v2.y;
	vector3.z = v1.z - v2.z;
	return (int)sqrt(pow((double)vector3.x, 2.0) + pow((double)vector3.y, 2.0) + pow((double)vector3.z, 2.0)) / Divide;
}

void CROS_ESP_Hax::OnDraw()
{
	auto dwLocalPlayer	= m_pMemoryHelper->Read<DWORD>(m_dwBaseModule + LOCAL_PLAYER);
	auto dwEntityBegin	= m_pMemoryHelper->Read<DWORD>(m_dwBaseModule + CLIENT_ADR, { OBJECTS, 0 , 0 });
	auto dwEntityEnd	= m_pMemoryHelper->Read<DWORD>(m_dwBaseModule + CLIENT_ADR, { OBJECTS, 0 , 4 });

	auto v3SelfPos = m_pMemoryHelper->Read<Vector3>(dwLocalPlayer + 16);

//	DEBUG_LOG(LL_SYS, "%p | %p - %p", dwLocalPlayer, dwEntityBegin, dwEntityEnd);

	while (dwEntityBegin != dwEntityEnd && dwEntityBegin != dwLocalPlayer)
	{
		auto dwCurrEntity = m_pMemoryHelper->Read<DWORD>(dwEntityBegin + 0xC);
		if (dwCurrEntity)
		{
			dwLocalPlayer = m_pMemoryHelper->Read<DWORD>(m_dwBaseModule + LOCAL_PLAYER);
			auto v3SelfPos = m_pMemoryHelper->Read<Vector3>(dwLocalPlayer + 0x10);

			Vector2 v2SelfScreen;
			if (WorldToScreen(v3SelfPos, v2SelfScreen, m_iWidth, m_iHeight))
			{
				auto v3EntityPos = m_pMemoryHelper->Read<Vector3>(dwCurrEntity + 0x10);

				Vector2 v2EntityScreen;
				if (WorldToScreen(v3EntityPos, v2EntityScreen, m_iWidth, m_iHeight))
				{
					auto iDistance = GetDistance(v3EntityPos, v3SelfPos, 10);
					if (iDistance < ESP_PLAYER_DISTANCE)
					{
						if ((v2EntityScreen.x != 0 || v2EntityScreen.y != 0) && (v2SelfScreen.x != 0 || v2SelfScreen.y != 0) && dwLocalPlayer != dwCurrEntity)
						{
							auto dwEntityTypeBase	= m_pMemoryHelper->Read<DWORD>(dwCurrEntity + 4, { 0xC });
							auto pszEntityType		= m_pMemoryHelper->ReadArray<char>(dwEntityTypeBase, 20);

							auto iEntityHP			= m_pMemoryHelper->Read<int>(dwCurrEntity + 0x100, { 0x14 ,0x98, 0x8 });
							auto iItemID			= m_pMemoryHelper->Read<int>(dwCurrEntity + 0x100, { 0x14 ,0x2C, 0x8 });

							auto v2EntityCenter		= Vector2(v2EntityScreen.x + m_iLeft, v2EntityScreen.y + m_iTop);
							auto v2SelfCenter		= Vector2(v2SelfScreen.x + m_iLeft, v2SelfScreen.y + m_iTop);

							auto szEntityType		= std::string(pszEntityType.get());
							auto bIsPlayer			= (std::string::npos != szEntityType.find("Avatar"));
							auto bIsBot				= (std::string::npos != szEntityType.find("Robot"));

							int tag_offset = -10;

//							DEBUG_LOG(LL_SYS, "Entity: %08X Type: %s HP: %d Pos: %f-%f-%f Distance: %d",
//								dwCurrEntity, entity_type_str.c_str(), iEntityHP, v3EntityPos.x, v3EntityPos.y, v3EntityPos.z, iDistance);

							// Draw line
							if (bIsPlayer)
							{
								m_pOverlayMgr->GetOverlayBase()->DrawLine(v2SelfCenter.x, v2SelfCenter.y, v2EntityCenter.x, v2EntityCenter.y, GREEN);
							}

							// Draw name
							if (bIsPlayer)
							{
								auto dwNameBase		= m_pMemoryHelper->Read<DWORD>(dwCurrEntity + 0x100, { 0x14 , 0x3EC });
								auto pszEntityName	= m_pMemoryHelper->ReadArray<char>(dwNameBase + 0x14, 40);
								auto szEntityName	= std::string(pszEntityName.get());

								m_pOverlayMgr->GetOverlayBase()->DrawString(v2EntityCenter.x - 10, v2EntityCenter.y - 40, RED, m_pOverlayMgr->GetOverlayBase()->GetD3DFont(), szEntityName.c_str());
							}

							// Draw type
							if (iDistance < ESP_LOOT_DISTANCE)
							{
								auto bIsVehicle		= (std::string::npos != szEntityType.find("Land"));
								auto bIsItem		= (std::string::npos != szEntityType.find("DtsProp"));
								auto bIsSupplyBox	= (std::string::npos != szEntityType.find("DtsPlayerHeritage"));
								auto bIsPlane		= (std::string::npos != szEntityType.find("Plane"));
								auto bIsWeapon		= (std::string::npos != szEntityType.find("ClientWeaponEntity"));

								auto szEntityTag	= std::string("Unknown");
								if (bIsPlayer)		szEntityTag = "Human";
								if (bIsBot)			szEntityTag = "Robot";
								if (bIsSupplyBox)	szEntityTag = "Supply";
								if (bIsPlane)		szEntityTag = "Plane";
								if (bIsWeapon)		szEntityTag = "Weapon";
								if (bIsItem)		szEntityTag = mItemNames[iItemID];
								if (bIsVehicle)
								{
									if (std::string::npos != szEntityType.find("LandCar"))				szEntityTag = "Vehicle[Car]";
									else if (std::string::npos != szEntityType.find("LandBike"))		szEntityTag = "Vehicle[Bicycle]";
									else if (std::string::npos != szEntityType.find("LandExpress"))		szEntityTag = "Vehicle[Express]";
									else if (std::string::npos != szEntityType.find("LandMotor"))		szEntityTag = "Vehicle[Motor]";
									else if (std::string::npos != szEntityType.find("LandGlider"))		szEntityTag = "Vehicle[Glider]";
									else																szEntityTag = "Vehicle";
								}

								auto	d3TagColor = ORANGE;
								if		(bIsPlayer || bIsBot)	d3TagColor = GREEN;
								else if (bIsItem)				d3TagColor = RED;

								m_pOverlayMgr->GetOverlayBase()->DrawString(v2EntityCenter.x + tag_offset, v2EntityCenter.y - 20, d3TagColor, m_pOverlayMgr->GetOverlayBase()->GetD3DFont(), szEntityTag.c_str());
								tag_offset += 12 * szEntityTag.size();
							}

							// Draw distance
							if (bIsPlayer)
							{
								auto szEntityTag = std::to_string(iDistance) + "M";

								m_pOverlayMgr->GetOverlayBase()->DrawString(v2EntityCenter.x + tag_offset, v2EntityCenter.y - 20, RED, m_pOverlayMgr->GetOverlayBase()->GetD3DFont(), szEntityTag.c_str());
								tag_offset += 12 * szEntityTag.size() + 12;
							}

							// Draw hp
							if (iEntityHP > 0)
							{
								auto szEntityTag = std::to_string(iEntityHP) + "HP";

								m_pOverlayMgr->GetOverlayBase()->DrawString(v2EntityCenter.x + tag_offset, v2EntityCenter.y - 20, BLUE, m_pOverlayMgr->GetOverlayBase()->GetD3DFont(), szEntityTag.c_str());
								tag_offset += 10 * szEntityTag.size() + 20;
							}
						}
					}
				}
			}
		}

		Sleep(1);

		// Next entity object
		dwEntityBegin	= m_pMemoryHelper->Read<DWORD>(dwEntityBegin);

		// Refresh current objects
		dwLocalPlayer	= m_pMemoryHelper->Read<DWORD>(m_dwBaseModule + LOCAL_PLAYER);
		dwEntityEnd		= m_pMemoryHelper->Read<DWORD>(m_dwBaseModule + CLIENT_ADR, { 0xE9C, 0 , 4 });
	}
}

