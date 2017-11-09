// Win32Project1.cpp : 定义应用程序的入口点。
//

#include "targetver.h"
#include <sstream>
#include "Psapi.h"

HINSTANCE hInst;
PCWSTR   MyWindowName = L"MFC Test";
HWND hWnd = 0;
HANDLE GameHanle = 0;
RECT MyRect;
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

HWND GameHwnd = 0;

ID2D1HwndRenderTarget* pRender = NULL; 
ID2D1Factory*		   pFactory = NULL;
ID2D1SolidColorBrush*  pBrush = NULL; 
IDWriteFactory*		   pWriteFactory = NULL;
IDWriteTextFormat*	   pTextFormatMsyh = NULL;
ID2D1SolidColorBrush*  DrawBrush = NULL;
IDWriteTextFormat*	   pTextFormatKaiTi = NULL;

ULONG64 GameBase = 0;
ULONG64  AimPlayer = 0, CameraManagerAddress = 0, MyPlayerAddress = 0, AimDistance = 0;
BOOLEAN LockIng = FALSE, NoRecoil = FALSE;
DWORD  CurrentDistance = 0, NearestDistance = 0,WindowX = 1920, WindowY = 1080;
list<GameData>  PlayerArray;
D3DXVECTOR3 MMM;

BYTE HookAimCode[] = { 0x48, 0xB8, 0x0C, 0xAD, 0x10, 0x40, 0x01, 0x00, 0x00, 0x00, 0x48, 0x39, 0x84, 0x24, 0x80, 0x00, 0x00, 0x00, 0x75, 0x3E, 0x50, 0x53, 0x48, 0xA1, 0x57, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x1C, 0x25, 0x5F, 0x00, 0x30, 0x00, 0x48, 0x89, 0x45, 0xF7, 0x89, 0x5D, 0xFF, 0x80, 0x3C, 0x25, 0x00, 0x00, 0x00, 0x00, 0x01, 0x75, 0x18, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x48, 0x31, 0xC0, 0x48, 0x89, 0x45, 0xEB, 0x89, 0x45, 0xF3, 0x5B, 0x58, 0x48, 0xB8, 0xA0, 0x15, 0x59, 0x77, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0 };

ULONG64 ApiAddr = 0, AimJmpAddress = 0, HookAim = 0, ApiX = 0;

void DirectXInit()
{
	

	if (!pRender)
	{
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
		GetWindowRect(hWnd, &MyRect);
		pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(WindowX, WindowY)), &pRender);
		pRender->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pWriteFactory));
	}
	pRender->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &DrawBrush);

	pWriteFactory->CreateTextFormat(L"微软雅黑", NULL, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"MyFont", &pTextFormatKaiTi);


}

