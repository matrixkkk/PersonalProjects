#include "ObjectPick.h"
#include "GameApp.h"
#include "ObjectPickBinaryShader.h"


#include <NSBShaderLib.h>
#include <NSBShader.h>
#include <NSFParserLib.h>
/*
#pragma comment(lib,"NiBinaryShaderLibDX9.lib")
#pragma comment(lib,"NSBShaderLibDX9.lib")
#pragma comment(lib,"NSFParserLibDX9.lib")
*/
//#pragma comment(lib, "NSBShaderLib.nl9" )

CObjectPick::CObjectPick()
{
	//~생성자 부분
	m_pkColorPicker = NULL;
	m_pkPickedObject = NULL;
}
//소멸자
CObjectPick::~CObjectPick()
{
	Release();
}

bool CObjectPick::Init()
{
	//쉐이더 시스템 초기화
	if(!SetupShaderSystem())
	{
		return false;
	}

	//칼라피커 생성
	if(!CreatePicker())
	{
		NiMessageBox("CObjectPick::Init() - CreatePicker() Failed!",NULL);
		return false;
	}

	return true;
}

bool CObjectPick::CheckPick(NiCursor *pkCursor,NiNode* pkSceneRoot,NiCamera* pkCamera)
{
	//마우스 x,y 좌표 선언
	int iX = 0;
	int iY = 0;
	

	//------픽킹 수행----------
	
	//커서의 현재 x,y 위치를 가져온다.
	pkCursor->GetScreenSpacePosition(iX,iY);
		
	//실제로 피킹하는 부분입니다. 픽킹 광선을 만들기위해 카메라 클래스를 
	//넘겨주어야 합니다.
	if(m_pkColorPicker)
	{			
		m_pkColorPicker->StartPick(pkCamera,iX,iY);			//피킹 시작
		m_pkColorPicker->RecursivePickRender(pkSceneRoot);
		m_pkPickedObject = m_pkColorPicker->EndPick();		//피킹 완료

//		char buffer[512];
		//피킹 결과
		if(m_pkPickedObject)		
			return true;
		else
			return false;	
	}
	return true;
}
	
void CObjectPick::Release()
{
	NiDelete m_pkColorPicker;	
}

//칼라피커 생성
bool CObjectPick::CreatePicker()
{
	NiRenderer* pkRenderer = CGameApp::mp_Appication->GetRenderer();
	const NiRenderTargetGroup* pkRTGroup = pkRenderer->GetDefaultRenderTargetGroup();
    m_pkColorPicker = NiNew ColorPicker(pkRenderer, 
        pkRTGroup->GetWidth(0), pkRTGroup->GetHeight(0));
    if (m_pkColorPicker)
        return true;

	return false;
}

//피킹된 오브젝트 리턴
const NiAVObject* CObjectPick::GetPickedObject()
{
	return m_pkPickedObject;
}

bool CObjectPick::SetupShaderSystem()
{
	NiShaderFactory::RegisterErrorCallback(ShaderErrorCallback);

    NSBShader::SetCreateNiBinaryShaderCallback( CObjectPick::CreateNiBinaryShaderCallback );

    // First, we will run the NSF parser. This is done to make sure that
    // any text-based shader files that have been modified are re-compiled
    // to binary before loading all the binary representations.
    if (!RunShaderParsers())
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
        return false;
    }

    if (!RegisterShaderLibraries())
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
        return false;
    }
	
    return true;
}

bool CObjectPick::RunShaderParsers()
{
	NiShaderFactory::RegisterRunParserCallback(RunParser);

	unsigned int uiCount = NiShaderFactory::LoadAndRunParserLibrary(NULL,"Data",true);

	return true;
}

bool CObjectPick::RegisterShaderLibraries()
{
    NiShaderFactory::RegisterClassCreationCallback(LibraryClassCreate);

    NiShaderFactory::AddShaderProgramFileDirectory("./Data/pick/");

    unsigned int uiCount = 1;
    char* apcDirectories[1];
    apcDirectories[0] = "./Data/";

    if (!NiShaderFactory::LoadAndRegisterShaderLibrary("NSBShaderLib.dl9", 
        uiCount, apcDirectories, true))
    {
        NiMessageBox("Failed to load shader library!", "ERROR");
        return false;
    }
    return true;
}

bool CObjectPick::LibraryClassCreate(const char* pcLibFile, 
    NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

    // Create the NSB Shader Library.
//    return NSBShaderLib_LoadShaderLibrary(pkRenderer, iDirectoryCount, 
//        apcDirectories, bRecurseSubFolders, ppkLibrary);

	return true;
}

unsigned int CObjectPick::RunParser(const char* pcLibFile, 
    NiRenderer* pkRenderer, const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    // Run the NSF Parser.
//    return NSFParserLib_RunShaderParser(pcDirectory, 
//        bRecurseSubFolders);

	return true;
}

unsigned int CObjectPick::ShaderErrorCallback(const char* pcError, 
    NiShaderError eError, bool bRecoverable)
{
    NiMessageBox(pcError, "Shader Error");
    NiOutputDebugString("ERROR: ");
    NiOutputDebugString(pcError);

    return 0;
}

NiBinaryShader* CObjectPick::CreateNiBinaryShaderCallback(
    const char* pcClassName)
{
    if (pcClassName && strcmp(pcClassName, "ObjectPickBinaryShader") == 0)
    {
        return NiNew ObjectPickBinaryShader();
    }

    return NULL;
}