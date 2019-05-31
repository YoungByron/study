
// Auto_TDXAssets.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Auto_TDXAssets.h"
#include "Auto_TDXAssetsDlg.h"
#include <vector>
#include <afxcview.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAuto_TDXAssetsApp

BEGIN_MESSAGE_MAP(CAuto_TDXAssetsApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAuto_TDXAssetsApp ����

CAuto_TDXAssetsApp::CAuto_TDXAssetsApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

HTREEITEM GetNextTreeItem(HWND hWnd, HTREEITEM hCurTreeItem)
{
	if (NULL == hWnd || NULL == hCurTreeItem)
		return NULL;

	HTREEITEM hResultItem = TreeView_GetNextItem(hWnd, hCurTreeItem, TVGN_CHILD);
	if (NULL == hResultItem)
	{
		hResultItem = TreeView_GetNextItem(hWnd, hCurTreeItem, TVGN_NEXT);
	}
	//else
	//	::OutputDebugStringA("  ");


	if (NULL == hResultItem)
	{
		HTREEITEM hParentItem = TreeView_GetNextItem(hWnd, hCurTreeItem, TVGN_PARENT);
		hResultItem = TreeView_GetNextItem(hWnd, hParentItem, TVGN_NEXT);
	}

	return hResultItem;
}



void Func()
{
	::OutputDebugStringA("\r\n================================\r\n");
	//��ȡ���ؼ�������
	HWND hParent = (HWND)0xA0D24; //SysTreeView32�����ھ��
	HWND hTree = FindWindowEx(hParent, NULL, _T("SysTreeView32"), _T(""));

	HWND hWnd = (HWND)0xA0D24; //��������
	DWORD PID;
	GetWindowThreadProcessId(hWnd, &PID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	//hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, PID);
	ASSERT(NULL != hProcess);

	UINT vItemCount = TreeView_GetCount(hTree);
	HTREEITEM hTreeItem = TreeView_GetRoot(hTree);
	TCHAR buf[256];

	std::vector<CString> vecItemText;
	CTreeView* pTreeView = (CTreeView*)CWnd::FromHandle(hTree);
	CTreeCtrl& treeCtrl = pTreeView->GetTreeCtrl();
	for (int i = 0; i < vItemCount; i++)
	{
		LPTV_ITEM pTreeItem = (LPTV_ITEM)VirtualAllocEx(hProcess, NULL, sizeof(TV_ITEM), MEM_COMMIT, PAGE_READWRITE);
		TCHAR* pItemText = (TCHAR*)VirtualAllocEx(hProcess, NULL, 256, MEM_COMMIT, PAGE_READWRITE);

		TV_ITEM item;
		item.mask = TVIF_TEXT;
		item.hItem = hTreeItem;
		item.pszText = pItemText;
		item.cchTextMax = 256;

		WriteProcessMemory(hProcess, pTreeItem, &item, sizeof(TV_ITEM), NULL);
		treeCtrl.GetItem(pTreeItem);
		ReadProcessMemory(hProcess, pItemText, buf, 256, NULL);

		VirtualFreeEx(hProcess, pTreeItem, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pItemText, 0, MEM_RELEASE);

		//if (TreeView_GetRoot(hTree) != hTreeItem)
		{
			CString sTreeItemText(buf);
			vecItemText.push_back(sTreeItemText);

			if (sTreeItemText == "�ʽ�ɷ�")
			{
				TreeView_SelectItem(hTree, hTreeItem);

				HWND hTestWnd = (HWND)0x00120A66;
				////WM_PARENTNOTIFY fwEvent : WM_LBUTTONDOWN xPos : 59 yPos : 57[wParam:00000201 lParam : 0039003B]
				//::SendMessage((HWND)hTestWnd, WM_PARENTNOTIFY, 0x00000201, 0x04560048);

				////WM_MOUSEACTIVATE hwndTopLevel:00120A66 nHittest:HTCLIENT uMsg:WM_LBUTTONDOWN [wParam:00120A66 lParam:02010001]
				//::SendMessage((HWND)hTestWnd, WM_MOUSEACTIVATE, 0x00120A66, 0x02010001);
				//::SendMessage((HWND)hTestWnd, WM_WINDOWPOSCHANGING, 0, 0x0018FA40);
				//::SendMessage((HWND)hTestWnd, WM_WINDOWPOSCHANGED, 0, 0x0018FA34);
				//::SendMessage((HWND)hTestWnd, WM_ACTIVATEAPP, 1, 0);
				//::SendMessage((HWND)hTestWnd, WM_NCACTIVATE, 1, 0);
				//::SendMessage((HWND)hTestWnd, WM_IME_NOTIFY, 2, 0);
				//::SendMessage((HWND)hTestWnd, WM_IME_SETCONTEXT, 1, 0xC000000F);

				////WM_SETCURSOR hwnd:00110B0A nHittest:HTCLIENT wMouseMsg:WM_LBUTTONDOWN [wParam:00110B0A lParam:02010001]
				//::SendMessage((HWND)hTestWnd, WM_SETCURSOR, 0x00110B0A, 0x02010001);

				////message:0x17EF [User-defined:WM_USER+5103] wParam:00000000 lParam:00000000
				//::SendMessage((HWND)hTestWnd, WM_USER + 5103, 0, 0);
				////message : 0x17ED[User - defined:WM_USER + 5101] wParam : 00000000 lParam : 00000000
				//::SendMessage((HWND)hTestWnd, WM_USER + 5101, 0, 0);
				////message : 0x17F0[User - defined:WM_USER + 5104] wParam : 00000000 lParam : 000000
				//::SendMessage((HWND)hTestWnd, WM_USER + 5104, 0, 0);

				{
					//����
					//HTREEITEM hItem = TreeView_GetRoot(hTree);
					//hItem = TreeView_GetNextItem(hTree, hItem, TVGN_CHILD);
					//::SendMessage(hTree, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);
					//TreeView_EnsureVisible(hTree, hItem);
					//DWORD pid = 0;
					//GetWindowThreadProcessId(hTree, &pid);
					//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, pid);

					RECT *prect = (RECT*)VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);
					RECT rect = { 0 };
					unsigned long n = 0;

					*(HTREEITEM*)&rect = hTreeItem;
					WriteProcessMemory(hProcess, prect, &rect, sizeof(RECT), NULL);
					::SendMessage(hTree, TVM_GETITEMRECT, (WPARAM)FALSE, (LPARAM)prect);
					ReadProcessMemory(hProcess, prect, &rect, sizeof(RECT), &n);

					POINT pt = { 0 };
					pt.x = rect.left + (rect.right - rect.left) / 2;
					pt.y = rect.top + (rect.bottom - rect.top) / 2;

					//::SendMessage(hTree, WM_LBUTTONDBLCLK, 0, MAKELPARAM((WORD)pt.x, (WORD)pt.y));

					//�ͷ��ڴ�
					CloseHandle(hProcess);

					if (prect)
					{
						VirtualFreeEx(hProcess, prect, 0, MEM_RELEASE);
					}

					//ת��������
					::ClientToScreen(hTree, &pt);

					//����¼�
					SetCursorPos(pt.x - 8, pt.y);

					POINT pt2 = { 0 };
					while ((pt2.x != pt.x - 8) || (pt2.y != pt.y))
					{
						Sleep(10);
						GetCursorPos(&pt2);
					}

// 					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
// 					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

// 					::SendMessage(hTestWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(72, 1110));
// 					::SendMessage(hTestWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(72, 1110));
					Sleep(10);
				}
			}
		}
		hTreeItem = GetNextTreeItem(hTree, hTreeItem);
	}

	for (auto it = vecItemText.begin(); it != vecItemText.end(); ++it)
	{
		//�ɸ���Item�����ж�ת����pdf�Ƿ����
		::OutputDebugString(*it);
		::OutputDebugStringA("\r\n");
	}
}

using namespace std;
BOOL bNextOK = FALSE;
BOOL bDlgOK = FALSE;
double dbAvailableFunds = 0.0;
double dbStockValue = 0.0;
double dbOrderValue = 0.0;

void LoopReadList(HWND hwnd)
{
	if (!hwnd)
		return;

	int count, i;
	char item[512] = { 0 }, subitem[512] = { 0 };

	LVITEMA lvi, *_lvi;
	char *_item, *_subitem;
	DWORD pid;
	HANDLE process;

	count = (int)SendMessage(hwnd, LVM_GETITEMCOUNT, 0, 0);

	GetWindowThreadProcessId(hwnd, &pid);
	process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ |
		PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, pid);

	_lvi = (LVITEMA*)VirtualAllocEx(process, NULL, sizeof(LVITEMA),
		MEM_COMMIT, PAGE_READWRITE);
	_item = (char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,
		PAGE_READWRITE);
	_subitem = (char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,
		PAGE_READWRITE);

	lvi.cchTextMax = 512;

	::OutputDebugStringA("\r\n==========================\r\n");
	dbStockValue = 0.0;
	for (i = 0; i < count; i++) {
		lvi.iSubItem = 6;
		lvi.pszText = _item;
		WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEMA), NULL);
		SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

		lvi.iSubItem = 7;
		lvi.pszText = _subitem;
		WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEMA), NULL);
		SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

		ReadProcessMemory(process, _item, item, 512, NULL);
		ReadProcessMemory(process, _subitem, subitem, 512, NULL);

		dbStockValue += atof(item);
		TRACE("%s - %s\n", item, subitem);
	}

	VirtualFreeEx(process, _lvi, 0, MEM_RELEASE);
	VirtualFreeEx(process, _item, 0, MEM_RELEASE);
	VirtualFreeEx(process, _subitem, 0, MEM_RELEASE);


	return;
}