void DrawRenderText(int X, int Y, const WCHAR* text, DWORD Color, IDWriteTextFormat*TextFormat, float Alpha)//文字
{

	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
	pRender->DrawText(text, wcslen(text), TextFormat,
	D2D1::RectF((float)X, (float)Y, (float)MyRect.right, (float)MyRect.bottom), DrawBrush);
}
void DrawRenderTextA(int X, int Y, const char* text, DWORD Color, IDWriteTextFormat*TextFormat, float Alpha)//文字
{
	WCHAR WStr[256] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, text, strlen(text), WStr, strlen(text) * 2);
	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
	pRender->DrawText(WStr, wcslen(WStr), TextFormat,D2D1::RectF((float)X, (float)Y, (float)MyRect.right, (float)MyRect.bottom), DrawBrush);
}
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char * lpCmdLine, _In_ int nCmdShow)
{

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG Message;
	ZeroMemory(&Message, sizeof(Message));
	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		Sleep(1);
	}

	return (int)Message.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wndClass;//EX窗口类
	wndClass.cbClsExtra = NULL;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = NULL;
	wndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wndClass.hCursor = ::LoadCursor(0, IDC_ARROW);
	wndClass.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wndClass.hIconSm = ::LoadIcon(0, IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = MyWindowName;
	wndClass.lpszMenuName = MyWindowName;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	return RegisterClassEx(&wndClass);
}
void DrawLine(int X, int Y, int XX, int YY, DWORD Color, float Width, float Alpha)
{
	DrawBrush->SetColor(D2D1::ColorF(Color, Alpha));
	pRender->DrawLine(D2D1::Point2F(X, Y), D2D1::Point2F(XX, YY), DrawBrush, Width);
}
void DrawHalfRectangle(float X, float Y, float W, float H, DWORD Color, float Width, float Alpha)
{
	float DivW = W / 4.5;
	float DivH = H / 3.5;

	DrawLine(X, Y, X + DivW, Y, Color, Width, Alpha);
	DrawLine(X, Y, X, Y + DivH, Color, Width, Alpha);

	DrawLine(X + W - DivW, Y, X + W, Y, Color, Width, Alpha);
	DrawLine(X + W, Y, X + W, Y + DivH, Color, Width, Alpha);

	DrawLine(X, Y + H, X + DivW, Y + H, Color, Width, Alpha);
	DrawLine(X, Y + H, X, Y + H - DivH, Color, Width, Alpha);

	DrawLine(X + W - DivW, Y + H, X + W, Y + H, Color, Width, Alpha);
	DrawLine(X + W, Y + H, X + W, Y + H - DivH, Color, Width, Alpha);
}
BOOLEAN GetVal(ULONG64 Val)
{
	GameData TempData;

	for (list<GameData>::iterator it = PlayerArray.begin(); it != PlayerArray.end(); it++)
	{
		TempData = *it;
		if (TempData.Address == Val)
		{
			return TRUE;
		}
	}
	return FALSE;
}
VOID WeaponData()
{
	ULONG64 Weapon_Processor = 0, Weapon_Base = 0, WeaponAddress = 0;
	int Weapon_Count = 0;
	Weapon_Processor = X64_Read_Int(GameHanle, MyPlayerAddress + WeaponProcessor);
	Weapon_Count = X64_Read_(GameHanle, Weapon_Processor + EquippedWeapons_Count, 4);
	if (Weapon_Count >= 0 && Weapon_Count <= 2)
	{
		Weapon_Base = X64_Read_Int(GameHanle, Weapon_Processor + EquippedWeapons_Base);
		WeaponAddress = X64_Read_Int(GameHanle, Weapon_Base + Weapon_Count * 8);
		if (WeaponAddress)
		{

			X64_Write_FLOAT(GameHanle, WeaponAddress + FTrajectoryWeaponData + 0x20 ,0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FTrajectoryWeaponData + 0x24, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FTrajectoryWeaponData + 0x28, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FTrajectoryWeaponData + 0x2C, 99999.0f);

			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x0, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x4, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0xC, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x3C, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x40, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x44, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x48, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x4C, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x50, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x54, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FRecoilInfo + 0x58, 0);
			X64_Write_Int(GameHanle, WeaponAddress + FWeaponGunAnim + 0xA0, 0);
			X64_Write_Int(GameHanle, WeaponAddress + FWeaponGunAnim + 0xA8, 0);
			X64_Write_Int(GameHanle, WeaponAddress + FWeaponGunAnim + 0xB0, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x00, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x04, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x08, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x0C, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x10, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x14, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x18, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x1C, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x20, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x24, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x28, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x2C, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x30, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x34, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x38, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + FWeaponDeviationData + 0x3C, 0);
			
			
			X64_Write_FLOAT(GameHanle, WeaponAddress + 0x9F8, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + 0x9FC, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + 0xA10, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + 0xA14, 0);
			X64_Write_FLOAT(GameHanle, WeaponAddress + 0xA18, 0);
			
		}
	}

}
VOID GetCameraAndControl(ULONG64 Data)
{
	ULONG64 Character = 0;
	CameraManagerAddress = X64_Read_Int(GameHanle, Data + OwningGameInstance);
	CameraManagerAddress = X64_Read_Int(GameHanle, CameraManagerAddress + LocalPlayer);
	CameraManagerAddress = X64_Read_Int(GameHanle, CameraManagerAddress + LocalPlayerData);
	CameraManagerAddress = X64_Read_Int(GameHanle, CameraManagerAddress + PlayerController);
	MyPlayerAddress = X64_Read_Int(GameHanle, CameraManagerAddress + 0x3A8);
	Character = X64_Read_Int(GameHanle, MyPlayerAddress + CharacterMovement);

	if (NoRecoil)
	{
		WeaponData();
	}
}
BOOLEAN WorldToScreen(ULONG64 pThis, D3DXVECTOR3 In, D3DXVECTOR2* Out, DWORD* Distance)
{
	ULONG64 Camera_Cache = X64_Read_Int(GameHanle, CameraManagerAddress + PlayerCameraManager);
	D3DXVECTOR3 AxisX, AxisY, AxisZ, Delta, Transformed, Location, Rotator;
	float FovAngle = 0;

	Location = X64_Read_D3DXVECTOR3(GameHanle, Camera_Cache + CameraPos);
	Rotator = X64_Read_D3DXVECTOR3(GameHanle, Camera_Cache + CameraRot);
	FovAngle = X64_Read_FLOAT(GameHanle, Camera_Cache + FOV);

	Delta = SubD3DXVECTOR3(In, Location);
	GetAxes(Rotator, &AxisX, &AxisY, &AxisZ);
	Transformed.x = DotProduct(Delta, AxisY);
	Transformed.y = DotProduct(Delta, AxisZ);
	Transformed.z = DotProduct(Delta, AxisX);
	if (Transformed.z < 1.00f)
	{
		Transformed.z = 1.0f;
		return FALSE;
	}
	if (Transformed.z / 100 > 0)
	{
		if (Distance)
		{
			*Distance = Transformed.z / 100;
		}
		float ScreenCenterX = (WindowX / 2);
		float ScreenCenterY = (WindowY / 2);
		Out->x = ScreenCenterX + Transformed.x * ScreenCenterX / tanf(FovAngle * Pi / 360.0f) / Transformed.z;
		Out->y = ScreenCenterY - Transformed.y * ScreenCenterX / tanf(FovAngle * Pi / 360.0f) / Transformed.z;

		if (Out->x < WindowX && Out->x > 0)
		{
			if (Out->y < WindowY && Out->y > 0)
			{
				return TRUE;
			}
		}
	}



	return FALSE;
}
VOID GetData()
{


	ULONG64 pUWorld, Persistent, Base;
	GameData TempData;
	DWORD Count = 0;
	pUWorld = X64_Read_Int(GameHanle, GameBase + UWorld);
	if (pUWorld)
	{
		GetCameraAndControl(pUWorld);

		Persistent = X64_Read_Int(GameHanle, pUWorld + PersistentLevel);
		Count = X64_Read_(GameHanle, Persistent + AActor_Count, 4);
		if (Count > 0 && Count < 8000)
		{

			Base = X64_Read_Int(GameHanle, Persistent + AActor_Base);
			for (size_t i = 0; i < Count; i++)
			{
				TempData.Address = X64_Read_Int(GameHanle, Base + i * 8);
				if (TempData.Address)
				{
					TempData.Id = X64_Read_(GameHanle, TempData.Address + ID, 4);

					if (IsPlayer(TempData.Id))
					{
						if (X64_Read_FLOAT(GameHanle, TempData.Address + Health + 4) == 100)
						{
							if (GetVal(TempData.Address) == FALSE)
							{
								TempData.Class = People;
								PlayerArray.push_front(TempData);
							}

						}
					}
					else
					{
						if (TempData.Id == GunItemId)
						{
							if (GetVal(TempData.Address) == FALSE)
							{
								TempData.Class = Goods;
								PlayerArray.push_front(TempData);
							}
						}
						else
						{
							if (GetVal(TempData.Address) == FALSE)
							{
								if (IsVehicle(TempData.Id, NULL))
								{
									TempData.Class = Vehicle;
									PlayerArray.push_front(TempData);
								}
							}
						}
					}
				}
			}
		}

	}
	else
	{
		PlayerArray.clear();
	}
}
FTransform ReadFTransform(ULONG64 ptr)
{

	float quatX = X64_Read_FLOAT(GameHanle, ptr);
	float quatY = X64_Read_FLOAT(GameHanle, ptr + 0x04);
	float quatZ = X64_Read_FLOAT(GameHanle, ptr + 0x08);
	float quatW = X64_Read_FLOAT(GameHanle, ptr + 0x0C);

	float transX = X64_Read_FLOAT(GameHanle, ptr + 0x0010);
	float transY = X64_Read_FLOAT(GameHanle, ptr + 0x0014);
	float transZ = X64_Read_FLOAT(GameHanle, ptr + 0x0018);

	float scaleX = X64_Read_FLOAT(GameHanle, ptr + 0x0020);
	float scaleY = X64_Read_FLOAT(GameHanle, ptr + 0x0024);
	float scaleZ = X64_Read_FLOAT(GameHanle, ptr + 0x0028);

	D3DXVECTOR4 fQuat = { quatX, quatY, quatZ, quatW };
	D3DXVECTOR3 trans = { transX, transY, transZ };
	D3DXVECTOR3 scale = { scaleX, scaleY, scaleZ };
	FTransform fTransform = { fQuat, trans, scale };
	return fTransform;
}
D3DXMATRIX ToMatrixWithScale(D3DXVECTOR4 Rotation, D3DXVECTOR3 Translation, D3DXVECTOR3 Scale3D)
{
	D3DXMATRIX m;

	m._41 = Translation.x;
	m._42 = Translation.y;
	m._43 = Translation.z;

	float x2 = Rotation.x + Rotation.x;
	float y2 = Rotation.y + Rotation.y;
	float z2 = Rotation.z + Rotation.z;

	float xx2 = Rotation.x * x2;
	float yy2 = Rotation.y * y2;
	float zz2 = Rotation.z * z2;
	m._11 = (1.0f - (yy2 + zz2)) * Scale3D.x;
	m._22 = (1.0f - (xx2 + zz2)) * Scale3D.y;
	m._33 = (1.0f - (xx2 + yy2)) * Scale3D.z;


	float yz2 = Rotation.y * z2;
	float wx2 = Rotation.w * x2;
	m._32 = (yz2 - wx2) * Scale3D.z;
	m._23 = (yz2 + wx2) * Scale3D.y;


	float xy2 = Rotation.x * y2;
	float wz2 = Rotation.w * z2;
	m._21 = (xy2 - wz2) * Scale3D.y;
	m._12 = (xy2 + wz2) * Scale3D.x;


	float xz2 = Rotation.x * z2;
	float wy2 = Rotation.w * y2;
	m._31 = (xz2 + wy2) * Scale3D.z;
	m._13 = (xz2 - wy2) * Scale3D.x;

	m._14 = 0.0f;
	m._24 = 0.0f;
	m._34 = 0.0f;
	m._44 = 1.0f;

	return m;
}
D3DXMATRIX D3DXMatrixMultiply(D3DXMATRIX pM1, D3DXMATRIX pM2)
{
	D3DXMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
D3DXVECTOR3 ToFRotator(D3DXVECTOR3 Ver)
{
	D3DXVECTOR3 Rot_;
	float RADPI = 180 / Pi;
	Rot_.y = atan2(Ver.y, Ver.x) * RADPI;
	Rot_.x = atan2(Ver.z, sqrt((Ver.x * Ver.x) + (Ver.y * Ver.y))) * RADPI;
	Rot_.z = 0;
	return Rot_;
}
D3DXVECTOR3 Clamp(D3DXVECTOR3 Ver)
{
	D3DXVECTOR3 Result = Ver;
	if (Result.x > 180)
	{
		Result.x = Result.x - 360;
	}
	else
	{
		if (Result.x < -180)
		{
			Result.x = Result.x + 360;
		}
	}


	if (Result.y > 180)
	{
		Result.y = Result.y - 360;
	}
	else
	{
		if (Result.y < -180)
		{
			Result.y = Result.y + 360;
		}
	}

	if (Result.x > 89)
	{
		Result.x = 89;
	}
	if (Result.x < -89)
	{
		Result.x = -89;
	}

	while (Result.y < 180)
	{
		Result.y = Result.y + 360;
	}

	while (Result.y > 180)
	{
		Result.y = Result.y - 360;
	}
	Result.z = 0;
	return Result;
}
VOID HookJmp(ULONG64 GameAddress, ULONG64 MyAddress)
{
	BYTE Jmp[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
	*(ULONG64 *)(Jmp + 1) = (ULONG64)MyAddress - GameAddress - 5;

	X64_Write_(GameHanle, GameAddress, &Jmp, sizeof(Jmp));

}
VOID AimBot(D3DXVECTOR3 TargetPos)
{
	ULONG64 Camera_Cache = X64_Read_Int(GameHanle, CameraManagerAddress + PlayerCameraManager);
	D3DXVECTOR3 LocalCameraPos, LocalAngles, AimAngles;

	LocalAngles = X64_Read_D3DXVECTOR3(GameHanle, CameraManagerAddress + PItchX);
	LocalCameraPos = X64_Read_D3DXVECTOR3(GameHanle, Camera_Cache + CameraPos);
	LocalCameraPos = SubD3DXVECTOR3(TargetPos, LocalCameraPos);
	LocalCameraPos = ToFRotator(LocalCameraPos);
	AimAngles = SubD3DXVECTOR3(LocalCameraPos, LocalAngles);
	AimAngles = Clamp(AimAngles);
	LocalAngles = AddD3DXVECTOR3(LocalAngles, AimAngles);
	X64_Write_FLOAT(GameHanle, CameraManagerAddress + PItchX, LocalAngles.x);
	X64_Write_FLOAT(GameHanle, CameraManagerAddress + YawY, LocalAngles.y);


}
VOID GetBoneMatrix(ULONG64 pObj, D3DXVECTOR3 *result, int BoneIdx)
{

	FTransform Bone = ReadFTransform(X64_Read_Int(GameHanle, pObj + 0x790) + BoneIdx * 0x30);
	FTransform Actor = ReadFTransform(pObj + 0x190);
	D3DXMATRIX BoneMatrix = ToMatrixWithScale(Bone.Rotation, Bone.Translation, Bone.Scale3D);
	D3DXMATRIX ComponentToWorldMatrix = ToMatrixWithScale(Actor.Rotation, Actor.Translation, Actor.Scale3D);
	D3DXMATRIX NewMatrix = D3DXMatrixMultiply(BoneMatrix, ComponentToWorldMatrix);
	*result = { NewMatrix._41, NewMatrix._42, NewMatrix._43 };

}
VOID BoneESP(ULONG64 Target, DWORD cColor)
{
	BOOLEAN Ok = FALSE;
	ULONG64 MeshAddress = X64_Read_Int(GameHanle, Target + Mesh);
	if (MeshAddress <= 0)
	{
		return;
	}

	D3DXVECTOR3 vHeadBone, vHip, vNeck, vUpperArmLeft, vUpperArmRight, vLeftHand, vRightHand, vRightThigh, vLeftThigh, vRightCalf, vLeftCalf, vLeftFoot, vRightFoot;
	D3DXVECTOR2 vHeadBoneOut, vHipOut, vNeckOut, vUpperArmLeftOut, vUpperArmRightOut, vLeftHandOut, vRightHandOut, vRightThighOut, vLeftThighOut, vRightCalfOut, vLeftCalfOut, vLeftFootOut, vRightFootOut;
	//Head
	GetBoneMatrix(MeshAddress, &vHeadBone, 15);//6
	Ok = WorldToScreen(CameraManagerAddress, vHeadBone, &vHeadBoneOut, NULL);

	//Hip

	GetBoneMatrix(MeshAddress, &vHip, 1);
	Ok = WorldToScreen(CameraManagerAddress, vHip, &vHipOut, NULL);

	//Neck

	GetBoneMatrix(MeshAddress, &vNeck, 5);
	Ok = WorldToScreen(CameraManagerAddress, vNeck, &vNeckOut, NULL);

	//Left UpperArm

	GetBoneMatrix(MeshAddress, &vUpperArmLeft, 78);
	Ok = WorldToScreen(CameraManagerAddress, vUpperArmLeft, &vUpperArmLeftOut, NULL);

	//Right UpperArm

	GetBoneMatrix(MeshAddress, &vUpperArmRight, 105);
	Ok = WorldToScreen(CameraManagerAddress, vUpperArmRight, &vUpperArmRightOut, NULL);

	//Left Hand

	GetBoneMatrix(MeshAddress, &vLeftHand, 80);
	Ok = WorldToScreen(CameraManagerAddress, vLeftHand, &vLeftHandOut, NULL);

	//Right Hand

	GetBoneMatrix(MeshAddress, &vRightHand, 107);
	Ok = WorldToScreen(CameraManagerAddress, vRightHand, &vRightHandOut, NULL);

	//Right Thigh

	GetBoneMatrix(MeshAddress, &vRightThigh, 164);
	Ok = WorldToScreen(CameraManagerAddress, vRightThigh, &vRightThighOut, NULL);

	//Left Thigh

	GetBoneMatrix(MeshAddress, &vLeftThigh, 158);
	Ok = WorldToScreen(CameraManagerAddress, vLeftThigh, &vLeftThighOut, NULL);

	//Right Calf

	GetBoneMatrix(MeshAddress, &vRightCalf, 165);
	Ok = WorldToScreen(CameraManagerAddress, vRightCalf, &vRightCalfOut, NULL);

	//Left Calf

	GetBoneMatrix(MeshAddress, &vLeftCalf, 159);
	Ok = WorldToScreen(CameraManagerAddress, vLeftCalf, &vLeftCalfOut, NULL);

	//Left Foot

	GetBoneMatrix(MeshAddress, &vLeftFoot, 160);
	Ok = WorldToScreen(CameraManagerAddress, vLeftFoot, &vLeftFootOut, NULL);

	//Right Foot
	GetBoneMatrix(MeshAddress, &vRightFoot, 166);
	Ok = WorldToScreen(CameraManagerAddress, vRightFoot, &vRightFootOut, NULL);

	DrawLine(vNeckOut.x, vNeckOut.y, vHeadBoneOut.x, vHeadBoneOut.y, cColor, 1.0, 1.0);
	DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, cColor, 1.0, 1.0);

	DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, cColor, 1.0, 1.0);
	DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, cColor, 1.0, 1.0);

	DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, cColor, 1.0, 1.0);
	DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, cColor, 1.0, 1.0);

	DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, cColor, 1.0, 1.0);
	DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, cColor, 1.0, 1.0);

	DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, cColor, 1.0, 1.0);
	DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, cColor, 1.0, 1.0);

	DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, cColor, 1.0, 1.0);
	DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, cColor, 1.0, 1.0);

}
VOID Esp_People(ULONG64 Address)
{


	D3DXVECTOR3 PlayerPosition = { 0, 0, 0 };
	D3DXVECTOR2 _2DAxis = { 0, 0 };
	DWORD Distance = 0, Id = 0;
	WCHAR Display[256];
	ULONG64 StateAddress = 0;
	Id = X64_Read_(GameHanle, Address + ID, 4);
	if (IsPlayer(Id) && Address != MyPlayerAddress)
	{
		GetBoneMatrix(X64_Read_Int(GameHanle, Address + Mesh), &PlayerPosition, 5);
		if (X64_Read_FLOAT(GameHanle, Address + Health + 4) == 100)
		{
			if (WorldToScreen(CameraManagerAddress, PlayerPosition, &_2DAxis, &Distance))
			{
		
				if (X64_Read_FLOAT(GameHanle, Address + Health) > 1)
				{

						if (LockIng == FALSE)
						{
							CurrentDistance = GetDistance(WindowX / 2, WindowY / 2, _2DAxis.x, _2DAxis.y);
							if (NearestDistance == 0)
							{
								NearestDistance = CurrentDistance;
								AimPlayer = Address;
								AimDistance = NearestDistance;
							}
							else
							{
								if (CurrentDistance < NearestDistance)
								{
									NearestDistance = CurrentDistance;
									AimPlayer = Address;
									AimDistance = NearestDistance;
								}
							}

						}

						BoneESP(Address, D2D1::ColorF::Lime);

						GetBoneMatrix(X64_Read_Int(GameHanle, Address + Mesh), &PlayerPosition, 160);
						PlayerPosition.z = PlayerPosition.z - 20;
						if (WorldToScreen(CameraManagerAddress, PlayerPosition, &_2DAxis, &Distance))
						{
							std::wstringstream ss;
							ss << "[" << Distance << "] m";
							DrawRenderText(_2DAxis.x, _2DAxis.y + 12, ss.str().data(), D2D1::ColorF::Lime, pTextFormatKaiTi, 1.0f);
							RtlZeroMemory(&Display, sizeof(Display));
							SIZE_T dwNumberOfBytesRead;
							ReadProcessMemory(GameHanle, (LPCVOID)X64_Read_Int(GameHanle, X64_Read_Int(GameHanle, Address + PlayerState) + PlayerName), &Display, 64, &dwNumberOfBytesRead);
							DrawRenderText(_2DAxis.x, _2DAxis.y, Display, D2D1::ColorF::Lime, pTextFormatKaiTi, 1.0f);

						}



					}

				
			}
		}
	}

}
VOID Esp_Goods(ULONG64 Address)
{

	D3DXVECTOR3 TempPos;
	D3DXVECTOR3 PlayerPosition = { 0, 0, 0 };
	D3DXVECTOR2 _2DAxis = { 0, 0 };
	DWORD Distance = 0, Id = 0;
	WCHAR Display[256];
	ULONG64 ItemAddress = 0, GoodsAddress = 0, pUItemAddress = 0;
	int Count = 0;
	Id = X64_Read_(GameHanle, Address + ID, 4);
	ItemAddress = X64_Read_Int(GameHanle, Address + DroppedItemGroup);
	Count = X64_Read_(GameHanle, Address + DroppedItemGroup_Count, 4);
	if (ItemAddress && Count < 500 && Count > 0)
	{
		for (size_t i = 0; i < Count; i++)
		{
			GoodsAddress = X64_Read_Int(GameHanle, ItemAddress + i * 8);
			if (GoodsAddress)
			{
				pUItemAddress = X64_Read_Int(GameHanle, GoodsAddress + DroppedItemGroup_UItem);
				ULONG64 pUItemFString = X64_Read_Int(GameHanle, pUItemAddress + 0x40);
				ULONG64 pItemName = X64_Read_Int(GameHanle, pUItemFString + 0x28);
				int NameLen = X64_Read_(GameHanle, pUItemFString + 0x30, 4) * 2;
				if (pItemName && NameLen > 1 && NameLen < sizeof(Display))
				{
					SIZE_T dwNumberOfBytesRead;
					ReadProcessMemory(GameHanle, (LPCVOID)pItemName, &Display, NameLen, &dwNumberOfBytesRead);
					if (IsGoods(Display))
					{

						PlayerPosition = X64_Read_D3DXVECTOR3(GameHanle, X64_Read_Int(GameHanle, Address + m_rootComponent) + Pos_);
						PlayerPosition = AddD3DXVECTOR3(PlayerPosition, X64_Read_D3DXVECTOR3(GameHanle, GoodsAddress + DroppedItemGroup_Relative_Pos));
						if (WorldToScreen(CameraManagerAddress, PlayerPosition, &_2DAxis, &Distance))
						{
							//显示

							DrawRenderText(_2DAxis.x, _2DAxis.y, Display, D2D1::ColorF::Yellow, pTextFormatKaiTi, 1.0f);
							std::wstringstream ss;
							ss << "[" << Distance << "] m";
							DrawRenderText(_2DAxis.x, _2DAxis.y + 12, ss.str().data(), D2D1::ColorF::Yellow, pTextFormatKaiTi, 1.0f);
							ss.str(L"");
						}

					}
				}




			}

		}
	}


}
VOID Esp_Vehicle(ULONG64 Address)
{

	D3DXVECTOR3 TempPos;
	D3DXVECTOR3 PlayerPosition = { 0, 0, 0 };
	D3DXVECTOR2 _2DAxis = { 0, 0 };
	DWORD Distance = 0, Id = 0;
	CHAR Display[64];
	Id = X64_Read_(GameHanle, Address + ID, 4);
	if (IsVehicle(Id, Display))
	{
		PlayerPosition = X64_Read_D3DXVECTOR3(GameHanle, X64_Read_Int(GameHanle, Address + m_rootComponent) + Pos_);

		if (WorldToScreen(CameraManagerAddress, PlayerPosition, &_2DAxis, &Distance))
		{
			
			//显示
			DrawRenderTextA(_2DAxis.x, _2DAxis.y, Display, D2D1::ColorF::Yellow, pTextFormatKaiTi, 1.0f);
			std::stringstream ss;
			ss << "[" << Distance << "] m";
			DrawRenderTextA(_2DAxis.x, _2DAxis.y + 12, ss.str().data(), D2D1::ColorF::Yellow, pTextFormatKaiTi, 1.0f);

		}

	}

}
VOID Esp()
{
	BYTE Temp[] = { 0x1 };
	BYTE Temp_[] = { 0x0 };
	GameData TempData;
	CurrentDistance = 0;
	NearestDistance = 0;
	GetData();

	for (list<GameData>::iterator it = PlayerArray.begin(); it != PlayerArray.end(); it++)
	{

		TempData = *it;
		if (TempData.Class == People)
		{
			Esp_People(TempData.Address);
		}
		if (TempData.Class == Goods)
		{
			Esp_Goods(TempData.Address);
		}
		if (TempData.Class == Vehicle)
		{
			Esp_Vehicle(TempData.Address);
		}
	}

	if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && AimPlayer && X64_Read_FLOAT(GameHanle, AimPlayer + Health) > 1 && X64_Read_FLOAT(GameHanle, AimPlayer + Health + 4) == 100.0f)
	{

				if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
				{
					if (!NoRecoil)
					{
						X64_Write_(GameHanle, HookAim + 0x5E, &Temp, 1);
					}
					
					GetBoneMatrix(X64_Read_Int(GameHanle,AimPlayer + Mesh), &MMM, 15);
				}
				else
				{
					GetBoneMatrix(X64_Read_Int(GameHanle,AimPlayer + Mesh), &MMM, 5);
					if (!NoRecoil)
					{
						X64_Write_(GameHanle, HookAim + 0x5E, &Temp_, 1);
					}
					
				}
				LockIng = TRUE;

				AimBot(MMM);
				if (!NoRecoil)
				{
					X64_Write_FLOAT(GameHanle, HookAim + 0x60, MMM.x);
					X64_Write_FLOAT(GameHanle, HookAim + 0x64, MMM.y);
					X64_Write_FLOAT(GameHanle, HookAim + 0x68, MMM.z);
				}
				


	}
	else
	{
		if (!NoRecoil)
		{
			X64_Write_(GameHanle, HookAim + 0x5E, &Temp_, 1);
		}
		
		LockIng = FALSE;
		AimPlayer = NULL;
	}
}
VOID GetGameData()
{
	RECT tSize;
	while (true)
	{
		GameHwnd = FindWindowA("UnrealWindow", 0);
		if (GameHwnd)
		{

			DWORD dwStyle = GetWindowLong(GameHwnd, GWL_STYLE);
			
			if ((dwStyle & WS_BORDER) != 0)
			{
				GetWindowRect(GameHwnd, &tSize);
				int x = GetSystemMetrics(32) * 2;
				int y = GetSystemMetrics(4) + GetSystemMetrics(33) * 2;

				tSize.left += x;
				tSize.top += y;
				WindowX = tSize.right - tSize.left - 10;
				WindowY = tSize.bottom - tSize.top - 10;
				MoveWindow(hWnd, tSize.left, tSize.top, WindowX, WindowY, TRUE);
				pRender->Resize(D2D1::SizeU(WindowX, WindowY));

			}
			else
			{
				WindowX = GetSystemMetrics(SM_CXSCREEN);
				WindowY = GetSystemMetrics(SM_CYSCREEN);
				MoveWindow(hWnd, 0, 0, WindowX, WindowY, TRUE);
				pRender->Resize(D2D1::SizeU(WindowX, WindowY));
			}

		}
		pRender->BeginDraw();
		pRender->Clear(D2D1_COLOR_F{ 1, 0, 0, 0 });
		Esp();
		pRender->EndDraw();
		WaitForSingleObject((HANDLE)-1, 5);
	}


}

