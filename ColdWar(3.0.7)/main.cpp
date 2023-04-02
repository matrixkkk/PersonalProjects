#include "GameApp.h"
#include "GameState.h"
#include "ZFLog.h"

#include <NiMemTracker.h>
#include <NiStandardAllocator.h>


//#pragma warning( disible: 4819 )


CGameApp *gameApp;


//윈도우 메인 함수
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{	
	//할당된 오브젝트 개수 출력
	unsigned int uiInitialCount = NiRefObject::GetTotalObjectCount();  //현재 할당되어 있는 객체들의 개수를 반환합니다. 
	
	char str[256];
	NiSprintf(str,256,"uilnitialCount = %d",uiInitialCount);
	OutputDebugString(str);
    
	int retCode = 0;

	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
	NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator())        
#else
	NiExternalNew NiStandardAllocator()        
#endif
        );
	//게임 브리오 라이브러리 initialization
	NiInit( pkInitOptions );	

	//어플리케이션 할당
	gameApp = NiNew CGameApp();
	//어플리케이션 정적 포인터에 어플리케이션 객체 전달
	CGameApp::mp_Appication = gameApp;

	
	
//디버깅을 위한 log 다이얼로그 박스 생성
//#ifdef ZFLOG
//	ZFLog* log = new ZFLog(ZF_LOG_TARGET_WINDOW);
//	ZFLog::g_sLog = log;	
//#endif

	//메인 윈도우 생성
	if(!gameApp->CreateMainWindow("Cold War",hInstance,iCmdShow,true))
	{
		NiMessageBox("Window Create Failed",NULL);
		const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();    
		NiShutdown();
		NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
		NiExternalDelete pkInitOptions;
		NiExternalDelete pkAllocator;
		return -1;
	}
	//미디어 파일 지정
	gameApp->SetMediaPath("Data\\");

	//GameApp 초기화
	if(!gameApp->Initialize())
	{
		NiMessageBox("Application initialize Failed!",NULL);
		const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();    
		NiShutdown();
		NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
		NiExternalDelete pkInitOptions;
		NiExternalDelete pkAllocator;
		return -1;
	}
				
		
	retCode = gameApp->BeginGame(); //GameApp 본격적으로 시작
		
	//게임브리오 라이브러리 종료
	gameApp->ShutDown();
	NiDelete gameApp;

	const NiInitOptions* pInitOptions = NiStaticDataManager::GetInitOptions();    
	NiShutdown();
	NiAllocator* pkAllocator = pInitOptions->GetAllocator();
	NiExternalDelete pInitOptions;
	NiExternalDelete pkAllocator;
	
	
			
//#ifdef ZFLOG
//	delete log;
//	log = NULL;
//#endif
/*
	// 오브젝트의 메모리 추출 검사
    unsigned int uiFinalCount = NiRefObject::GetTotalObjectCount();
    char acMsg[256];
    NiSprintf(acMsg, 256,
        "\nGamebryo NiRefObject counts:  initial = %u, final = %u. ", 
        uiInitialCount, uiFinalCount);
    OutputDebugString(acMsg);
    if (uiFinalCount > uiInitialCount)
    {
        unsigned int uiDiff = uiFinalCount - uiInitialCount;
        NiSprintf(acMsg, 256, "Application is leaking %u objects\n\n", uiDiff);
        OutputDebugString(acMsg);
    }
    else
    {
        OutputDebugString("Application has no object leaks.\n\n");
    }
*/

	return retCode;
}
