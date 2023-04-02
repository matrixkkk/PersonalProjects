#include <SDL/SDL.h>
#include "gameengine.h"
#include "gamestate.h"


#define TARGET_FPS 60

void CGameEngine::Init(const char* title, int width, int height,
					   int bpp, bool fullscreen,int netState)
{
	int flags = 0;

	SDL_Init(SDL_INIT_EVERYTHING);

	if ( fullscreen ) {
		flags = SDL_FULLSCREEN;
	}
	screen = SDL_SetVideoMode(width, height, bpp, SDL_DOUBLEBUF);

	m_fullscreen = fullscreen;
	m_running = true;

	serverclose			=	false;	// Ver.MadCow
	changeServerCount	=	0;		// Ver.MadCow ���� ���� Ƚ���� �ʱ� 0�� �Ѵ�.


	curTicks = SDL_GetTicks();

	if(netState == 0) //������ ��
	{
		gameNet = new CMyGameNetServer();
	}
	else		//Ŭ���̾�Ʈ �� ��
	{
		gameNet = new CMyGameNetClient();
	}

	//�Ӱ迵�� �ʱ�ȭ
	InitializeCriticalSection(&cs);
	TTF_Init();
}

void CGameEngine::Cleanup()
{
	while ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// switch back to windowed mode so other
	// programs won't get accidentally resized
	if ( m_fullscreen ) {
		screen = SDL_SetVideoMode(800, 600, 0, 0);
	}

	if(gameNet)			//��Ʈ��ũ ��ü �Ұ�
		delete gameNet;

	//�Ӱ迵�� ����
	DeleteCriticalSection(&cs);

	TTF_Quit();	
}

void CGameEngine::ChangeState(CGameState* state)
{
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);	
	states.back()->Init(this);	
}

void CGameEngine::PushState(CGameState* state)
{
	// pause current state
	if ( !states.empty() ) {
		states.back()->Pause();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->Init(this);
}

void CGameEngine::PopState()
{
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// resume previous state
	if ( !states.empty() ) {
		states.back()->Resume();
	}
}

void CGameEngine::HandleEvents()
{
	// let the state handle events
	states.back()->HandleEvents(this);
}

void CGameEngine::Update()
{
	// let the state update the game
	states.back()->Update(this);
}

void CGameEngine::Draw()
{
	states.back()->Draw(this);
}

void CGameEngine::UpdateScreen(void)
{
	SDL_Flip(screen);
}


void CGameEngine::RegulateFPS()
{

	Uint32 elapsedTicks = SDL_GetTicks() - curTicks;
	if (1000 > TARGET_FPS * elapsedTicks)
		SDL_Delay(1000 / TARGET_FPS - elapsedTicks);

	elapsedTicks = SDL_GetTicks() - curTicks;
	curTicks += elapsedTicks;

	SDL_Rect pos = {10, 550, 0, 0};
}

CNetSystem* CGameEngine::GetNetClass()
{
	return gameNet;
}

//���� ������ ���� �� ���� ���ҽ��� ������ �Լ�
void CGameEngine::StorageData()
{	
	MYDATA *tempData;									//������ ������ ���� �ӽ� ������

	//�ӽ� �÷��̾� ����

	tempData=states.back()->Collect();					//tempData�� ó�� ���� �����͸� �޾ƿ´�.
	tempData->numOfPlayer = this->arg->currPlayerNum;					//�÷��̾� ����.
	//��ƿ� ���� �����͸� ���� ���ҽ��� ����

	gameNet->publicDataProcess(tempData);	
}