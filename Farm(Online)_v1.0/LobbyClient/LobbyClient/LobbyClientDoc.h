// LobbyClientDoc.h : CLobbyClientDoc Ŭ������ �������̽�
//


#pragma once

#include "NetLobbyClient.h"
#include "DlgLogin.h"
#include "DlgMember.h"
#include "DlgCreateRoom.h"
#include "DlgInvitation.h"
#include "DlgJoinerInfo.h"



class CLobbyClientDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CLobbyClientDoc();
	DECLARE_DYNCREATE(CLobbyClientDoc)

// Ư���Դϴ�.
public:

	DlgLogin		*m_pDlgLogin;			// �α���
	DlgMember		*m_pDlgMember;			// ȸ�� ����
	DlgCreateRoom	*m_pDlgCreateRoom;		// �� �����
	DlgInvitation	*m_pDlgInvitation;		// �ʴ��ϱ�
	DlgJoinerInfo	*m_pDlgJoinerInfo;		// ���� ����

	NetLobbyClient	m_MyLobbyClient;		// ��Ʈ��ũ �ý���
	MapManager		m_MapManager;			// BMP ���ҽ� ����

	CWinThread		*m_pThread;

// �۾��Դϴ�.
public:	

	void OnCreateRoom();


// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// �����Դϴ�.
public:
	virtual ~CLobbyClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLogin();
	afx_msg void OnMemberShip();
public:
	virtual void OnCloseDocument();
};


