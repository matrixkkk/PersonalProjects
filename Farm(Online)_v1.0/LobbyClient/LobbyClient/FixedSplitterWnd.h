#pragma once


// CFixedSplitterWnd

class CFixedSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC(CFixedSplitterWnd)

public:
	CFixedSplitterWnd();
	virtual ~CFixedSplitterWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


