
// Auto_TDXAssets.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAuto_TDXAssetsApp: 
// �йش����ʵ�֣������ Auto_TDXAssets.cpp
//

class CAuto_TDXAssetsApp : public CWinApp
{
public:
	CAuto_TDXAssetsApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAuto_TDXAssetsApp theApp;