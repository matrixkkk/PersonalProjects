#pragma once
#include <afxcview.h>

// RoomListView ���Դϴ�.

class RoomListView : public CListView
{
	DECLARE_DYNCREATE(RoomListView)

protected:
	RoomListView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~RoomListView();

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


