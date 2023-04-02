// LobbyClientView.cpp : CLobbyClientView Ŭ������ ����
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
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// CLobbyClientView ����/�Ҹ�

CLobbyClientView::CLobbyClientView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CLobbyClientView::~CLobbyClientView()
{
}

BOOL CLobbyClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CLobbyClientView �׸���

void CLobbyClientView::OnDraw(CDC* /*pDC*/)
{
	CLobbyClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CLobbyClientView �μ�

BOOL CLobbyClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CLobbyClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CLobbyClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CLobbyClientView ����

#ifdef _DEBUG
void CLobbyClientView::AssertValid() const
{
	CView::AssertValid();
}

void CLobbyClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLobbyClientDoc* CLobbyClientView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLobbyClientDoc)));
	return (CLobbyClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CLobbyClientView �޽��� ó����

int CLobbyClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}

void CLobbyClientView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	ReleaseCapture();
}

void CLobbyClientView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CLobbyClientDoc* pDoc = GetDocument();

	//	�α��� Dialog ����
	if( NULL == pDoc->m_pDlgLogin )
	{
		pDoc->m_pDlgLogin = new DlgLogin;
		pDoc->m_pDlgLogin->Create( IDD_LOGIN_AND_REGISTERMEMBER );
	}

	//	ȸ������ Dialog ����
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
