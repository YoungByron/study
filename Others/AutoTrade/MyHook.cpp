#include "StdAfx.h"
#include "MyHook.h"

MyHook* MyHook::s_instance = NULL;
string MyHook::s_strWindowName = "";

MyHook::MyHook(void):_hwnd(0),_handle(0),_bSelf(FALSE),_state(MYHOOK_NULL)
{
}


MyHook::~MyHook(void)
{
}

MyHook& MyHook::instance()
{
	if (s_instance == NULL)
	{
		static MyHook defaultInstance;
		s_instance = &defaultInstance;
	}
	return *s_instance;
}

void MyHook::init(const HWND& hwnd, const HANDLE& handle, bool bSelf)
{
	_handle = handle;
	_hwnd = hwnd;
	_bSelf = bSelf;
	_state = MYHOOK_READY;
}

void MyHook::initByWindowName(const string& strName, bool isReal)
{
	// ���̳�ʼ��
	// strName �������ַ���
	if(isReal)
	{
		_initByWindowNameReal(strName);
	}
	else
	{
		_initByWindowNameSimilar(strName);
	}
}

void MyHook::_initByWindowNameReal(const string& strName)
{
	// ��ʵ����ʼ������
	DWORD pid;
	_hwnd = ::FindWindowA(NULL, strName.c_str());
	::GetWindowThreadProcessId(_hwnd, &pid);							// ��ý���ID
	_handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);			// ���Ȩ�޴�
	init(_hwnd, _handle);
}

void MyHook::_initByWindowNameSimilar(const string& strName)
{
	// ����������ʼ������
	s_strWindowName = strName;
	EnumWindows(lpEnumFunc, (LPARAM)this);
}

BOOL CALLBACK MyHook::lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	// EnumWindows CALLBACK
	// lParamΪ��ָ��
	char strBuff[MAX_PATH];
	MyHook* pMyHook = (MyHook*)lParam;

	GetWindowTextA(hwnd, strBuff, MAX_PATH);
	string strName = strBuff;
	if (strName.find(s_strWindowName) != string::npos)
	{
		HWND hwnd = ::FindWindowA(NULL, strName.c_str());
		DWORD pid;
		::GetWindowThreadProcessId(hwnd, &pid);							// ��ý���ID
		HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);	// ���Ȩ�޴�
		pMyHook->init(hwnd, handle);
	}
	return TRUE;
}

void MyHook::initByCreateProcess(const string& strCmd, const string& strParameter)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if(CreateProcessA(strCmd.c_str(), const_cast<char*>(strParameter.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{

		HANDLE handle = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_CREATE_THREAD|
			PROCESS_VM_WRITE|PROCESS_QUERY_INFORMATION, FALSE, pi.dwProcessId);		// �򿪽���

		init(NULL, handle);
	}
}

BOOL MyHook::readProcessMemory(LPVOID pBuff, LPVOID pAddr, int nLength)
{
	// ��ȡ�ڴ�ֵ
	// pBuff �ڴ�λ��
	// pAddr ��ַ
	// nLength ����
	if(_state == MYHOOK_NULL)
	{	
		return FALSE;
	}
	if(_bSelf)
	{
		memcpy(pBuff, pAddr, nLength);
	}
	else
	{
		::ReadProcessMemory(_handle, pAddr, pBuff, nLength, NULL);
	}
	return TRUE;
}