#ifndef OBJECTPICK_H_
#define OBJECTPICK_H_

/*
Ŭ���� ����
������Ʈ�� ��ŷ�� ó���ϱ� ���� Ŭ����
*/
#include "ColorPicker.h"

class NiBinaryShader;
class NiShaderLibrary;

class CObjectPick
{
public:
	CObjectPick();
	~CObjectPick();

	bool Init();								//�ʱ�ȭ
	bool CreatePicker();						//colorPick ��ü ����
	bool CheckPick(NiCursor* pkCursor,NiNode* pkSceneRoot,NiCamera* pkCamera);		//��ŷ�� �Ǿ����� Ȯ�� 
	
	const NiAVObject* GetPickedObject();		//��ŷ�� ������Ʈ ���
	void Release();

public:	

	//���̴� �κ�
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

	//��� ����
	ColorPicker*		m_pkColorPicker;		//��Ŀ
	const NiAVObject*	m_pkPickedObject;		//��ŷ�� ������Ʈ
};


#endif