


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ͷ�ļ�
#include <Windows.h>
#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cstack.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �궨��
#define EXTERNC extern "C"
#define NAKED __declspec(naked)
#define EXPORT __declspec(dllexport)

#define ALCPP EXPORT NAKED
#define ALSTD EXTERNC EXPORT NAKED void __stdcall
#define ALCFAST EXTERNC EXPORT NAKED void __fastcall
#define ALCDECL EXTERNC NAKED void __cdecl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define HOOK_LEN 0xC
// ����ԭʼָ��
BYTE Ori_Code[HOOK_LEN] = {0x00};
BYTE HookCode[HOOK_LEN] = { 0x48, 0xB8, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x50, 0xC3 };

void HookApi();
void UnHookApi();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AheadLib �����ռ�
namespace AheadLib
{
	HMODULE m_hModule = NULL;		// ԭʼģ����
	DWORD m_dwReturn[23] = {0};		// ԭʼ�������ص�ַ


	// ����ԭʼģ��
	inline BOOL WINAPI Load()
	{
		TCHAR tzPath[MAX_PATH];
		TCHAR tzTemp[MAX_PATH * 2];

		lstrcpy(tzPath, TEXT("TcOrg"));
		m_hModule = LoadLibrary(tzPath);
		if (m_hModule == NULL)
		{
			wsprintf(tzTemp, TEXT("�޷����� %s�������޷��������С�"), tzPath);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		}

		return (m_hModule != NULL);	
	}
		
	// �ͷ�ԭʼģ��
	inline VOID WINAPI Free()
	{
		if (m_hModule)
		{
			FreeLibrary(m_hModule);
		}
	}

	// ��ȡԭʼ������ַ
	FARPROC WINAPI GetAddress(PCSTR pszProcName)
	{
		//myStack.push(pszProcName);

		string dbgStr = "Tc::Begin--> ";

		ostringstream os;
		os<<dbgStr<<myStack.size()<<" "<<pszProcName;
		::OutputDebugString(os.str().c_str());

		FARPROC fpAddress;
		CHAR szProcName[64];
		TCHAR tzTemp[MAX_PATH];

		fpAddress = GetProcAddress(m_hModule, pszProcName);
		if (fpAddress == NULL)
		{
			if (HIWORD(pszProcName) == 0)
			{
				wsprintfA(szProcName, "%d", pszProcName);
				pszProcName = szProcName;
			}

			wsprintf(tzTemp, TEXT("�޷��ҵ����� %hs�������޷��������С�"), pszProcName);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
			ExitProcess(-2);
		}

		return fpAddress;
	}

	 inline VOID WINAPI  PopDbgStrFuncEnd(PCSTR pszProcName)
	{
		// ���淵�ص�ַ
		__asm PUSH EDI;
		__asm PUSH ESI;
		__asm PUSH EBX;
		__asm PUSH EAX;
		__asm PUSH ECX;
		__asm PUSH EDX;

		//myStack.push("TcEnd");
// 		string dbgStr = "Tc::E n d--> ";
// 		ostringstream os;
// 		if (myStack.size()>0)
// 		{
// 			os<<dbgStr<<myStack.pop()<<" "<<myStack.size();
// 		}
// 		else
// 		{
// 			os<<dbgStr<<" "<<myStack.size();
// 		}
		::OutputDebugString(pszProcName);
		__asm POP EDX;
		__asm POP ECX;
		__asm POP EAX;
		__asm POP EBX;
		__asm POP ESI;
		__asm POP EDI;
		
	}

}
using namespace AheadLib;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//������Hook��API -- ��ֵ�ĳ���ҪHook��API��ע�����������ֵ����
static HMODULE (WINAPI * kernel_BeHooked_Api)(LPCSTR lpFileName) = LoadLibraryA;

