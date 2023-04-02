
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
	void StorageData();					//게임 데이터 공유 리소스에 저장.
	void Draw();
	void UpdateScreen(void);

	void RegulateFPS(void);
	
	bool Running() { return m_running; }
	void Quit() { m_running = false; }

	CNetSystem* GetNetClass();	//네트워크 클래스 주소 리턴

	int map;

	bool	serverclose;				// Ver.MadCow 메인스레드에서 서버의 재생성여부를 결정한다.
	int		changeServerCount;			//Ver.MadCow 서버변경 횟수 저장

	SDL_Surface* screen;		
	Argument		 *arg;			//arg 정보
	//스레드 동기화를 위한 임계영역 static 변수
	CRITICAL_SECTION cs;

	// the stack of states
	vector<CGameState*> states;
private:	

	CNetSystem	*gameNet;	//게임 네트워크 객체

	bool m_running;
	bool m_fullscreen;

	int  playerNum;			//플레이어 넘버.

  Uint32 curTicks;
};

#endif
