#include "GameApp.h"
#include "GameState.h"
#include "ZFLog.h"

#include <NiMemTracker.h>
#include <NiStandardAllocator.h>


//#pragma warning( disible: 4819 )


CGameApp *gameApp;


//������ ���� �Լ�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{	
	//�Ҵ�� ������Ʈ ���� ���
	unsigned int uiInitialCount = NiRefObject::GetTotalObjectCount();  //���� �Ҵ�Ǿ� �ִ� ��ü���� ������ ��ȯ�մϴ�. 
	
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
	//���� �긮�� ���̺귯�� initialization
	NiInit( pkInitOptions );	

	//���ø����̼� �Ҵ�
	gameApp = NiNew CGameApp();
	//���ø����̼� ���� �����Ϳ� ���ø����̼� ��ü ����
	CGameApp::mp_Appication = gameApp;

	
	
//������� ���� log ���̾�α� �ڽ� ����
//#ifdef ZFLOG
//	ZFLog* log = new ZFLog(ZF_LOG_TARGET_WINDOW);
//	ZFLog::g_sLog = log;	
//#endif

	//���� ������ ����
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
	//�̵�� ���� ����
	gameApp->SetMediaPath("Data\\");

	//GameApp �ʱ�ȭ
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
				
		
	retCode = gameApp->BeginGame(); //GameApp ���������� ����
		
	//���Ӻ긮�� ���̺귯�� ����
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
	// ������Ʈ�� �޸� ���� �˻�
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
