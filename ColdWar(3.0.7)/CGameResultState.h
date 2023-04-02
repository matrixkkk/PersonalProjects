#ifndef CGAMERESULTSTATE_H
#define CGAMERESULTSTATE_H

#include "CGameUIState.h"
#include "NetWorkManager.h"
#include <algorithm>
#include <vector>


class CGameResultState : public CGameUIState
{
	enum{ ID, KILL, DEATH };
	enum{ USA, EU, GUERRILLA };

	CEGUI::MultiColumnList	*m_pListMcl[3];

	float	m_fStartTime;

	std::vector< GAME_USER_INFO > m_vecPlayers;

public:

	//	Á÷¾÷	
	CGameResultState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);

	void SavePlayerData();


protected:
	
	virtual bool Initialize();

	// Overrides and concrete implementations of IBaseState methods.
	virtual void OnUpdate(float fCurrentTime);
	virtual void OnLeave(const char* szNext);

	bool Event_OkBtnClick(const CEGUI::EventArgs &rE);
	
};

#endif 