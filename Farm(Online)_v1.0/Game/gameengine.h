
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "common.h"
#include "NetSystem.h"
#include "CMyGameNetClient.h"
#include "CMyGameNetServer.h"


class CGameState;

class CGameEngine
{
public:

	void Init(const char* title, int width=480, int height=272,
		      int bpp=0, bool fullscreen=false, int netState=0);
	void Cleanup();

	void ChangeState(CGameState* state);
	void PushState(CGameState* state);
	void PopState();

	void HandleEvents();
	void Update();
	void StorageData();					//���� ������ ���� ���ҽ��� ����.
	void Draw();
	void UpdateScreen(void);

	void RegulateFPS(void);
	
	bool Running() { return m_running; }
	void Quit() { m_running = false; }

	CNetSystem* GetNetClass();	//��Ʈ��ũ Ŭ���� �ּ� ����

	int map;

	bool	serverclose;				// Ver.MadCow ���ν����忡�� ������ ��������θ� �����Ѵ�.
	int		changeServerCount;			//Ver.MadCow �������� Ƚ�� ����

	SDL_Surface* screen;		
	Argument		 *arg;			//arg ����
	//������ ����ȭ�� ���� �Ӱ迵�� static ����
	CRITICAL_SECTION cs;

	// the stack of states
	vector<CGameState*> states;
private:	

	CNetSystem	*gameNet;	//���� ��Ʈ��ũ ��ü

	bool m_running;
	bool m_fullscreen;

	int  playerNum;			//�÷��̾� �ѹ�.

  Uint32 curTicks;
};

#endif
