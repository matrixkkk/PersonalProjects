// LobbyClientView.cpp : CLobbyClientView 클래스의 구현
//

#include "stdafx.h"
#include "LobbyClient.h"

#include "LobbyClientDoc.h"
#include "LobbyClientView.h"
UINT WorkerThread( LPVOID arg )
//-----------------------------------------
{
	NetSystem *pNetSystem = reinterpret_cast< NetSystem* >( arg );
	
	if( NULL != pNetSystem )
		pNetSystem->BeginWorKerThread();

	return 0;
}



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLobbyClientView

IMPLEMENT_DYNCREATE(CLobbyClientView, CView)

BEGIN_MESSAGE_MAP(CLobbyClientView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// CLobbyClientView 생성/소멸

CLobbyClientView::CLobbyClientView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CLobbyClientView::~CLobbyClientView()
{
}

BOOL CLobbyClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CLobbyClientView 그리기

void CLobbyClientView::OnDraw(CDC* /*pDC*/)
{
	CLobbyClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CLobbyClientView 인쇄

BOOL CLobbyClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CLobbyClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CLobbyClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CLobbyClientView 진단

#ifdef _DEBUG
void CLobbyClientView::AssertValid() const
{
	CView::AssertValid();
}

void CLobbyClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLobbyClientDoc* CLobbyClientView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLobbyClientDoc)));
	return (CLobbyClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CLobbyClientView 메시지 처리기

int CLobbyClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CLobbyClientView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	ReleaseCapture();
}

void CLobbyClientView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CLobbyClientDoc* pDoc = GetDocument();

	//	로그인 Dialog 생성
	if( NULL == pDoc->m_pDlgLogin )
	{
		pDoc->m_pDlgLogin = new DlgLogin;
		pDoc->m_pDlgLogin->Create( IDD_LOGIN_AND_REGISTERMEMBER );
	}

	//	회원가입 Dialog 생성
	if( NULL == pDoc->m_pDlgMember )
	{
		pDoc->m_pDlgMember = new DlgMember;
		pDoc->m_pDlgMember->Create( IDD_MEMBERSHIP );
	}

	if( NULL == pDoc->m_pDlgCreateRoom )
	{
		pDoc->m_pDlgCreateRoom = new DlgCreateRoom;
		pDoc->m_pDlgCreateRoom->Create( IDD_CREATE_ROOM );
	}

	if( NULL == pDoc->m_pDlgInvitation )
	{
		pDoc->m_pDlgInvitation = new DlgInvitation;
		pDoc->m_pDlgInvitation->Create( IDD_INVITATION );
	}

	if( NULL == pDoc->m_pDlgJoinerInfo )
	{
		pDoc->m_pDlgJoinerInfo = new DlgJoinerInfo;
		pDoc->m_pDlgJoinerInfo->Create( IDD_JOINER_INFO );
	}
	

	pDoc->m_MyLobbyClient.Begin();	

	pDoc->m_pThread = AfxBeginThread( WorkerThread, reinterpret_cast< LPVOID >( &pDoc->m_MyLobbyClient ), 0, 0, CREATE_SUSPENDED );

	if( NULL == pDoc->m_pThread )
	{
//		MessageBox( NULL, L"NetSystem::CreateWorkerThread() - CreateThread() Failed", L"Failed", MB_OK );
		return;
	}

	pDoc->m_pThread->m_bAutoDelete = FALSE;

	pDoc->m_pThread->ResumeThread();

}
