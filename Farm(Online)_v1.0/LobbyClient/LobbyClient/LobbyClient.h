// LobbyClient.h : LobbyClient ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CLobbyClientApp:
// �� Ŭ������ ������ ���ؼ��� LobbyClient.cpp�� �����Ͻʽÿ�.
//

class CLobbyClientApp : public CWinApp
{
public:
	CLobbyClientApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLobbyClientApp theApp;