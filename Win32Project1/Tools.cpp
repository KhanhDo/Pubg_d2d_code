#include "targetver.h"
#include "Psapi.h"
ULONG64 X64_Read_Int(HANDLE hProcess, ULONG64 Address)
{
	ULONG64 Ret = 0;
	SIZE_T dwNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPCVOID)Address, &Ret, 8, &dwNumberOfBytesRead);
	return Ret;
}
ULONG64 X64_Read_(HANDLE hProcess, ULONG64 Address, DWORD Length)
{
	ULONG64 Ret = 0;
	SIZE_T dwNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPCVOID)Address, &Ret, Length, &dwNumberOfBytesRead);
	return Ret;
}
FLOAT X64_Read_FLOAT(HANDLE hProcess, ULONG64 Address)
{
	FLOAT Ret = 0;
	SIZE_T dwNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPCVOID)Address, &Ret, sizeof(FLOAT), &dwNumberOfBytesRead);
	return Ret;
}
D3DXVECTOR3 X64_Read_D3DXVECTOR3(HANDLE hProcess, ULONG64 Address)
{
	D3DXVECTOR3 Ret;
	SIZE_T dwNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPCVOID)Address, &Ret, sizeof(D3DXVECTOR3), &dwNumberOfBytesRead);
	return Ret;
}
D3DXVECTOR4 X64_Read_D3DXVECTOR4(HANDLE hProcess, ULONG64 Address)
{
	D3DXVECTOR4 Ret;
	SIZE_T dwNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPCVOID)Address, &Ret, sizeof(D3DXVECTOR4), &dwNumberOfBytesRead);
	return Ret;
}
D3DXMATRIX X64_Read_D3DXMATRIX(HANDLE hProcess, ULONG64 Address)
{
	D3DXMATRIX Ret;
	SIZE_T dwNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPCVOID)Address, &Ret, sizeof(D3DXMATRIX), &dwNumberOfBytesRead);
	return Ret;
}
VOID X64_Write_DOUBLE(HANDLE hProcess, ULONG64 Address, DOUBLE Value)
{
	SIZE_T dwNumberOfBytesRead;
	WriteProcessMemory(hProcess, (LPVOID)Address, &Value, sizeof(DOUBLE), &dwNumberOfBytesRead);
}
VOID X64_Write_Int(HANDLE hProcess, ULONG64 Address, ULONG64 Value)
{
	DWORD OldProt;
	SIZE_T size = sizeof(ULONG64);
	ULONG64 ProtectAddress;
	SIZE_T dwNumberOfBytesRead;
	ProtectAddress = Address;
	VirtualProtectEx(hProcess, (PVOID)ProtectAddress, size, PAGE_EXECUTE_READWRITE, &OldProt);

	WriteProcessMemory(hProcess, (LPVOID)Address, &Value, sizeof(ULONG64), &dwNumberOfBytesRead);
	ProtectAddress = Address;

	VirtualProtectEx(hProcess, (PVOID)ProtectAddress, size, OldProt, &OldProt);
}
VOID X64_Write_(HANDLE hProcess, ULONG64 Address, PVOID Source, DWORD Length)
{
	DWORD OldProt;
	SIZE_T size = Length;
	ULONG64 ProtectAddress;
	ProtectAddress = Address;
	SIZE_T dwNumberOfBytesRead;

	VirtualProtectEx(hProcess, (PVOID)ProtectAddress, size, PAGE_EXECUTE_READWRITE, &OldProt);

	WriteProcessMemory(hProcess, (LPVOID)Address, Source, Length, &dwNumberOfBytesRead);

	ProtectAddress = Address;

	VirtualProtectEx(hProcess, (PVOID)ProtectAddress, size, OldProt, &OldProt);
}
VOID X64_Write_FLOAT(HANDLE hProcess, ULONG64 Address, FLOAT Value)
{

	SIZE_T dwNumberOfBytesRead;

	WriteProcessMemory(hProcess, (LPVOID)Address, &Value, sizeof(FLOAT), &dwNumberOfBytesRead);
}

