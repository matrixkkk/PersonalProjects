#pragma once


// PlayerList ���Դϴ�.

class PlayerList : public CListView
{
	DECLARE_DYNCREATE(PlayerList)

public:
	PlayerList();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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


