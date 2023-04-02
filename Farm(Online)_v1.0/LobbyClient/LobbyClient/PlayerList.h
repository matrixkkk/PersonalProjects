#pragma once


// PlayerList 뷰입니다.

class PlayerList : public CListView
{
	DECLARE_DYNCREATE(PlayerList)

public:
	PlayerList();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~PlayerList();

public:

	void SetColumnMenu();

public:
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
public:
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


