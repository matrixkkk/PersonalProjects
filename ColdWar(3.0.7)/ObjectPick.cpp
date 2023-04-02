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
	//~������ �κ�
	m_pkColorPicker = NULL;
	m_pkPickedObject = NULL;
}
//�Ҹ���
CObjectPick::~CObjectPick()
{
	Release();
}

bool CObjectPick::Init()
{
	//���̴� �ý��� �ʱ�ȭ
	if(!SetupShaderSystem())
	{
		return false;
	}

	//Į����Ŀ ����
	if(!CreatePicker())
	{
		NiMessageBox("CObjectPick::Init() - CreatePicker() Failed!",NULL);
		return false;
	}

	return true;
}

bool CObjectPick::CheckPick(NiCursor *pkCursor,NiNode* pkSceneRoot,NiCamera* pkCamera)
{
	//���콺 x,y ��ǥ ����
	int iX = 0;
	int iY = 0;
	

	//------��ŷ ����----------
	
	//Ŀ���� ���� x,y ��ġ�� �����´�.
	pkCursor->GetScreenSpacePosition(iX,iY);
		
	//������ ��ŷ�ϴ� �κ��Դϴ�. ��ŷ ������ ��������� ī�޶� Ŭ������ 
	//�Ѱ��־�� �մϴ�.
	if(m_pkColorPicker)
	{			
		m_pkColorPicker->StartPick(pkCamera,iX,iY);			//��ŷ ����
		m_pkColorPicker->RecursivePickRender(pkSceneRoot);
		m_pkPickedObject = m_pkColorPicker->EndPick();		//��ŷ �Ϸ�

//		char buffer[512];
		//��ŷ ���
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

//Į����Ŀ ����
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

//��ŷ�� ������Ʈ ����
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