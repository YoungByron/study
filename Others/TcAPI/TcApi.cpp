


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ͷ�ļ�
//#include <Windows.h>
#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib")
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cstack.h"
//#include "tradedlg.h"
#include "resource.h"


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

HINSTANCE hinst = NULL;     
HWND hwndDLG = NULL;     
BOOL CALLBACK DlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);     
BOOL CALLBACK AboutProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);     
void ShowDlg();    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AheadLib �����ռ�
namespace AheadLib
{
	HMODULE g_Mdl_TcAPI = NULL;		// ԭʼģ����
	HMODULE g_Mdl_AddInStock = NULL;		// ԭʼģ����
	DWORD m_dwReturn[151] = {0};		// ԭʼ�������ص�ַ


	// ����ԭʼģ��
	inline BOOL WINAPI Load()
	{
		TCHAR tzPath[MAX_PATH];
		TCHAR tzTemp[MAX_PATH * 2];

		lstrcpy(tzPath, TEXT("TcApiOrg.dll"));
		g_Mdl_TcAPI = LoadLibrary(tzPath);
		if (g_Mdl_TcAPI == NULL)
		{
			wsprintf(tzTemp, TEXT("�޷����� %s�������޷��������С�"), tzPath);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		}

		g_Mdl_AddInStock = LoadLibrary(TEXT(".\\TCPlugins\\AddInStock.dll"));

		return (g_Mdl_TcAPI != NULL);	
	}

	// �ͷ�ԭʼģ��
	inline VOID WINAPI Free()
	{
		if (g_Mdl_TcAPI)
		{
			FreeLibrary(g_Mdl_TcAPI);
		}
		if (g_Mdl_AddInStock)
		{
			FreeLibrary(g_Mdl_AddInStock);
		}
	}

	void hex2str(const int int_temp, char* r)  
	{  
		//stringstream stream;  
		//char* s = new char[12];             //�趨12λ���ڴ洢32λintֵ�㹻  
		char s[12] = {0};  
		//char r[12] = {0};  
		itoa(int_temp,s,16);            //itoa���������ʵ�֣���������C�к�������C++���Ƽ������ķ���  
		
		for (int i=strlen(s);i<8;i++)
		{
			r[i-strlen(s)] = '0';
		}
		strcat(r, s);
		//sprintf(s, "%8x\n", int_temp);
		r[8] = 0;
	} 

	void outHexString(void* c)
	{
		stringstream stream;  
		char s[12] = {0}; 
		stream.clear();
		stream.str("");

		int iLen=200;

		while (((BYTE*)(c))[iLen] == 0)
		{
			iLen--;
		}

		for (int i=0; i<=iLen; i++)
		{
			memset(s, 0, 12);
			sprintf(s, "%02X ",((BYTE*)(c))[i]);
			stream<<s; 
		}
		::OutputDebugString(stream.str().c_str());
	}


