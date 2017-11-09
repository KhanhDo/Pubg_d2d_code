#pragma once

// 包括 SDKDDKVer.h 将定义可用的最高版本的 Windows 平台。

// 如果要为以前的 Windows 平台生成应用程序，请包括 WinSDKVer.h，并将
// WIN32_WINNT 宏设置为要支持的平台，然后再包括 SDKDDKVer.h。

#include <iostream> 
#include <atlstr.h> 
#include <SDKDDKVer.h>
#include <Dwmapi.h>
#include <d2d1.h>
#include <Dwrite.h>
#include <WinBase.h>
#include <tlhelp32.h>
#include <list>

using namespace std;
#pragma comment( lib,"Dwmapi.lib" )
#pragma  comment(lib,"d2d1.lib")
#pragma  comment(lib,"Dwrite.lib")
#pragma comment( lib,"winmm.lib" )


#define UWorld 0x37E4918 //0F 28 D6 48 8B 07 49 8B D5 48 8B CF
#define GNames 0x36E7710 //41 83 3F 00 0F 8D 7E 00 00 00
#define World  0x80
#define PersistentLevel  0x30 
#define OwningGameInstance  0x0140 
#define LocalPlayer  0x38 
#define LocalPlayerData  0x0 
#define ViewportClient  0x58 
#define PlayerController   0x30 
#define PlayerCameraManager   0x0438 
#define AActor_Base   0xA0 
#define AActor_Count   0xA8 
#define m_rootComponent   0x180 
#define ComponentVelocity = 0x0258;
#define PlayerState   0x3C0 
#define PlayerName   0x3A8

#define Mesh   0x400 
#define CharacterMovement 0x408
#define Acceleration 0x298

#define ID   0x0018 
#define Pos_   0x0174
#define Rot   0x01D8 

#define Health   0x107C 
#define GroggyHealth   Health + 0x8 

#define CameraCache   0x0410 
#define POV   0x0010 
#define CameraPos   CameraCache + POV  
#define CameraRot   CameraCache + POV + 0x000C 
#define FOV   CameraCache + POV + 0x0018 

//AController
#define PItchX   0x03D0  
#define YawY   PItchX + 0x4 


// UDroppedItemInteractionComponent
#define DroppedItemGroup   0x2D8 
#define DroppedItemGroup_Count   0x2E0
#define DroppedItemGroup_Relative_Pos   0x1E0 
#define DroppedItemGroup_UItem   0x0448 

#define DroppedItem_UItem   0x03A0 


#define WeaponProcessor 0x9E8
#define EquippedWeapons_Base 0x438
#define EquippedWeapons_Count 0x448
#define FTrajectoryWeaponData 0xA48
#define FRecoilInfo 0x0AA8
#define FWeaponGunAnim 0x930
#define FWeaponData 0x538
#define FWeaponGunData 0x860
#define FWeaponDeviationData 0x8C8

#define GunItemId 7261

#define Pi 3.1415926535


#ifndef D3DCOLOR_ABGR 
#define D3DCOLOR_ABGR(a,b,g,r)\
((DWORD)((((a)& 0xff) << 24) | (((b)& 0xff) << 16) | (((g)& 0xff) << 8) | ((r)& 0xff)))
#endif 
typedef struct _D3DXVECTOR2
{
	FLOAT x;
	FLOAT y;

} D3DXVECTOR2, *PD3DXVECTOR2;
typedef struct _D3DXVECTOR3 {
	FLOAT x;
	FLOAT y;
	FLOAT z;
} D3DXVECTOR3, *LPD3DXVECTOR3;
typedef struct _D3DXVECTOR4
{

	FLOAT   x;
	FLOAT	y;
	FLOAT	z;
	FLOAT	w;


} D3DXVECTOR4, *PD3DXVECTOR4;
typedef struct _D3DXCOLOR
{
	FLOAT   r;
	FLOAT   g;
	FLOAT   b;
	FLOAT   a;

} D3DXCOLOR, *PD3DXCOLOR;
typedef struct _D3DXMATRIX
{

	FLOAT _11;
	FLOAT	_12;
	FLOAT	_13;
	FLOAT	_14;
	FLOAT		 _21;
	FLOAT	 _22;
	FLOAT	 _23;
	FLOAT	 _24;
	FLOAT	 _31;
	FLOAT	 _32;
	FLOAT	 _33;
	FLOAT	 _34;
	FLOAT	 _41;
	FLOAT	 _42;
	FLOAT	 _43;
	FLOAT	 _44;


} D3DXMATRIX, *PD3DXMATRIX;

enum GameClass
{
	People,
	Goods,
	Vehicle
};
typedef struct GameData
{
	DWORD Id;
	ULONG64 Address;
	GameClass Class;
};


typedef struct FTransform
{
	D3DXVECTOR4 Rotation;
	D3DXVECTOR3 Translation;
	D3DXVECTOR3 Scale3D;
};

extern ULONG64 GameBase;

extern HANDLE GameHanle;


ULONG64 X64_Read_Int(HANDLE hProcess, ULONG64 Address);
ULONG64 X64_Read_(HANDLE hProcess, ULONG64 Address, DWORD Length);
FLOAT X64_Read_FLOAT(HANDLE hProcess, ULONG64 Address);
D3DXVECTOR3 X64_Read_D3DXVECTOR3(HANDLE hProcess, ULONG64 Address);
D3DXVECTOR4 X64_Read_D3DXVECTOR4(HANDLE hProcess, ULONG64 Address);
D3DXMATRIX X64_Read_D3DXMATRIX(HANDLE hProcess, ULONG64 Address);
VOID X64_Write_Int(HANDLE hProcess, ULONG64 Address, ULONG64 Value);
VOID X64_Write_DOUBLE(HANDLE hProcess, ULONG64 Address, DOUBLE Value);
VOID X64_Write_FLOAT(HANDLE hProcess, ULONG64 Address, FLOAT Value);
VOID X64_Write_(HANDLE hProcess, ULONG64 Address, PVOID Source, DWORD Length);
BOOLEAN IsPlayer(DWORD Id);
BOOLEAN IsVehicle(DWORD Id, CHAR* Temp2);
BOOLEAN IsGoods(WCHAR* Temp);
FLOAT GetDistance(FLOAT X, FLOAT Y, FLOAT X1, FLOAT Y1);
D3DXVECTOR3 SubD3DXVECTOR3(D3DXVECTOR3 VecA, D3DXVECTOR3 VecB);
D3DXVECTOR3 AddD3DXVECTOR3(D3DXVECTOR3 VecA, D3DXVECTOR3 VecB);
VOID GetAxes(D3DXVECTOR3 Rotation, D3DXVECTOR3* X, D3DXVECTOR3* Y, D3DXVECTOR3* Z);
float DotProduct(D3DXVECTOR3 VecA, D3DXVECTOR3 VecB);
BOOLEAN GetGNames(int _id, CHAR* Temp);