BOOLEAN GetGNames(int _id, CHAR* Temp)
{
	ULONG64 GNamesAddress;
	SIZE_T dwNumberOfBytesRead;
	int IdDiv = 0, Idtemp = 0;

	if (_id < 0 || _id > 200000)
	{
		return FALSE;
	}

	GNamesAddress = X64_Read_Int(GameHanle, GameBase + GNames);
	if (GNamesAddress)
	{
		IdDiv = _id / 0x4000;
		Idtemp = _id % 0x4000;
		ULONG64 fNamePtr = X64_Read_Int(GameHanle, GNamesAddress + IdDiv * 8);
		ULONG64 fName = X64_Read_Int(GameHanle, fNamePtr + 8 * Idtemp);
		fName = fName + 0x10;	
		ReadProcessMemory(GameHanle, (LPCVOID)fName, Temp, 64, &dwNumberOfBytesRead);
		return TRUE;


	}

	return FALSE;

}
BOOLEAN IsPlayer(DWORD Id)
{
	CHAR *StringFitter3 = "PlayerMale_A PlayerMale_A_C PlayerFemale_A PlayerFemale_A_C";
	CHAR Temp[64];
	RtlZeroMemory(&Temp, sizeof(Temp));
	if (GetGNames(Id, Temp))
	{
		if (strstr(StringFitter3, Temp))
		{
			return TRUE;
		}
	}
	return FALSE;
}
BOOLEAN IsVehicle(DWORD Id,CHAR* Temp2)
{

	CHAR Temp[64];
	RtlZeroMemory(&Temp, sizeof(Temp));
	if (GetGNames(Id, Temp))
	{
		if (!strcmp(Temp, "DeathDropItemPackage_C"))
		{
			if (Temp2)
			{
				RtlCopyMemory(Temp2, "战利品", 64);
			}
			return TRUE;
		}
		if (!strcmp(Temp, "Carapackage_RedBox_C"))
		{
			if (Temp2)
			{
				RtlCopyMemory(Temp2, "空投箱", 64);
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOLEAN IsGoods(WCHAR* Temp)
{
	//WCHAR* StringFitter2 = L"Vector Tommy Gun Micro UZI UMP9 AKM Groza SKS Kar98K M24 VSS M416 M16A4 SCAR-L M249 S686 S1897 S12K"; //写物品名称
	
	if (!wcscmp(Temp, L"Vector"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"Tommy Gun"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"Micro UZI"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"UMP9"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"AKM"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"SKS"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"VSS"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"M416"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"M16A4"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"SCAR-L"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"S686"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"S1897"))
	{
		return TRUE;
	}
	if (!wcscmp(Temp, L"S12K"))
	{
		return TRUE;
	}	
	return FALSE;
}
D3DXVECTOR3 SubD3DXVECTOR3(D3DXVECTOR3 VecA, D3DXVECTOR3 VecB)
{
	D3DXVECTOR3 M;
	M.x = VecA.x - VecB.x;
	M.y = VecA.y - VecB.y;
	M.z = VecA.z - VecB.z;
	return M;
}
D3DXVECTOR3 AddD3DXVECTOR3(D3DXVECTOR3 VecA, D3DXVECTOR3 VecB)
{
	D3DXVECTOR3 M;
	M.x = VecA.x + VecB.x;
	M.y = VecA.y + VecB.y;
	M.z = VecA.z + VecB.z;
	return M;
}
FLOAT GetDistance(FLOAT X, FLOAT Y, FLOAT X1, FLOAT Y1)
{
	FLOAT XX, YY;
	XX = X - X1;
	YY = Y - Y1;
	return sqrt(XX * XX + YY * YY);
}
VOID GetAxes(D3DXVECTOR3 Rotation, D3DXVECTOR3* X, D3DXVECTOR3* Y, D3DXVECTOR3* Z)
{
	float SP = 0, CP = 0, SY = 0, CY = 0, SR = 0, CR = 0;

	SP = sin(Rotation.x * Pi / 180);
	CP = cos(Rotation.x * Pi / 180);
	SY = sin(Rotation.y * Pi / 180);
	CY = cos(Rotation.y * Pi / 180);
	SR = sin(Rotation.z * Pi / 180);
	CR = cos(Rotation.z * Pi / 180);

	X->x = CP * CY;
	X->y = CP * SY;
	X->z = SP;

	Y->x = SR * SP * CY - CR * SY;
	Y->y = SR * SP * SY + CR * CY;
	Y->z = -SR * CP;

	Z->x = -(CR * SP * CY + SR * SY);
	Z->y = CY * SR - CR * SP * SY;
	Z->z = CR * CP;

}
float DotProduct(D3DXVECTOR3 VecA, D3DXVECTOR3 VecB)
{
	return VecA.x * VecB.x + VecA.y * VecB.y + VecA.z * VecB.z;
}