//�����Լ���API�������ͷ������������������ı���һ�� -- ԭAPI����ת������ִ��
HMODULE WINAPI My_Hook_Api(LPCSTR lpFileName)
{
	///////////////////////////////////////////////////////////////////
	//  ������ԭAPIִ��ǰ��Ҫ������
	///////////////////////////////////////////////////////////////////
	DWORD PatchAddr    = 0;              //��Ϊʵ��Ҫ�����ĵ�ַ
	BYTE  oldData[2]   = {0x74,0x1f};    //��Ϊʵ�ʵĴ���������
	BYTE  patchData[2] = {0x74,0x00};    //��Ϊ��������
	BYTE  bufRead[2]   = {0};            //��Ϊʵ��Ҫ��ȡ���̵��ֽ���

	DWORD dwOldProtect;
	DWORD bytesReadWritten;
	///////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////
	//  ִ��ԭAPI
	///////////////////////////////////////////////////////////////////
	UnHookApi();
	//  ��ע���޸Ĳ����ͷ���ֵ����
	HMODULE hRet = kernel_BeHooked_Api(lpFileName); 
	HookApi();
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	//  ������ԭAPIִ�к���Ҫ������
	///////////////////////////////////////////////////////////////////
	PatchAddr = (DWORD)hRet + 0x103C; //��Ϊ��Ҫ������Ŀ���ַ��
	HANDLE hProcess = GetCurrentProcess();

	VirtualProtect((LPVOID)PatchAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
	ReadProcessMemory(hProcess, (LPVOID)PatchAddr, (LPVOID)bufRead, 2, &bytesReadWritten);
	if (memcmp(bufRead, oldData, 2) == 0)
	{
		OutputDebugString(TEXT("[PYG] Found the target!"));
		WriteProcessMemory(hProcess, (LPVOID)PatchAddr, &patchData, 2, &bytesReadWritten);
		OutputDebugString(TEXT("[PYG] Patch successfully!"));
	}
	VirtualProtect((LPVOID)PatchAddr, sizeof(DWORD), dwOldProtect, &dwOldProtect);
	///////////////////////////////////////////////////////////////////

	//ע�ⷵ��ֵ����
	return hRet;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HookApi()
{
	DWORD dwOldProtect;
	//��ȡ�ڴ�Ȩ��
	::VirtualProtect(kernel_BeHooked_Api, HOOK_LEN, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	//����ԭ��������
	memcpy(Ori_Code, kernel_BeHooked_Api, HOOK_LEN);
	//�����Լ������ĵ�ַ
	*(PDWORD)(HookCode+2) = (DWORD)&My_Hook_Api;
	 //д������ڴ�
	memcpy(kernel_BeHooked_Api, HookCode, HOOK_LEN);
	//�ָ��ڴ�Ȩ��
	::VirtualProtect(kernel_BeHooked_Api, HOOK_LEN, dwOldProtect, &dwOldProtect);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UnHookApi()
{
	DWORD dwOldProtect;
	::VirtualProtect(kernel_BeHooked_Api, HOOK_LEN, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(kernel_BeHooked_Api, Ori_Code, HOOK_LEN);
	::VirtualProtect(kernel_BeHooked_Api, HOOK_LEN, dwOldProtect, &dwOldProtect);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ں���
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		if(Load())
		{
			TCHAR szAppName[MAX_PATH]  = TEXT("TdxW.exe");	//���Ϊ��Ӧ��Dll�����ļ���
			TCHAR szFullPath[MAX_PATH] = {0};
			int nLength = 0;
			nLength = GetModuleFileName(NULL, szFullPath, MAX_PATH);
			PathStripPath(szFullPath);
			if (StrCmpI(szAppName, szFullPath) == 0)
			{
				OutputDebugString(TEXT("[PYG] Dll Loaded successfully!"));
				HookApi();
			}
		}
		else
		{
			return FALSE;
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Free();
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
#pragma comment(linker, "/EXPORT:TC_AfterStatusChange=_AheadLib_TC_AfterStatusChange,@1")
#pragma comment(linker, "/EXPORT:TC_ArrangeWnd=_AheadLib_TC_ArrangeWnd,@2")
#pragma comment(linker, "/EXPORT:TC_BeforeHideWT=_AheadLib_TC_BeforeHideWT,@3")
#pragma comment(linker, "/EXPORT:TC_CallExtrenURL=_AheadLib_TC_CallExtrenURL,@4")
#pragma comment(linker, "/EXPORT:TC_CreateAll=_AheadLib_TC_CreateAll,@5")
#pragma comment(linker, "/EXPORT:TC_DoGridJy=_AheadLib_TC_DoGridJy,@6")
#pragma comment(linker, "/EXPORT:TC_DoLevinGN=_AheadLib_TC_DoLevinGN,@7")
#pragma comment(linker, "/EXPORT:TC_DoLevinJy=_AheadLib_TC_DoLevinJy,@8")
#pragma comment(linker, "/EXPORT:TC_GetClientInfo=_AheadLib_TC_GetClientInfo,@9")
#pragma comment(linker, "/EXPORT:TC_GetDlg=_AheadLib_TC_GetDlg,@10")
#pragma comment(linker, "/EXPORT:TC_GetJyStatus=_AheadLib_TC_GetJyStatus,@11")
#pragma comment(linker, "/EXPORT:TC_GetL2Info=_AheadLib_TC_GetL2Info,@12")
#pragma comment(linker, "/EXPORT:TC_GetLoginRet=_AheadLib_TC_GetLoginRet,@13")
#pragma comment(linker, "/EXPORT:TC_GetRightInfo=_AheadLib_TC_GetRightInfo,@14")
#pragma comment(linker, "/EXPORT:TC_GetTCInfo=_AheadLib_TC_GetTCInfo,@15")
#pragma comment(linker, "/EXPORT:TC_GetVersion=_AheadLib_TC_GetVersion,@16")
#pragma comment(linker, "/EXPORT:TC_Init_Environ=_AheadLib_TC_Init_Environ,@17")
#pragma comment(linker, "/EXPORT:TC_Login=_AheadLib_TC_Login,@18")
#pragma comment(linker, "/EXPORT:TC_PreTransMsg=_AheadLib_TC_PreTransMsg,@19")
#pragma comment(linker, "/EXPORT:TC_RegisterCallBack=_AheadLib_TC_RegisterCallBack,@20")
#pragma comment(linker, "/EXPORT:TC_SetFeedBackMsg=_AheadLib_TC_SetFeedBackMsg,@21")
#pragma comment(linker, "/EXPORT:TC_SetVersionInfo=_AheadLib_TC_SetVersionInfo,@22")
#pragma comment(linker, "/EXPORT:TC_Uninit=_AheadLib_TC_Uninit,@23")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_AfterStatusChange(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[0 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_AfterStatusChange")();

	PopDbgStrFuncEnd("TC_AfterStatusChange");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[0 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_ArrangeWnd(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[1 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_ArrangeWnd")();

	PopDbgStrFuncEnd("TC_ArrangeWnd");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[1 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_BeforeHideWT(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[2 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_BeforeHideWT")();

	PopDbgStrFuncEnd("TC_BeforeHideWT");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[2 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_CallExtrenURL(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[3 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_CallExtrenURL")();

	PopDbgStrFuncEnd("TC_CallExtrenURL");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[3 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_CreateAll(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[4 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_CreateAll")();

	PopDbgStrFuncEnd("TC_CreateAll");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[4 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_DoGridJy(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[5 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_DoGridJy")();

	PopDbgStrFuncEnd("TC_DoGridJy");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[5 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_DoLevinGN(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[6 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_DoLevinGN")();

	PopDbgStrFuncEnd("TC_DoLevinGN");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[6 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_DoLevinJy(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[7 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_DoLevinJy")();

	PopDbgStrFuncEnd("TC_DoLevinJy");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[7 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetClientInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[8 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetClientInfo")();

	PopDbgStrFuncEnd("TC_GetClientInfo");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[8 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetDlg(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[9 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetDlg")();

	PopDbgStrFuncEnd("TC_GetDlg");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[9 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetJyStatus(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[10 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetJyStatus")();

	PopDbgStrFuncEnd("TC_GetJyStatus");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[10 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetL2Info(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[11 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetL2Info")();

	PopDbgStrFuncEnd("TC_GetL2Info");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[11 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetLoginRet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[12 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetLoginRet")();

	PopDbgStrFuncEnd("TC_GetLoginRet");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[12 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetRightInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[13 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetRightInfo")();

	PopDbgStrFuncEnd("TC_GetRightInfo");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[13 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetTCInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[14 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetTCInfo")();

	PopDbgStrFuncEnd("TC_GetTCInfo");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[14 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_GetVersion(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[15 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_GetVersion")();

	PopDbgStrFuncEnd("TC_GetVersion");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[15 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_Init_Environ(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[16 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_Init_Environ")();

	PopDbgStrFuncEnd("TC_Init_Environ");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[16 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_Login(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[17 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_Login")();

	PopDbgStrFuncEnd("TC_Login");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[17 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_PreTransMsg(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[18 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_PreTransMsg")();

	PopDbgStrFuncEnd("TC_PreTransMsg");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[18 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_RegisterCallBack(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[19 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_RegisterCallBack")();

	PopDbgStrFuncEnd("TC_RegisterCallBack");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[19 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_SetFeedBackMsg(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[20 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_SetFeedBackMsg")();

	PopDbgStrFuncEnd("TC_SetFeedBackMsg");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[20 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_SetVersionInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[21 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_SetVersionInfo")();

	PopDbgStrFuncEnd("TC_SetVersionInfo");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[21 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TC_Uninit(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[22 * TYPE long];

	// ����ԭʼ����
	GetAddress("TC_Uninit")();

	PopDbgStrFuncEnd("TC_Uninit");
	// ת�������ص�ַ
	__asm JMP m_dwReturn[22 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
