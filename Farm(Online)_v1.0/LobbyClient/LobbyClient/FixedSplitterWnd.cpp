// FixedSplitterWnd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "FixedSplitterWnd.h"


// CFixedSplitterWnd

IMPLEMENT_DYNAMIC(CFixedSplitterWnd, CSplitterWnd)

CFixedSplitterWnd::CFixedSplitterWnd()
{
	m_cxSplitter = m_cySplitter = 0 ; 
    m_cxBorderShare = m_cyBorderShare = 0; 
	m_cxSplitterGap = m_cySplitterGap = 1; 
    m_cxBorder = m_cyBorder = 1;
}

CFixedSplitterWnd::~CFixedSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CFixedSplitterWnd, CSplitterWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CFixedSplitterWnd �޽��� ó�����Դϴ�.



void CFixedSplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

//	CSplitterWnd::OnLButtonDown(nFlags, point);
}

void CFixedSplitterWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

//	CSplitterWnd::OnLButtonUp(nFlags, point);
}

void CFixedSplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

//	CSplitterWnd::OnMouseMove(nFlags, point);
}
