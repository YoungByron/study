
// Auto_TDXBuy.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAuto_TDXBuyApp: 
// �йش����ʵ�֣������ Auto_TDXBuy.cpp
//

class CAuto_TDXBuyApp : public CWinApp
{
public:
	CAuto_TDXBuyApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAuto_TDXBuyApp theApp;