DWORD GetPid()
{

	HANDLE snapshot;
	DWORD PID = 0;
	PROCESSENTRY32 processinfo;
	processinfo.dwSize = sizeof(processinfo);
	snapshot = CreateToolhelp32Snapshot(0xF, 0);
	if (snapshot == NULL)
	{
		PID = 0;
	}


	bool status = Process32First(snapshot, &processinfo);
	while (status)
	{
		if (processinfo.szExeFile)
		{

			if (wcsstr(processinfo.szExeFile, L"TslGame.exe") != 0)
			{
				PID = processinfo.th32ProcessID;
			}
		}
		status = Process32Next(snapshot, &processinfo);
	}
	if (snapshot)
	{
		CloseHandle(snapshot);
	}
	return PID;
}
ULONG64 GetAimJmpAddress(ULONG64 Address)
{
	ULONG64 Temp = Address;
	while (X64_Read_(GameHanle, Temp, 1) != 0x83)
	{
		Temp = Temp + 1;
	}
	return Temp;
}
VOID APIHOOK()
{
	ULONG64 TempAddress;

	TempAddress = (ULONG64)GetProcAddress(GetModuleHandleA("kernel32.dll"), "TlsGetValue");

	AimJmpAddress = GetAimJmpAddress(TempAddress);

	ApiAddr = TempAddress;

	ApiX = X64_Read_Int(GameHanle, ApiAddr);

	HookAim = (ULONG64)VirtualAllocEx(GameHanle, NULL, 100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	*(ULONG64 *)(HookAimCode + 0x2) = GameBase + 0x42504C;    //48 6B CF 58          48 03 8B 200B0000    74 17  48 8D 55 E7     

	*(ULONG64 *)(HookAimCode + 0x18) = (ULONG64)HookAim + 0x60;
	*(DWORD *)(HookAimCode + 0x23) = (DWORD)HookAim + 0x68;
	*(DWORD *)(HookAimCode + 0x31) = (DWORD)HookAim + 0x5E;

	*(ULONG64 *)(HookAimCode + 0x54) = (ULONG64)AimJmpAddress;

	X64_Write_(GameHanle, HookAim, &HookAimCode, sizeof(HookAimCode));


	HookJmp(ApiAddr, (ULONG64)HookAim);

}
ULONG64 EnumModules(HANDLE m_hProcess)
{
	HMODULE hModulesArray[2048] = { 0 };
	DWORD cbNeeded = 0;
	EnumProcessModules(m_hProcess, hModulesArray, sizeof(hModulesArray), &cbNeeded);


	for (unsigned int i = 0; i < cbNeeded; i++)
	{
		char szPath[MAX_PATH] = { 0 };
		GetModuleFileNameExA(m_hProcess, hModulesArray[i], szPath, MAX_PATH);

		std::string Path(szPath);
		int pos = Path.find_last_of('\\');
		std::string szModule(Path.substr(pos + 1));

		MODULEINFO ModeInfo = { 0 };
		DWORD ModeInfoSize = sizeof(MODULEINFO);
		GetModuleInformation(m_hProcess, hModulesArray[i], &ModeInfo, ModeInfoSize);
		return (ULONG64)ModeInfo.lpBaseOfDll;

	}
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   

   MARGINS Margin = { -1, -1, -1, -1 };
   hInst = hInstance;
   hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW, MyWindowName, MyWindowName, WS_POPUP, 1, 1, WindowX, WindowY, 0, 0, 0, 0);
   if (!hWnd)
   {
      return FALSE;
   }
   SetLayeredWindowAttributes(hWnd, 0, 1, LWA_ALPHA);
   SetLayeredWindowAttributes(hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
   ShowWindow(hWnd, SW_SHOW);
   DwmExtendFrameIntoClientArea(hWnd, &Margin);
   DirectXInit();
   Sleep(3000);

   GameHanle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetPid());
   GameBase = EnumModules(GameHanle);
   NoRecoil = TRUE;   //若要开启魔术子弹 把这条件改一下

   if (!NoRecoil)
   {
	   APIHOOK();
   }

   ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetGameData, 0, 0, 0);
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
		break;
	case WM_DESTROY:
		PostQuitMessage(1);
		return 0;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}