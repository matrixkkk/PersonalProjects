#pragma once


// JoinerList 뷰입니다.

class JoinerList : public CListView
{
	DECLARE_DYNCREATE(JoinerList)

public:

	int			m_nSelectedItem;
	COLORREF	m_rgbLightGray;
	COLORREF	m_rgbWhite;



public:
	JoinerList();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~JoinerList();


public:
	void SetColumnMenu();


#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRoomMenuControlForCaptain( UINT nID );
	afx_msg void OnRoomMenuControlForUser( UINT nID );
};


