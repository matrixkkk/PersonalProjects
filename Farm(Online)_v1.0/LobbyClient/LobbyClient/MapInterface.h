#pragma once

#include "MapManager.h"
#include "afxwin.h"

#include <vector>

// MapInterface 폼 뷰입니다.
#define		PREVIEW_SIZE	140
#define		MAP_COUNT		11

class MapInterface : public CFormView
{
	DECLARE_DYNCREATE(MapInterface)

public:
	MapInterface();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~MapInterface();

public:

//	MapManager	m_MapManager;	// 맵 정보 Manager;
	CComboBox	m_ctlMapNameList;
	
//	CStatic		m_ctlBitMapPreview; // 픽쳐 컨트롤

	CStatic		m_ctlMapName;
	CStatic		m_ctlCntBlocks;
	CStatic		m_ctlMapInfo;

	CBitmap		m_bitMapList[ MAP_COUNT ];
	CBitmap		*m_pCurrentBitmap;


//	std::vector< CBitmap >	m_vecBitMaps;

public:

	void UpdateMapInfo	( BYTE nMapIndex );
	void SetRoomCaptain	( BOOL bCaptain );

	void GetMapInfo		( MapInfo &mapInfo );
	
public:
	enum { IDD = IDD_MAPINTERFACE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnPaint();

	
};


