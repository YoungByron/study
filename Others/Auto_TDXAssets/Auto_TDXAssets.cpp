
// Auto_TDXAssets.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Auto_TDXAssets.h"
#include "Auto_TDXAssetsDlg.h"

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
	HWND hParent = (HWND)0x512394; //SysTreeView32�����ھ��
	HWND hTree = FindWindowEx(hParent, NULL, _T("SysTreeView32"), _T(""));

	HWND hWnd = (HWND)0x512394; //��������
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

					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
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


// Ψһ��һ�� CAuto_TDXAssetsApp ����

CAuto_TDXAssetsApp theApp;


// CAuto_TDXAssetsApp ��ʼ��

BOOL CAuto_TDXAssetsApp::InitInstance()
{
	CWinApp::InitInstance();


	Func();
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