void LoopReadList2(HWND hwnd)
{
	if (!hwnd)
		return;

	int count, i;
	char item[512] = { 0 }, subitem[512] = { 0 }, subitem2[512] = { 0 };

	LVITEMA lvi, *_lvi;
	char *_item, *_subitem, *_subitem2;
	DWORD pid;
	HANDLE process;

	count = (int)SendMessage(hwnd, LVM_GETITEMCOUNT, 0, 0);

	GetWindowThreadProcessId(hwnd, &pid);
	process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ |
		PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, pid);

	_lvi = (LVITEMA*)VirtualAllocEx(process, NULL, sizeof(LVITEMA),
		MEM_COMMIT, PAGE_READWRITE);
	_item = (char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,
		PAGE_READWRITE);
	_subitem = (char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,
		PAGE_READWRITE);
	_subitem2 = (char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,
		PAGE_READWRITE);

	lvi.cchTextMax = 512;

	::OutputDebugStringA("\r\n==========================\r\n");
	dbOrderValue = 0.0;
	for (i = 0; i < count; i++) {
		lvi.iSubItem = 3;
		lvi.pszText = _item;
		WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEMA), NULL);
		SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

		lvi.iSubItem = 5;
		lvi.pszText = _subitem;
		WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEMA), NULL);
		SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

		lvi.iSubItem = 6;
		lvi.pszText = _subitem2;
		WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEMA), NULL);
		SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

		ReadProcessMemory(process, _item, item, 512, NULL);
		ReadProcessMemory(process, _subitem, subitem, 512, NULL);
		ReadProcessMemory(process, _subitem2, subitem2, 512, NULL);

		if (strcmp(item, "����") == 0)
		{
			dbOrderValue += atof(subitem) * atof(subitem2)*1.00025;
			TRACE("%s - %s - %s %.2f\n", item, subitem, subitem2, dbOrderValue + dbAvailableFunds + dbStockValue);
		}
	}

	VirtualFreeEx(process, _lvi, 0, MEM_RELEASE);
	VirtualFreeEx(process, _item, 0, MEM_RELEASE);
	VirtualFreeEx(process, _subitem, 0, MEM_RELEASE);


	return;
}