	// ��ȡԭʼ������ַ
	FARPROC WINAPI GetAddress(PCSTR pszProcName)
	{
		//::OutputDebugString(pszProcName);

// 		myStack.push(pszProcName);
// 
// 		string dbgStr = "TcAPI::Begin--> ";
// 
// 		ostringstream os;
// 		os<<dbgStr<<myStack.size()<<" "<<pszProcName;
// 		::OutputDebugString(os.str().c_str());

		//������ò���
/*
		ESP+00��00178208   19449630	�����ṹ��ָ��
		ESP+04��0017820C   00000079
		ESP+08��00178210   72F3A7E4  mfc42.72F3A7E4
		ESP+12��00178214   00000000
*/
		DWORD   a,b,c,d;   
		//unsigned int   *c   =   &a;   
		__asm   //��������Ƕ���...   
		{   
			//mov   eax,   c;   //c�д洢��a�ĵ�ַ->eax     
			//mov   eax,   [eax];   //a��ֵ->eax   
			//ע��ֱ��mov   eax,   [c]�Ǵ����   
			//mov   ebx,   b;   //����������ֱ�Ӷ�ebx��ֵ   
			//lea   eax,   [eax+ebx];   

			mov eax, [ebp+12] ;
			mov a, eax ;
			mov eax, [ebp+16] ;
			mov b, eax ;
			mov eax, [ebp+20] ;
			mov c, eax ;
			mov eax, [ebp+24] ;
			mov d, eax ;
		}   //��Ƕ��ಿ�ֽ���... TcSdk_Send

		stringstream stream;  
		char s[12] = {0}; 

		/*
		[8964] TcSdk_SetParam                  ---> a: 05e750c0 b: 000000ad c: 07f888e8 d: 00000000 ==Str:
		[8964] TcSdk_Send                      ---> a: 05e40ba8 b: 04910000 c: 049100ca d: 00000000
		[8964] CC E6 DD 04 00 00 00 00 05 00 79 00 0A 00 31 32 30 36 30 30 37 30 35 00 02 00 82 00 04 00 44 00 00 00 02 00 7D 00 04 00 00 00 00 00 05 00 7B 00 0B 00 30 36 30 32 35 37 32 31 31 38 00 05 00 8C 00 07 00 31 35 39 39 31 35 00 05 00 8D 00 07 00 B4 B4 D2 B5 B0 E5 00 05 00 91 00 06 00 31 2E 30 30 30 00 02 00 90 00 04 00 6C 1A 01 00 02 00 A6 00 04 00 04 00 00 00 05 00 5E 02 01 00 00 02 00 85 00 04 00 00 00 00 00 05 00 AD 00 01
		[8964] TcSdk_UpdateTcJobParameterSet   ---> a: 0827f8e0 b: 05e750c0 c: 05ec27e8 d: 0076034e
		[8964] CC E6 DD 04 00 00 00 00 05 00 79 00 0A 00 31 32 30 36 30 30 37 30 35 00 02 00 82 00 04 00 44 00 00 00 02 00 7D 00 04 00 00 00 00 00 05 00 7B 00 0B 00 30 36 30 32 35 37 32 31 31 38 00 05 00 8C 00 07 00 31 35 39 39 31 35 00 05 00 8D 00 07 00 B4 B4 D2 B5 B0 E5 00 05 00 91 00 06 00 31 2E 30 30 30 00 02 00 90 00 04 00 6C 1A 01 00 02 00 A6 00 04 00 04 00 00 00 05 00 5E 02 01 00 00 02 00 85 00 04 00 00 00 00 00 05 00 AD 00 01
		[8964] TcSdk_ResumeTcJob               ---> a: 0827f8e0 b: 0827f8e0 c: 05e750c0 d: 05ec27e8

		*/
		if (strstr(pszProcName, "TcSdk_SetParam") == pszProcName) 
		{
			outHexString((void*)a);
		}

		if (strstr(pszProcName, "TcSdk_UpdateTcJobParameterSet") == pszProcName) 
		{
			outHexString((void*)b);
		}

		if (strstr(pszProcName, "TcSdk_ResumeTcJob") == pszProcName) 
		{
			outHexString((void*)c);
		}

		stream.clear();
		stream.str("");
		stream<<pszProcName;
		while (stream.str().length()<32)
		{
			stream<<" ";
		}


		memset(s, 0, 12);
		hex2str(a, s);
		stream<<"---> a: "<<s; 

		memset(s, 0, 12);
		hex2str(b, s);
		stream<<" b: "<<s; 

		memset(s, 0, 12);
		hex2str(c, s);
		stream<<" c: "<<s; 

		memset(s, 0, 12);
		hex2str(d, s);
		stream<<" d: "<<s; 


//		stream<<pszProcName<<" a: "<<hex2str(a)<<" b: "<<hex2str(b)<<" c: "<<hex2str(c)<<" d: "<<hex2str(d); 

		if (strstr(pszProcName, "TcSdk_SetParam") == pszProcName)
		{
			if (0 == IsBadReadPtr((void *)c, 4))
			{
				stream << " ==Str:" << (char*)c;
			}
		}
		if (strstr(pszProcName, "TcSdk_SetParamLong") == pszProcName)
		{
			{
				stream << " ==Long:" << (long)c;
			}
		}
		if (strstr(pszProcName, "TcSdk_SetParamFloat") == pszProcName)
		{
			{
				stream << " ==Float:" << (float)c;
			}
		}
// 		if (0 == IsBadReadPtr((void *)b, 4))
// 		{
// 			stream<<" bStr:"<<(char*)b; 
// 		}
		//stream<< endl; 

// 		stream<<" [";
// 		
		//if (stream.str().length() > 1)
		{
			::OutputDebugString(stream.str().c_str());
		}


		FARPROC fpAddress;
		CHAR szProcName[64];
		TCHAR tzTemp[MAX_PATH];

		fpAddress = GetProcAddress(g_Mdl_TcAPI, pszProcName);
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

	VOID WINAPI  PopDbgStrFuncEnd()
	{
//		::OutputDebugString("TcAPI::E n d*** ");
// 		string dbgStr = "TcAPI::E n d--> ";
// 		ostringstream os;
// 		if (myStack.size()>0)
// 		{
// 			os<<dbgStr<<myStack.pop()<<" "<<myStack.size();
// 		}
// 		else
// 		{
// 			os<<dbgStr<<" "<<myStack.size();
// 		}
// 		::OutputDebugString(os.str().c_str());
		
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
			hinst = (HINSTANCE)hModule; 
			CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ShowDlg,NULL,NULL,NULL); 
			OutputDebugString(TEXT("******************CreateThread ShowDlg!"));
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

void ShowDlg()     
{     
	MSG msg;  
	hwndDLG = CreateDialog(hinst,MAKEINTRESOURCE(IDD_DIALOG1),NULL,(DLGPROC)DlgProc);     
	//  ShowWindow(hwndDLG, SW_SHOW);   
	if (hwndDLG)
	{
		OutputDebugString(TEXT("******************CreateDialog ShowDlg!"));
	}

	PeekMessage(&msg, hwndDLG, 0U, 0U, PM_NOREMOVE);  
	while(msg.message != WM_QUIT)  
	{  

		if(PeekMessage(&msg, hwndDLG, 0U, 0U, PM_REMOVE))  
		{  
			TranslateMessage(&msg);  
			DispatchMessage(&msg);  
		}  
		else  
		{  
			Sleep(10);  
		}  
	}  
}  

void CallTcAPI()
{
	FARPROC   addr;
	__asm push 0x0;
	__asm push 0x0;
	__asm mov ecx, 0;
	GetAddress("TcSdk_CreateParameterSet")();
	__asm mov addr, eax;

	//__asm mov esi,eax;
	//__asm push 0x79;
	//__asm push eax;
	//__asm mov ecx,esi;
	//GetAddress("TcSdk_SetParam")();

	string tmp = "120600705";
	const char* p = tmp.c_str();
	__asm mov eax, addr;
	__asm  push p;
	__asm  push 0x79;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParam")();

	// 				__asm  movsx eax,byte ptr ss:[esp+0x34];
	// 				__asm  mov edx,dword ptr ds:[esi];
	__asm mov eax, addr;
	__asm  push 0x44;	//43->���뼴ʱ�ɽ���44->������ʱ�ɽ���01->�޼�ί��
	__asm  push 0x82;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParamLong")();

	__asm mov eax, addr;
	__asm  push 0x00;
	__asm  push 0x7d;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParamLong")();

	tmp = "0602572118";
	p = tmp.c_str();
	__asm mov eax, addr;
	__asm  push p;
	__asm  push 0x7b;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParam")();

	tmp = "159915";
	p = tmp.c_str();
	__asm mov eax, addr;
	__asm  push p;
	__asm  push 0x8c;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParam")();

	tmp = "��ҵ��";
	p = tmp.c_str();
	__asm mov eax, addr;
	__asm  push p;
	__asm  push 0x8d;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParam")();

	__asm mov eax, addr;
	__asm  push 0x03;		//������С�����λ��������
	__asm  push 0x3f800000;	//�۸񸡵����� 1.0����ʵ��ί�м۸�
	__asm  push 0x91;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParamFloat")();

	__asm mov eax, addr;
	__asm  push 0x55f0; //��Ʊ���� 22000
	__asm  push 0x90;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParamLong")();

	__asm mov eax, addr;
	__asm  push 0x04; //�۸�ί�з�ʽ��0--X����Ӧѡ���combobox
	__asm  push 0xa6;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParamLong")();

	DWORD x25E = 0X00000000;
	void *px25E = &x25E;
	__asm mov eax, px25E;
	__asm  push eax;
	__asm  push 0x25e;
	__asm mov eax, addr;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParam")();

	__asm mov eax, addr;
	__asm  push 0x00;
	__asm  push 0x85;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParamLong")();

	__asm mov eax, addr;
	__asm  push 0x00;
	__asm  push 0xad;
	__asm  push eax;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_SetParam")();

	//���� 000004c7
	//TcSdk_SetParam                  ---> a: 17f00ea8 b: 000004c7 c: 0a82dd40 d: 00000000 cStr:1	
	//...
/*
					05979157  |> \8B47 20       mov eax,dword ptr ds:[edi+0x20]          ;  eax ��Ч��ַ
					0597915A  |>  8B16          mov edx,dword ptr ds:[esi]               ;  edx ���û���ַ
					0597915C  |.  6A 00         push 0x0
					0597915E  |.  68 24030000   push 0x324
					05979163  |.  68 88170000   push 0x1788
					05979168  |.  50            push eax
					05979169  |.  8BCE          mov ecx,esi
					0597916B  |.  FF52 64       call dword ptr ds:[edx+0x64]             ;  TC...->Send Call������Tc.05092C40(42c40/41c40)
					0597916E  |.  85F6          test esi,esi                             ;  TC...->Send Call������End
*/

				//MODULEINFO moduleinfo = {0};
				//GetModuleInformation(GetCurrentProcess(), GetModuleHandle("invest.dll"), &moduleinfo, sizeof(moduleinfo));
				////_tprintf(TEXT("with GetModuleInformation = 0x%x\r\n"), (DWORD)moduleinfo.EntryPoint+0x42c40);
				//px25E = (void*)((DWORD)moduleinfo.lpBaseOfDll+ 0x40744);

				//GetModuleInformation(GetCurrentProcess(), GetModuleHandle("tc.dll"), &moduleinfo, sizeof(moduleinfo));
				//void *px25E22 = (void*)((DWORD)moduleinfo.lpBaseOfDll+ 0x42c40);

				//__asm mov eax,px25E;

				//__asm  push 0x0;
				//__asm  push 0x324;
				//__asm  push 0x1788;
				//__asm  push eax;//invest.dll+0x40744
				//__asm  mov eax, px25E22;
				//__asm  mov ecx,esi;
				//__asm  call eax; //call tc.0x42c40

/*
04877C2A    FF75 1C         push dword ptr ss:[ebp+0x1C]	1
04877C2D    FF75 18         push dword ptr ss:[ebp+0x18]	0
04877C30    FF75 14         push dword ptr ss:[ebp+0x14]	0
04877C33    6A 02           push 0x2
04877C35    FF75 10         push dword ptr ss:[ebp+0x10]             ; Tc.047900CA
04877C38    FF75 0C         push dword ptr ss:[ebp+0xC]  Tc.dll+0x70000            ; Tc.??_C@_0BB@DBEFDDME@chinese?9hongkong?$AA@@estructor?8?$AA@@D@2@@std@@8r@D@2@@std@@8:allocator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > >::_Iterator<1>::_Iterator<1>>std::char_traits<char>,std::allocator<char> > > ><char...
04877C3B    E8 16E8FEFF     call TcApi.04866456                      ; //Real send...
04877C40    5D              pop ebp                                  ; Tc.??_C@_0BB@DBEFDDME@chinese?9hongkong?$AA@@estructor?8?$AA@@D@2@@std@@8r@D@2@@std@@8:allocator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > >::_Iterator<1>::_Iterator<1>>std::char_traits<char>,std::allocator<char> > > ><char...
04877C41    C3              retn
*/
MODULEINFO moduleinfo = {0};
GetModuleInformation(GetCurrentProcess(), GetModuleHandle("TC.dll"), &moduleinfo, sizeof(moduleinfo));
px25E = (void*)((DWORD)moduleinfo.lpBaseOfDll+ 0x70000);
void *px25ECA = (void*)((DWORD)moduleinfo.lpBaseOfDll + 0x700CA);

GetModuleInformation(GetCurrentProcess(), GetModuleHandle("TcApiOrg.dll"), &moduleinfo, sizeof(moduleinfo));
void *px25E22 = (void*)((DWORD)moduleinfo.lpBaseOfDll+ 0x6456);

__asm  push 0x1;
__asm  push 0x0;
__asm  push 0x0;
__asm  push 0x2;
__asm  push px25ECA;
__asm  push px25E;
__asm  mov eax, px25E22;
__asm  mov ecx,esi;
__asm  call eax; //call tc.0x42c40

//__asm mov eax,addr;
//__asm  push eax;
//__asm  push 0x01;
//__asm  push 0x00;
//__asm  push 0x00;
////__asm  mov ecx,esi;
//__asm  push ecx;
//__asm  push ecx;
//__asm  push ecx;
//__asm  mov ecx,esi;
//GetAddress("TcSdk_Send")();

	__asm mov eax, addr;
	__asm  push 0x00;
	__asm  push eax;
	__asm  push 0x00;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_UpdateTcJobParameterSet")();

	__asm mov eax, addr;
	__asm  push eax;
	__asm  push 0x00;
	__asm  push 0x00;
	__asm  mov ecx, esi;
	GetAddress("TcSdk_ResumeTcJob")();
}

void CallAddInStock()
{
	MODULEINFO moduleinfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(".\\TCPlugins\\AddinStock.dll"), &moduleinfo, sizeof(moduleinfo));
	//_tprintf(TEXT("with GetModuleInformation = 0x%x\r\n"), (DWORD)moduleinfo.lpBaseOfDll+0x42c40);
	typedef void(*LPFNC_SETEVENT)(
		DWORD,
		DWORD,
		const char*,
		const char*,
		DWORD,
		DWORD,
		DWORD,
		DWORD,
		DWORD,
		DWORD,
		DWORD,
		DWORD,
		DWORD
		);

	
	LPFNC_SETEVENT lpfunc;
	lpfunc = (LPFNC_SETEVENT)GetProcAddress(g_Mdl_AddInStock, "Addin_GetObject");

	if (lpfunc != NULL)
	{
		lpfunc = ((LPFNC_SETEVENT)((UINT)lpfunc + 0x31E3A0));

		string tmp = "159915";
		string tmp2 = "0602572116";
		lpfunc(
			0,
			0,
			tmp2.c_str(),
			tmp.c_str(),
			0x3FC70A3D,
			0X2710,
			0,
			(DWORD)moduleinfo.lpBaseOfDll + 0x4ADEB0,
			0,
			0,
			0,
			0,
			(DWORD)moduleinfo.lpBaseOfDll + 0x4ADEB0
			);
		//void *px25E = (void*)((DWORD)moduleinfo.lpBaseOfDll + 0x41c40);

		//__asm  push 0;
		//__asm  push 0;
		//__asm  push 0;
		//__asm  push 0;
		//__asm  push 0;
		//__asm  push 0;
		//__asm  push 0;
		//__asm  push 0X2710;
		//__asm  push 0x3FC70A3D;

		//const char* p = tmp.c_str();
		//__asm  push p;

		//const char* p2 = tmp2.c_str();
		//__asm  push p2;

		//__asm  push 0;
		//__asm  push 0;


		//__asm mov eax, lpfunc;
		//__asm  push eax;
		//__asm  mov ecx, ebp;
		//__asm  call eax;
	}
}
void CallAddInStock2()
{
	//������ť��� Base+0x349690;
	MODULEINFO moduleinfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(".\\TCPlugins\\AddinStock.dll"), &moduleinfo, sizeof(moduleinfo));
	void *px25E22 = (void*)((DWORD)moduleinfo.lpBaseOfDll + 0x349690);
	std::cout << "��������������������������������������::px25E22=" << px25E22 << std::endl;

