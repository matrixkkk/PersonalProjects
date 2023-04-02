/*
�̱� �׽�Ʈ�� ũ����
��ũ��ũ ���� ������ 

*/
#ifndef CSINGLEPLAYSTATE_H
#define CSINGLEPLAYSTATE_H

#include "GameApp.h"
#include "CGameUIState.h"
#include "WorldManager.h"

//#include "ObjectPick.h"


class CSinglePlayState : public CGameUIState
{
	enum{ ID, KILL, DEATH };
	enum{ USA,EU, GUERRILLA };

	CEGUI::FrameWindow		*m_pMenuBox;

	FMOD::Channel*			m_pBGChannel;		//��� ���� ä��

public:
	
	CSinglePlayState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);
	~CSinglePlayState();

	//================================================================
	//					virtual
	//================================================================
	void OnUpdate(float fCurrentTime);
	void OnLeave(const char* szNext);
	void OnOverride(const char* szNext);
	void OnResume(const char* szPrevious);
	void OnProcessInput();
	void OnRender();


protected:

	// ���� �ʱ�ȭ
	virtual bool Initialize();
	//	UI ����
	bool CreateUI();
	bool LoadAllEffectSound();	//��� ����Ʈ ���� �ε�

	void ProcessKeyBoard();		//Ű���� ó�� �Լ�
	void ProcessMouse();		//���콺 ó�� �Լ�
	void ProcessKeyBoardForUI();

	//================================================================
	//					Event ó��
	//================================================================


	bool Event_MenuBoxExitBtnClick	(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxCancelBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxOptionBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxVoteBtnClick	(const CEGUI::EventArgs &rE);
	//================================================================
	//================================================================
	
	unsigned int idx;

};

#endif