int iTmp = 0;
BOOL CALLBACK EnumChildProc(_In_ HWND   hwnd, _In_ LPARAM lParam)
{
	iTmp++;
	char szTitle[MAX_PATH] = { 0 };
	char szClass[MAX_PATH] = { 0 };
	int nMaxCount = MAX_PATH;

	LPSTR lpClassName = szClass;
	LPSTR lpWindowTxt = szTitle;

	GetWindowTextA(hwnd, lpWindowTxt, nMaxCount);
	GetClassNameA(hwnd, lpClassName, nMaxCount);
	//cout << "[Child window] window handle: " << hwnd << " window name: "
	//	<< lpWindowName << " class name " << lpClassName << endl;

	if (bNextOK)
	{
		bNextOK = FALSE;

		while (strlen(lpWindowTxt) ==0)
		{
			Sleep(10);
			GetWindowTextA(hwnd, lpWindowTxt, nMaxCount);
		}
		dbAvailableFunds = atof(lpWindowTxt);
		::OutputDebugStringA("\r\n");
		::OutputDebugStringA(lpWindowTxt);
		::OutputDebugStringA("->");
		::OutputDebugStringA(lpClassName);
		::OutputDebugStringA("  ");
	}
	if (strstr(lpWindowTxt, "����"))
	{
		bNextOK = TRUE;
	}
	else if ((strcmp(lpWindowTxt, "List1")==0) && (strcmp(lpClassName, "SysListView32") == 0))
	{
		int iItem = ListView_GetItemCount(hwnd);
		LoopReadList(hwnd);
		iTmp++;

		return FALSE; //�Ӵ����Һ��˾���ֹEnumChildWindows
	}
	return TRUE;
}