	//std::string tmp = "�ⲿ�̵߳���";
	//const char* p = tmp.c_str();

	//std::string tmp2 = "ע��dll��תonbtn�ڲ���ִַ�е�";
	//const char* p2 = tmp2.c_str();

	__asm  mov eax, px25E22;
	__asm  mov ecx, esi;
	__asm  push lable;
	//__asm  push 0x0;
	//__asm  push p;
	//__asm  push p2;
	//__asm  push 0x0;
	__asm  jmp eax; //call 
	__asm  lable:
	//__asm  add esp, 0x10;
	return;

}

BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{  
	switch(message)  
	{  
	case WM_INITDIALOG:  
		{  
// 			RegisterHotKey(hDlg,1987,NULL,VK_HOME);//ע���ݼ�  
// 			OutputDebugString(TEXT("ע���ݼ�!"));
			ShowWindow(hDlg,SW_SHOWMINIMIZED);  
			//SetForegroundWindow(hDlg);  
		}  
		return (TRUE);  
	case WM_CLOSE:  
		{  
			EndDialog(hDlg,0);  
		}  
		return (TRUE);  
	case WM_HOTKEY:  
		{  
			if (LOWORD(wParam) == 1987)  
			{  
// 				OutputDebugString(TEXT("VK_HOMExxxxxxxxxxxx��ݼ�!"));
// 				if (IsWindowVisible(hDlg))  
// 					ShowWindow(hDlg,SW_HIDE);  
// 				else  
// 				{  
// 					ShowWindow(hDlg,SW_NORMAL);  
// 					SetForegroundWindow(hDlg);  
// 				}  
			}  
		}  
		return TRUE;  
	case WM_KEYUP:  
		{  
// 			if(LOWORD(wParam) == VK_END)  
// 				MessageBox (hDlg,"XXX","df",MB_OK);  
		}  
	case WM_COMMAND:  
		switch (LOWORD(wParam))  
		{  
		case IDCANCEL:  
			{  
				SendMessage(hDlg, WM_CLOSE, 0, 0);  
			}  
			return (TRUE);  
		case IDOK:  
			{  
// 				MessageBox(hDlg,"�۹���...��д������","��ʾ",MB_ICONINFORMATION);  
			CallTcAPI();
			//CallAddInStock();
			//CallAddInStock2();

		}
			return (TRUE);  
		}  
		return (FALSE);  
	}  
	return (FALSE);  
}  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
#pragma comment(linker, "/EXPORT:TcSdk_AbortPacket=_AheadLib_TcSdk_AbortPacket,@1")
#pragma comment(linker, "/EXPORT:TcSdk_AddSite=_AheadLib_TcSdk_AddSite,@2")
#pragma comment(linker, "/EXPORT:TcSdk_AddSiteGroup=_AheadLib_TcSdk_AddSiteGroup,@3")
#pragma comment(linker, "/EXPORT:TcSdk_CheckJymm=_AheadLib_TcSdk_CheckJymm,@4")
#pragma comment(linker, "/EXPORT:TcSdk_CloneTcJob=_AheadLib_TcSdk_CloneTcJob,@5")
#pragma comment(linker, "/EXPORT:TcSdk_ConvertToMatrix=_AheadLib_TcSdk_ConvertToMatrix,@6")
#pragma comment(linker, "/EXPORT:TcSdk_CopyJobErr=_AheadLib_TcSdk_CopyJobErr,@7")
#pragma comment(linker, "/EXPORT:TcSdk_CreateClient=_AheadLib_TcSdk_CreateClient,@8")
#pragma comment(linker, "/EXPORT:TcSdk_CreateParameterSet=_AheadLib_TcSdk_CreateParameterSet,@9")
#pragma comment(linker, "/EXPORT:TcSdk_DelSite=_AheadLib_TcSdk_DelSite,@10")
#pragma comment(linker, "/EXPORT:TcSdk_DelSiteGroup=_AheadLib_TcSdk_DelSiteGroup,@11")
#pragma comment(linker, "/EXPORT:TcSdk_DestroyClient=_AheadLib_TcSdk_DestroyClient,@12")
#pragma comment(linker, "/EXPORT:TcSdk_DestroyParameterSet=_AheadLib_TcSdk_DestroyParameterSet,@13")
#pragma comment(linker, "/EXPORT:TcSdk_Disconnect=_AheadLib_TcSdk_Disconnect,@14")
#pragma comment(linker, "/EXPORT:TcSdk_EnableReconnOnTimer=_AheadLib_TcSdk_EnableReconnOnTimer,@15")
#pragma comment(linker, "/EXPORT:TcSdk_ExitInstance=_AheadLib_TcSdk_ExitInstance,@16")
#pragma comment(linker, "/EXPORT:TcSdk_FetchNextRow=_AheadLib_TcSdk_FetchNextRow,@17")
#pragma comment(linker, "/EXPORT:TcSdk_FindBestSite=_AheadLib_TcSdk_FindBestSite,@18")
#pragma comment(linker, "/EXPORT:TcSdk_FormatBz=_AheadLib_TcSdk_FormatBz,@19")
#pragma comment(linker, "/EXPORT:TcSdk_FormatEtfMmbs=_AheadLib_TcSdk_FormatEtfMmbs,@20")
#pragma comment(linker, "/EXPORT:TcSdk_FormatFhbz=_AheadLib_TcSdk_FormatFhbz,@21")
#pragma comment(linker, "/EXPORT:TcSdk_FormatJjMmbz=_AheadLib_TcSdk_FormatJjMmbz,@22")
#pragma comment(linker, "/EXPORT:TcSdk_FormatLcMmbz=_AheadLib_TcSdk_FormatLcMmbz,@23")
#pragma comment(linker, "/EXPORT:TcSdk_FormatMmbz=_AheadLib_TcSdk_FormatMmbz,@24")
#pragma comment(linker, "/EXPORT:TcSdk_FormatScFlag=_AheadLib_TcSdk_FormatScFlag,@25")
#pragma comment(linker, "/EXPORT:TcSdk_FormatWtfs=_AheadLib_TcSdk_FormatWtfs,@26")
#pragma comment(linker, "/EXPORT:TcSdk_FormatZhlb=_AheadLib_TcSdk_FormatZhlb,@27")
#pragma comment(linker, "/EXPORT:TcSdk_FormatZhlbSm=_AheadLib_TcSdk_FormatZhlbSm,@28")
#pragma comment(linker, "/EXPORT:TcSdk_FreeClientConfig=_AheadLib_TcSdk_FreeClientConfig,@29")
#pragma comment(linker, "/EXPORT:TcSdk_GetAttachPtr=_AheadLib_TcSdk_GetAttachPtr,@30")
#pragma comment(linker, "/EXPORT:TcSdk_GetAttachSize=_AheadLib_TcSdk_GetAttachSize,@31")
#pragma comment(linker, "/EXPORT:TcSdk_GetCliModel=_AheadLib_TcSdk_GetCliModel,@32")
#pragma comment(linker, "/EXPORT:TcSdk_GetClient=_AheadLib_TcSdk_GetClient,@33")
#pragma comment(linker, "/EXPORT:TcSdk_GetClientConfig=_AheadLib_TcSdk_GetClientConfig,@34")
#pragma comment(linker, "/EXPORT:TcSdk_GetClientLong=_AheadLib_TcSdk_GetClientLong,@35")
#pragma comment(linker, "/EXPORT:TcSdk_GetConnID=_AheadLib_TcSdk_GetConnID,@36")
#pragma comment(linker, "/EXPORT:TcSdk_GetConstParameterSet=_AheadLib_TcSdk_GetConstParameterSet,@37")
#pragma comment(linker, "/EXPORT:TcSdk_GetDestHost=_AheadLib_TcSdk_GetDestHost,@38")
#pragma comment(linker, "/EXPORT:TcSdk_GetDestName=_AheadLib_TcSdk_GetDestName,@39")
#pragma comment(linker, "/EXPORT:TcSdk_GetDestPort=_AheadLib_TcSdk_GetDestPort,@40")
#pragma comment(linker, "/EXPORT:TcSdk_GetErrCode=_AheadLib_TcSdk_GetErrCode,@41")
#pragma comment(linker, "/EXPORT:TcSdk_GetErrInfo=_AheadLib_TcSdk_GetErrInfo,@42")
#pragma comment(linker, "/EXPORT:TcSdk_GetErrType=_AheadLib_TcSdk_GetErrType,@43")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldBinaryValueByID=_AheadLib_TcSdk_GetFieldBinaryValueByID,@44")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldBinaryValueByIdx=_AheadLib_TcSdk_GetFieldBinaryValueByIdx,@45")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldInfo=_AheadLib_TcSdk_GetFieldInfo,@46")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldInfoAt=_AheadLib_TcSdk_GetFieldInfoAt,@47")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldInfoOf=_AheadLib_TcSdk_GetFieldInfoOf,@48")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldNum=_AheadLib_TcSdk_GetFieldNum,@49")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldNumOf=_AheadLib_TcSdk_GetFieldNumOf,@50")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldValueByID=_AheadLib_TcSdk_GetFieldValueByID,@51")
#pragma comment(linker, "/EXPORT:TcSdk_GetFieldValueByIdx=_AheadLib_TcSdk_GetFieldValueByIdx,@52")
#pragma comment(linker, "/EXPORT:TcSdk_GetFloatLenByBz=_AheadLib_TcSdk_GetFloatLenByBz,@53")
#pragma comment(linker, "/EXPORT:TcSdk_GetFocusSite=_AheadLib_TcSdk_GetFocusSite,@54")
#pragma comment(linker, "/EXPORT:TcSdk_GetFuncID=_AheadLib_TcSdk_GetFuncID,@55")
#pragma comment(linker, "/EXPORT:TcSdk_GetFuncType=_AheadLib_TcSdk_GetFuncType,@56")
#pragma comment(linker, "/EXPORT:TcSdk_GetInputUID=_AheadLib_TcSdk_GetInputUID,@57")
#pragma comment(linker, "/EXPORT:TcSdk_GetInputUType=_AheadLib_TcSdk_GetInputUType,@58")
#pragma comment(linker, "/EXPORT:TcSdk_GetJobLong=_AheadLib_TcSdk_GetJobLong,@59")
#pragma comment(linker, "/EXPORT:TcSdk_GetJobStep=_AheadLib_TcSdk_GetJobStep,@60")
#pragma comment(linker, "/EXPORT:TcSdk_GetMachineInfo=_AheadLib_TcSdk_GetMachineInfo,@61")
#pragma comment(linker, "/EXPORT:TcSdk_GetNetFlowInfo=_AheadLib_TcSdk_GetNetFlowInfo,@62")
#pragma comment(linker, "/EXPORT:TcSdk_GetOemQstag=_AheadLib_TcSdk_GetOemQstag,@63")
#pragma comment(linker, "/EXPORT:TcSdk_GetPacketBuffer=_AheadLib_TcSdk_GetPacketBuffer,@64")
#pragma comment(linker, "/EXPORT:TcSdk_GetPacketFuncID=_AheadLib_TcSdk_GetPacketFuncID,@65")
#pragma comment(linker, "/EXPORT:TcSdk_GetPacketParam=_AheadLib_TcSdk_GetPacketParam,@66")
#pragma comment(linker, "/EXPORT:TcSdk_GetParam=_AheadLib_TcSdk_GetParam,@67")
#pragma comment(linker, "/EXPORT:TcSdk_GetParamAt=_AheadLib_TcSdk_GetParamAt,@68")
#pragma comment(linker, "/EXPORT:TcSdk_GetParameterSet=_AheadLib_TcSdk_GetParameterSet,@69")
#pragma comment(linker, "/EXPORT:TcSdk_GetQsid=_AheadLib_TcSdk_GetQsid,@70")
#pragma comment(linker, "/EXPORT:TcSdk_GetRealBranchID=_AheadLib_TcSdk_GetRealBranchID,@71")
#pragma comment(linker, "/EXPORT:TcSdk_GetRealBranchName=_AheadLib_TcSdk_GetRealBranchName,@72")
#pragma comment(linker, "/EXPORT:TcSdk_GetRespData=_AheadLib_TcSdk_GetRespData,@73")
#pragma comment(linker, "/EXPORT:TcSdk_GetResultSet=_AheadLib_TcSdk_GetResultSet,@74")
#pragma comment(linker, "/EXPORT:TcSdk_GetRowNum=_AheadLib_TcSdk_GetRowNum,@75")
#pragma comment(linker, "/EXPORT:TcSdk_GetSecurityData=_AheadLib_TcSdk_GetSecurityData,@76")
#pragma comment(linker, "/EXPORT:TcSdk_GetSecurityType=_AheadLib_TcSdk_GetSecurityType,@77")
#pragma comment(linker, "/EXPORT:TcSdk_GetServDesc=_AheadLib_TcSdk_GetServDesc,@78")
#pragma comment(linker, "/EXPORT:TcSdk_GetServID=_AheadLib_TcSdk_GetServID,@79")
#pragma comment(linker, "/EXPORT:TcSdk_GetServerTimeSpan=_AheadLib_TcSdk_GetServerTimeSpan,@80")
#pragma comment(linker, "/EXPORT:TcSdk_GetSubmitUID=_AheadLib_TcSdk_GetSubmitUID,@81")
#pragma comment(linker, "/EXPORT:TcSdk_GetSubmitUName=_AheadLib_TcSdk_GetSubmitUName,@82")
#pragma comment(linker, "/EXPORT:TcSdk_GetTcJobEvent=_AheadLib_TcSdk_GetTcJobEvent,@83")
#pragma comment(linker, "/EXPORT:TcSdk_GetZqxxByZqdm=_AheadLib_TcSdk_GetZqxxByZqdm,@84")
#pragma comment(linker, "/EXPORT:TcSdk_InitInstance=_AheadLib_TcSdk_InitInstance,@85")
#pragma comment(linker, "/EXPORT:TcSdk_IsConnectOK=_AheadLib_TcSdk_IsConnectOK,@86")
#pragma comment(linker, "/EXPORT:TcSdk_IsFuncCallJob=_AheadLib_TcSdk_IsFuncCallJob,@87")
#pragma comment(linker, "/EXPORT:TcSdk_IsLoginJob=_AheadLib_TcSdk_IsLoginJob,@88")
#pragma comment(linker, "/EXPORT:TcSdk_IsLogoutJob=_AheadLib_TcSdk_IsLogoutJob,@89")
#pragma comment(linker, "/EXPORT:TcSdk_IsReloginJob=_AheadLib_TcSdk_IsReloginJob,@90")
#pragma comment(linker, "/EXPORT:TcSdk_MergeJob=_AheadLib_TcSdk_MergeJob,@91")
#pragma comment(linker, "/EXPORT:TcSdk_QueryClient=_AheadLib_TcSdk_QueryClient,@92")
#pragma comment(linker, "/EXPORT:TcSdk_QueryTcJob=_AheadLib_TcSdk_QueryTcJob,@93")
#pragma comment(linker, "/EXPORT:TcSdk_Reconnect=_AheadLib_TcSdk_Reconnect,@94")
#pragma comment(linker, "/EXPORT:TcSdk_ReconnectManual=_AheadLib_TcSdk_ReconnectManual,@95")
#pragma comment(linker, "/EXPORT:TcSdk_ReleaseClient=_AheadLib_TcSdk_ReleaseClient,@96")
#pragma comment(linker, "/EXPORT:TcSdk_ReleaseTcJob=_AheadLib_TcSdk_ReleaseTcJob,@97")
#pragma comment(linker, "/EXPORT:TcSdk_RemoveParam=_AheadLib_TcSdk_RemoveParam,@98")
#pragma comment(linker, "/EXPORT:TcSdk_ResetParameterSet=_AheadLib_TcSdk_ResetParameterSet,@99")
#pragma comment(linker, "/EXPORT:TcSdk_ResumeTcJob=_AheadLib_TcSdk_ResumeTcJob,@100")
#pragma comment(linker, "/EXPORT:TcSdk_Rewind=_AheadLib_TcSdk_Rewind,@101")
#pragma comment(linker, "/EXPORT:TcSdk_SeekToLine=_AheadLib_TcSdk_SeekToLine,@102")
#pragma comment(linker, "/EXPORT:TcSdk_Send=_AheadLib_TcSdk_Send,@103")
#pragma comment(linker, "/EXPORT:TcSdk_SetAppStatus=_AheadLib_TcSdk_SetAppStatus,@104")
#pragma comment(linker, "/EXPORT:TcSdk_SetAttach=_AheadLib_TcSdk_SetAttach,@105")
#pragma comment(linker, "/EXPORT:TcSdk_SetBaseCliType=_AheadLib_TcSdk_SetBaseCliType,@106")
#pragma comment(linker, "/EXPORT:TcSdk_SetBaseClientVersion=_AheadLib_TcSdk_SetBaseClientVersion,@107")
#pragma comment(linker, "/EXPORT:TcSdk_SetBaseVipTermFlag=_AheadLib_TcSdk_SetBaseVipTermFlag,@108")
#pragma comment(linker, "/EXPORT:TcSdk_SetCliType=_AheadLib_TcSdk_SetCliType,@109")
#pragma comment(linker, "/EXPORT:TcSdk_SetClientAccount=_AheadLib_TcSdk_SetClientAccount,@110")
#pragma comment(linker, "/EXPORT:TcSdk_SetClientAddress=_AheadLib_TcSdk_SetClientAddress,@111")
#pragma comment(linker, "/EXPORT:TcSdk_SetClientEventHook=_AheadLib_TcSdk_SetClientEventHook,@112")
#pragma comment(linker, "/EXPORT:TcSdk_SetClientLong=_AheadLib_TcSdk_SetClientLong,@113")
#pragma comment(linker, "/EXPORT:TcSdk_SetClientVersion=_AheadLib_TcSdk_SetClientVersion,@114")
#pragma comment(linker, "/EXPORT:TcSdk_SetExtendHook=_AheadLib_TcSdk_SetExtendHook,@115")
#pragma comment(linker, "/EXPORT:TcSdk_SetFocusSite=_AheadLib_TcSdk_SetFocusSite,@116")
#pragma comment(linker, "/EXPORT:TcSdk_SetGrpID=_AheadLib_TcSdk_SetGrpID,@117")
#pragma comment(linker, "/EXPORT:TcSdk_SetJobEventHook=_AheadLib_TcSdk_SetJobEventHook,@118")
#pragma comment(linker, "/EXPORT:TcSdk_SetJobLong=_AheadLib_TcSdk_SetJobLong,@119")
#pragma comment(linker, "/EXPORT:TcSdk_SetJobStep=_AheadLib_TcSdk_SetJobStep,@120")
#pragma comment(linker, "/EXPORT:TcSdk_SetJournalHook=_AheadLib_TcSdk_SetJournalHook,@121")
#pragma comment(linker, "/EXPORT:TcSdk_SetPacketFilterHook=_AheadLib_TcSdk_SetPacketFilterHook,@122")
#pragma comment(linker, "/EXPORT:TcSdk_SetPacketParam=_AheadLib_TcSdk_SetPacketParam,@123")
#pragma comment(linker, "/EXPORT:TcSdk_SetPacketParamBinary=_AheadLib_TcSdk_SetPacketParamBinary,@124")
#pragma comment(linker, "/EXPORT:TcSdk_SetPacketParamByte=_AheadLib_TcSdk_SetPacketParamByte,@125")
#pragma comment(linker, "/EXPORT:TcSdk_SetPacketParamLong=_AheadLib_TcSdk_SetPacketParamLong,@126")
#pragma comment(linker, "/EXPORT:TcSdk_SetParam=_AheadLib_TcSdk_SetParam,@127")
#pragma comment(linker, "/EXPORT:TcSdk_SetParamBinary=_AheadLib_TcSdk_SetParamBinary,@128")
#pragma comment(linker, "/EXPORT:TcSdk_SetParamByte=_AheadLib_TcSdk_SetParamByte,@129")
#pragma comment(linker, "/EXPORT:TcSdk_SetParamFloat=_AheadLib_TcSdk_SetParamFloat,@130")
#pragma comment(linker, "/EXPORT:TcSdk_SetParamLong=_AheadLib_TcSdk_SetParamLong,@131")
#pragma comment(linker, "/EXPORT:TcSdk_SetParamULong=_AheadLib_TcSdk_SetParamULong,@132")
#pragma comment(linker, "/EXPORT:TcSdk_SetPid=_AheadLib_TcSdk_SetPid,@133")
#pragma comment(linker, "/EXPORT:TcSdk_SetProxy=_AheadLib_TcSdk_SetProxy,@134")
#pragma comment(linker, "/EXPORT:TcSdk_SetQsid=_AheadLib_TcSdk_SetQsid,@135")
#pragma comment(linker, "/EXPORT:TcSdk_SetReqData=_AheadLib_TcSdk_SetReqData,@136")
#pragma comment(linker, "/EXPORT:TcSdk_SetSecurityInfo=_AheadLib_TcSdk_SetSecurityInfo,@137")
#pragma comment(linker, "/EXPORT:TcSdk_SetServiceOption=_AheadLib_TcSdk_SetServiceOption,@138")
#pragma comment(linker, "/EXPORT:TcSdk_SetTdxBranchID=_AheadLib_TcSdk_SetTdxBranchID,@139")
#pragma comment(linker, "/EXPORT:TcSdk_SetTdxProxy=_AheadLib_TcSdk_SetTdxProxy,@140")
#pragma comment(linker, "/EXPORT:TcSdk_SetUserErr=_AheadLib_TcSdk_SetUserErr,@141")
#pragma comment(linker, "/EXPORT:TcSdk_SetVipTermFlag=_AheadLib_TcSdk_SetVipTermFlag,@142")
#pragma comment(linker, "/EXPORT:TcSdk_SuspendTcJob=_AheadLib_TcSdk_SuspendTcJob,@143")
#pragma comment(linker, "/EXPORT:TcSdk_TraderLogin=_AheadLib_TcSdk_TraderLogin,@144")
#pragma comment(linker, "/EXPORT:TcSdk_TraderLoginAs=_AheadLib_TcSdk_TraderLoginAs,@145")
#pragma comment(linker, "/EXPORT:TcSdk_TraderLogout=_AheadLib_TcSdk_TraderLogout,@146")
#pragma comment(linker, "/EXPORT:TcSdk_TryReconnectAll=_AheadLib_TcSdk_TryReconnectAll,@147")
#pragma comment(linker, "/EXPORT:TcSdk_UpdateCoreOption=_AheadLib_TcSdk_UpdateCoreOption,@148")
#pragma comment(linker, "/EXPORT:TcSdk_UpdateTcJobParameterSet=_AheadLib_TcSdk_UpdateTcJobParameterSet,@149")
#pragma comment(linker, "/EXPORT:TcSdk_WaitForTcJob=_AheadLib_TcSdk_WaitForTcJob,@150")
#pragma comment(linker, "/EXPORT:Version=_AheadLib_Version,@151")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_AbortPacket(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[0 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_AbortPacket")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[0 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_AddSite(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[1 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_AddSite")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[1 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_AddSiteGroup(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[2 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_AddSiteGroup")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[2 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_CheckJymm(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[3 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_CheckJymm")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[3 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_CloneTcJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[4 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_CloneTcJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[4 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_ConvertToMatrix(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[5 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_ConvertToMatrix")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[5 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_CopyJobErr(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[6 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_CopyJobErr")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[6 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_CreateClient(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[7 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_CreateClient")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[7 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_CreateParameterSet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[8 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_CreateParameterSet")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[8 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_DelSite(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[9 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_DelSite")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[9 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_DelSiteGroup(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[10 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_DelSiteGroup")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[10 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_DestroyClient(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[11 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_DestroyClient")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[11 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_DestroyParameterSet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[12 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_DestroyParameterSet")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[12 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_Disconnect(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[13 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_Disconnect")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[13 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_EnableReconnOnTimer(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[14 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_EnableReconnOnTimer")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[14 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_ExitInstance(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[15 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_ExitInstance")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[15 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FetchNextRow(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[16 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FetchNextRow")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[16 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FindBestSite(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[17 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FindBestSite")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[17 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatBz(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[18 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatBz")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[18 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatEtfMmbs(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[19 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatEtfMmbs")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[19 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatFhbz(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[20 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatFhbz")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[20 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatJjMmbz(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[21 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatJjMmbz")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[21 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatLcMmbz(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[22 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatLcMmbz")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[22 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatMmbz(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[23 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatMmbz")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[23 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatScFlag(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[24 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatScFlag")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[24 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatWtfs(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[25 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatWtfs")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[25 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatZhlb(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[26 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatZhlb")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[26 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FormatZhlbSm(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[27 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FormatZhlbSm")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[27 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_FreeClientConfig(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[28 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_FreeClientConfig")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[28 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetAttachPtr(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[29 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetAttachPtr")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[29 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetAttachSize(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[30 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetAttachSize")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[30 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetCliModel(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[31 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetCliModel")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[31 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetClient(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[32 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetClient")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[32 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetClientConfig(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[33 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetClientConfig")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[33 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetClientLong(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[34 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetClientLong")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[34 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetConnID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[35 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetConnID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[35 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetConstParameterSet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[36 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetConstParameterSet")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[36 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetDestHost(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[37 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetDestHost")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[37 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetDestName(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[38 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetDestName")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[38 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetDestPort(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[39 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetDestPort")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[39 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetErrCode(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[40 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetErrCode")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[40 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetErrInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[41 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetErrInfo")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[41 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetErrType(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[42 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetErrType")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[42 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldBinaryValueByID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[43 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldBinaryValueByID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[43 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldBinaryValueByIdx(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[44 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldBinaryValueByIdx")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[44 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[45 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldInfo")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[45 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldInfoAt(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[46 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldInfoAt")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[46 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldInfoOf(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[47 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldInfoOf")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[47 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldNum(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[48 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldNum")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[48 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldNumOf(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[49 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldNumOf")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[49 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldValueByID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[50 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldValueByID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[50 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFieldValueByIdx(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[51 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFieldValueByIdx")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[51 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFloatLenByBz(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[52 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFloatLenByBz")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[52 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFocusSite(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[53 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFocusSite")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[53 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFuncID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[54 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFuncID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[54 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetFuncType(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[55 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetFuncType")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[55 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetInputUID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[56 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetInputUID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[56 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetInputUType(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[57 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetInputUType")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[57 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetJobLong(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[58 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetJobLong")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[58 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetJobStep(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[59 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetJobStep")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[59 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetMachineInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[60 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetMachineInfo")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[60 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetNetFlowInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[61 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetNetFlowInfo")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[61 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetOemQstag(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[62 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetOemQstag")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[62 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetPacketBuffer(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[63 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetPacketBuffer")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[63 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetPacketFuncID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[64 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetPacketFuncID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[64 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetPacketParam(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[65 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetPacketParam")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[65 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetParam(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[66 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetParam")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[66 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetParamAt(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[67 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetParamAt")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[67 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetParameterSet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[68 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetParameterSet")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[68 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetQsid(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[69 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetQsid")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[69 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetRealBranchID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[70 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetRealBranchID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[70 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetRealBranchName(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[71 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetRealBranchName")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[71 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetRespData(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[72 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetRespData")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[72 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetResultSet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[73 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetResultSet")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[73 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetRowNum(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[74 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetRowNum")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[74 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetSecurityData(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[75 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetSecurityData")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[75 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetSecurityType(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[76 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetSecurityType")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[76 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetServDesc(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[77 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetServDesc")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[77 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetServID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[78 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetServID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[78 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetServerTimeSpan(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[79 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetServerTimeSpan")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[79 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetSubmitUID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[80 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetSubmitUID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[80 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetSubmitUName(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[81 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetSubmitUName")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[81 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetTcJobEvent(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[82 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetTcJobEvent")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[82 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_GetZqxxByZqdm(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[83 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_GetZqxxByZqdm")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[83 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_InitInstance(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[84 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_InitInstance")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[84 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_IsConnectOK(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[85 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_IsConnectOK")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[85 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_IsFuncCallJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[86 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_IsFuncCallJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[86 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_IsLoginJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[87 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_IsLoginJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[87 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_IsLogoutJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[88 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_IsLogoutJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[88 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_IsReloginJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[89 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_IsReloginJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[89 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_MergeJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[90 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_MergeJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[90 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_QueryClient(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[91 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_QueryClient")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[91 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_QueryTcJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[92 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_QueryTcJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[92 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_Reconnect(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[93 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_Reconnect")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[93 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_ReconnectManual(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[94 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_ReconnectManual")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[94 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_ReleaseClient(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[95 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_ReleaseClient")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[95 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_ReleaseTcJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[96 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_ReleaseTcJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[96 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_RemoveParam(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[97 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_RemoveParam")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[97 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_ResetParameterSet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[98 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_ResetParameterSet")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[98 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_ResumeTcJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[99 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_ResumeTcJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[99 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_Rewind(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[100 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_Rewind")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[100 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SeekToLine(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[101 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SeekToLine")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[101 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_Send(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[102 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_Send")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[102 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetAppStatus(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[103 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetAppStatus")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[103 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetAttach(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[104 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetAttach")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[104 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetBaseCliType(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[105 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetBaseCliType")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[105 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetBaseClientVersion(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[106 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetBaseClientVersion")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[106 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetBaseVipTermFlag(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[107 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetBaseVipTermFlag")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[107 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetCliType(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[108 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetCliType")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[108 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetClientAccount(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[109 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetClientAccount")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[109 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetClientAddress(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[110 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetClientAddress")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[110 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetClientEventHook(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[111 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetClientEventHook")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[111 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetClientLong(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[112 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetClientLong")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[112 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetClientVersion(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[113 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetClientVersion")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[113 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetExtendHook(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[114 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetExtendHook")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[114 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetFocusSite(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[115 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetFocusSite")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[115 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetGrpID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[116 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetGrpID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[116 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetJobEventHook(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[117 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetJobEventHook")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[117 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetJobLong(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[118 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetJobLong")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[118 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetJobStep(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[119 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetJobStep")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[119 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetJournalHook(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[120 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetJournalHook")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[120 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetPacketFilterHook(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[121 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetPacketFilterHook")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[121 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetPacketParam(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[122 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetPacketParam")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[122 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetPacketParamBinary(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[123 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetPacketParamBinary")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[123 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetPacketParamByte(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[124 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetPacketParamByte")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[124 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetPacketParamLong(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[125 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetPacketParamLong")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[125 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetParam(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[126 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetParam")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[126 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetParamBinary(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[127 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetParamBinary")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[127 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetParamByte(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[128 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetParamByte")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[128 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetParamFloat(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[129 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetParamFloat")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[129 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetParamLong(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[130 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetParamLong")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[130 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetParamULong(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[131 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetParamULong")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[131 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetPid(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[132 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetPid")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[132 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetProxy(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[133 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetProxy")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[133 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetQsid(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[134 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetQsid")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[134 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetReqData(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[135 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetReqData")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[135 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetSecurityInfo(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[136 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetSecurityInfo")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[136 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetServiceOption(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[137 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetServiceOption")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[137 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetTdxBranchID(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[138 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetTdxBranchID")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[138 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetTdxProxy(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[139 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetTdxProxy")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[139 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetUserErr(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[140 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetUserErr")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[140 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SetVipTermFlag(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[141 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SetVipTermFlag")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[141 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_SuspendTcJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[142 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_SuspendTcJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[142 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_TraderLogin(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[143 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_TraderLogin")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[143 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_TraderLoginAs(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[144 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_TraderLoginAs")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[144 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_TraderLogout(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[145 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_TraderLogout")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[145 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_TryReconnectAll(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[146 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_TryReconnectAll")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[146 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_UpdateCoreOption(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[147 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_UpdateCoreOption")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[147 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_UpdateTcJobParameterSet(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[148 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_UpdateTcJobParameterSet")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[148 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_TcSdk_WaitForTcJob(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[149 * TYPE long];

	// ����ԭʼ����
	GetAddress("TcSdk_WaitForTcJob")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[149 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
ALCDECL AheadLib_Version(void)
{
	// ���淵�ص�ַ
	__asm POP m_dwReturn[150 * TYPE long];

	// ����ԭʼ����
	GetAddress("Version")();

	PopDbgStrFuncEnd();
	// ת�������ص�ַ
	__asm JMP m_dwReturn[150 * TYPE long];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
