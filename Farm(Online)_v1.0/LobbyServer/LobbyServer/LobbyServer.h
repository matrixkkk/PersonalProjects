// LobbyServer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CLobbyServerApp:
// �� Ŭ������ ������ ���ؼ��� LobbyServer.cpp�� �����Ͻʽÿ�.
//

class CLobbyServerApp : public CWinApp
{
public:
	CLobbyServerApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CLobbyServerApp theApp;