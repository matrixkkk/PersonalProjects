#pragma once



// MenuInterface �� ���Դϴ�.

class MenuInterface : public CFormView
{
	DECLARE_DYNCREATE(MenuInterface)

public:

	CFont	m_Font;

protected:
	MenuInterface();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~MenuInterface();

public:
	enum { IDD = IDD_MENUINTERFACE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnInitialUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};