BOOL CALLBACK EnumChildProc2(_In_ HWND   hwnd, _In_ LPARAM lParam)
{
	char szTitle[MAX_PATH] = { 0 };
	char szClass[MAX_PATH] = { 0 };
	int nMaxCount = MAX_PATH;

	LPSTR lpClassName = szClass;
	LPSTR lpWindowTxt = szTitle;

	GetWindowTextA(hwnd, lpWindowTxt, nMaxCount);
	GetClassNameA(hwnd, lpClassName, nMaxCount);
	//cout << "[Child window] window handle: " << hwnd << " window name: "
	//	<< lpWindowName << " class name " << lpClassName << endl;

	//if (bDlgOK)
	//{
	//	dbAvailableFunds = atof(lpWindowTxt);
	//	::OutputDebugStringA("\r\n");
	//	::OutputDebugStringA(lpWindowTxt);
	//	::OutputDebugStringA("->");
	//	::OutputDebugStringA(lpClassName);
	//	::OutputDebugStringA("  ");
	//}
	if ((strcmp(lpWindowTxt, "�� ��") == 0) && ::IsWindowVisible(::GetParent(hwnd)))
	{
		bDlgOK = TRUE;
	}
	else if (bDlgOK && (strcmp(lpWindowTxt, "List1")==0) && (strcmp(lpClassName, "SysListView32") == 0))
	{
		bDlgOK = FALSE;
		int iItem = ListView_GetItemCount(hwnd);
		if (iItem > 0)
		{
			LoopReadList2(hwnd);
		}

		return FALSE; //�Ӵ����Һ��˾���ֹEnumChildWindows
	}
	return TRUE;
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{

	/*
	* Remarks
	The EnumWindows function does not enumerate child windows,
	with the exception of a few top-level windows owned by the
	system that have the WS_CHILD style.
	*/
	char szTitle[MAX_PATH] = { 0 };
	char szClass[MAX_PATH] = { 0 };
	int nMaxCount = MAX_PATH;

	LPSTR lpClassName = szClass;
	LPSTR lpWindowName = szTitle;

	GetWindowTextA(hwnd, lpWindowName, nMaxCount);
	GetClassNameA(hwnd, lpClassName, nMaxCount);
	//cout << "[Parent window] window handle: " << hwnd << " window name: "
	//	<< lpWindowName << " class name " << lpClassName << endl;

	EnumChildWindows(hwnd, EnumChildProc, lParam);

	return TRUE;
}

// Ψһ��һ�� CAuto_TDXAssetsApp ����

CAuto_TDXAssetsApp theApp;


// CAuto_TDXAssetsApp ��ʼ��
#define MAIN_WND_TITLE "����֤ȯV6.36 - [���ͼ-��ҵ��]"
BOOL CAuto_TDXAssetsApp::InitInstance()
{
	CWinApp::InitInstance();


	//Func();

	HWND tdxWnd = ::FindWindowA("TdxW_MainFrame_Class", MAIN_WND_TITLE);
	//�ȱ���ȷ����ͼ�ǹ�Ʊ�Ĵ���
	while (NULL == tdxWnd)
	{
		::MessageBoxA(NULL, "�Ҳ�������Ϊ��'"
			MAIN_WND_TITLE
			"' �Ĵ��ڣ�", "�Ѻ���ʾ", MB_ICONEXCLAMATION);
		tdxWnd = ::FindWindowA("TdxW_MainFrame_Class", MAIN_WND_TITLE);
	}

	//ShowWindow(tdxWnd, SW_HIDE);
	//ShowWindow(tdxWnd, SW_SHOWNORMAL);
	::SendMessage((HWND)tdxWnd, WM_COMMAND, MAKEWPARAM(5302, 0), NULL);
	Sleep(2000);
	DWORD start_time = GetTickCount();
	EnumChildWindows(tdxWnd, EnumChildProc, 0);
	TRACE("��ʱA��%.3f��", GetTickCount()/1000.0 - start_time / 1000.0);

	::SendMessage((HWND)tdxWnd, WM_COMMAND, MAKEWPARAM(5333, 0), NULL);
	Sleep(1000);
	start_time = GetTickCount();
	EnumChildWindows(tdxWnd, EnumChildProc2, 0);
	TRACE("��ʱB��%.3f��", GetTickCount() / 1000.0 - start_time / 1000.0);

	//HWND subWnd = ::FindWindowA("Static", "�����ʽ�:");

	//HWND hd = GetDesktopWindow();        //�õ����洰��
	//hd = GetWindow(hd, GW_CHILD);        //�õ���Ļ�ϵ�һ���Ӵ���
	//char s[200] = { 0 };
	//int num = 1;
	//while (hd != NULL)                    //ѭ���õ����е��Ӵ���
	//{
	//	memset(s, 0, 200);
	//	GetWindowTextA(hd, s, 200);
	//	if (strcmp(s, "�����ʽ�:") == 0)
	//	{
	//		subWnd = hd;
	//		break;
	//	}
	//	hd = GetNextWindow(hd, GW_HWNDNEXT);
	//}
	//::SendMessage((HWND)subWnd, WM_COMMAND, MAKEWPARAM(5302, 0), NULL);
	return FALSE;


	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CAuto_TDXAssetsDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
	}


#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

