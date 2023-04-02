#pragma once
#include <afxcview.h>

// UserListView 뷰입니다.

class UserListView : public CListView
{
	DECLARE_DYNCREATE(UserListView)

protected:
	UserListView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~UserListView();

public:

	// 리스트 컬럼 메뉴 셋팅
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


