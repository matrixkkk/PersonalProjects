#pragma once
#include <afxcview.h>

// UserListView ���Դϴ�.

class UserListView : public CListView
{
	DECLARE_DYNCREATE(UserListView)

protected:
	UserListView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~UserListView();

public:

	// ����Ʈ �÷� �޴� ����
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
};


