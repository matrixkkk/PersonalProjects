//----------------------------------------------------------
//  File: ZFlog.h 
//
//  Desc: 다이얼로그 박스를 이용해서 출력을 위한 창을 만든다
//		  만들어진 창은 변수값의 확인등으로 사용이 된다. 
//  
//  Source: 해골책 
//-------------------------------------------------------------  


#ifndef _ZFLOG_H_
#define _ZFLOG_H_

#include "main.h"
#include <stdio.h>

#define ZF_LOG_TARGET_CONSOLE			0x00000001
#define ZF_LOG_TARGET_FILE				0x00000002
#define ZF_LOG_TARGET_WINDOW			0x00000004
#define ZF_LOG_TARGET_ALL				(ZF_LOG_TARGET_CONSOLE|ZF_LOG_TARGET_FILE|ZF_LOG_TARGET_WINDOW)
#define ZF_LOG_WINDOW_TITLE				"ZEngine F-Killer"
#define ZF_LOG_WINDOW_CX				900
#define ZF_LOG_WINDOW_CY				600

class ZFLog
{
	unsigned int	m_nTarget;
	char			m_szFilename[MAX_PATH];
	HWND			m_hwnd;
	HWND			m_hwndList;
public:
			ZFLog( UINT32 nTarget, LPSTR szFilename = NULL );
			~ZFLog();
	void	CreateLogWindow();
	int		Log( LPSTR fmt, ... );

	static ZFLog* g_sLog;

private:
	static	LRESULT CALLBACK
	ZFLog::WndProcZFLog( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

//DECLARE ZFLog*		g_pLog;

#endif // _ZFLOG_H_