
// HttpProxyServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHttpProxyServerApp:
// �йش����ʵ�֣������ HttpProxyServer.cpp
//

class CHttpProxyServerApp : public CWinAppEx
{
public:
	CHttpProxyServerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHttpProxyServerApp theApp;