#pragma once
#include <afxcview.h>


// CChatView �� ���Դϴ�.

class CChatView : public CFormView
{
	DECLARE_DYNCREATE(CChatView)

protected:
	CChatView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CChatView();

public:
	enum { IDD = IDD_CHATVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
};


