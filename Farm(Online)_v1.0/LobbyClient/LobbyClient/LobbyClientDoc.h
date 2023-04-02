// LobbyClientDoc.h : CLobbyClientDoc 클래스의 인터페이스
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
protected: // serialization에서만 만들어집니다.
	CLobbyClientDoc();
	DECLARE_DYNCREATE(CLobbyClientDoc)

// 특성입니다.
public:

	DlgLogin		*m_pDlgLogin;			// 로그인
	DlgMember		*m_pDlgMember;			// 회원 가입
	DlgCreateRoom	*m_pDlgCreateRoom;		// 방 만들기
	DlgInvitation	*m_pDlgInvitation;		// 초대하기
	DlgJoinerInfo	*m_pDlgJoinerInfo;		// 유저 정보

	NetLobbyClient	m_MyLobbyClient;		// 네트워크 시스템
	MapManager		m_MapManager;			// BMP 리소스 관리

	CWinThread		*m_pThread;

// 작업입니다.
public:	

	void OnCreateRoom();


// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현입니다.
public:
	virtual ~CLobbyClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLogin();
	afx_msg void OnMemberShip();
public:
	virtual void OnCloseDocument();
};


