#ifndef OBJECTPICK_H_
#define OBJECTPICK_H_

/*
클래스 설명
오브젝트의 픽킹을 처리하기 위한 클래스
*/
#include "ColorPicker.h"

class NiBinaryShader;
class NiShaderLibrary;

class CObjectPick
{
public:
	CObjectPick();
	~CObjectPick();

	bool Init();								//초기화
	bool CreatePicker();						//colorPick 객체 생성
	bool CheckPick(NiCursor* pkCursor,NiNode* pkSceneRoot,NiCamera* pkCamera);		//픽킹이 되었는지 확인 
	
	const NiAVObject* GetPickedObject();		//픽킹된 오브젝트 얻기
	void Release();

public:	

	//쉐이더 부분
	bool SetupShaderSystem();
    bool RunShaderParsers();
    bool RegisterShaderLibraries();
    static bool LibraryClassCreate(const char* pcLibFile, 
        NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
        bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
    static unsigned int RunParser(const char* pcLibFile, 
        NiRenderer* pkRenderer, const char* pcDirectory, 
        bool bRecurseSubFolders);
    static unsigned int ShaderErrorCallback(const char* pcError, 
        NiShaderError eError, bool bRecoverable);
    static NiBinaryShader* CObjectPick::CreateNiBinaryShaderCallback(
        const char* pcClassName);

	//멤버 변수
	ColorPicker*		m_pkColorPicker;		//피커
	const NiAVObject*	m_pkPickedObject;		//피킹된 오브젝트
};


#endif