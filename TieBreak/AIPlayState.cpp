#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
#include <windows.h>
#include <stdio.h>

#include "AIPlayState.h"
#include "MapSelectState.h"

AIPlayState *AIPlayState::m_Instance = 0;

void AIPlayState::Update( CGameApp* game )
{
    CPlayState::Update( game );
    AI->AIaction(ball,m_ControlPlayer,m_ControlPlayer1);
}

void AIPlayState::Init()
{
    CPlayState::Init();
    AI=new CAi;
}

void AIPlayState::Release()
{
    CPlayState::Release();
    delete AI;
}

void AIPlayState::Draw(CGameApp* game)
{
    CPlayState::Draw(game);
}

void AIPlayState::HandleEvents(CGameApp* game)
{
    if( GetAsyncKeyState(VK_LEFT) & 0x8000f )
        {
            m_ControlPlayer->SetLeft(true);
            m_ControlPlayer->SetDirection( LEFTDIRECTION );
            m_ControlPlayer->LeftWalk();

        }

        if( GetAsyncKeyState(VK_RIGHT) & 0x8000f )
        {
            m_ControlPlayer->SetRight(true);
            m_ControlPlayer->SetDirection( RIGHTDIRECTION );
            m_ControlPlayer->RightWalk();
        }

        if( GetAsyncKeyState(VK_DOWN) & 0x8000f )
        {
            m_ControlPlayer->SetDown(true);
            m_ControlPlayer->SetDirection( FRONTDIRECTION );
            m_ControlPlayer->DownWalk();
        }
        if( GetAsyncKeyState(VK_UP) & 0x8000f )
        {
            m_ControlPlayer->SetUp(true);
            m_ControlPlayer->SetDirection( FRONTDIRECTION );
            m_ControlPlayer->UpWalk();
        }


	if( GetAsyncKeyState('O') & 0x8000f )
	{
	    ballPoint = ball->GetBallPos();
	    m_ControlPlayer->Swing( ballPoint.x );
	}

	if( GetAsyncKeyState('P') & 0x8000f )
	{
	    ballPoint = ball->GetBallPos();
	    m_ControlPlayer->Diving( ballPoint.x );
	}

	if( GetAsyncKeyState('V') & 0x8000f )
	{
	    ballPoint = ball->GetBallPos();
	    m_ControlPlayer1->Swing( ballPoint.x );
	}

	if( GetAsyncKeyState('B') & 0x8000f )
	{
	     ballPoint = ball->GetBallPos();
	     m_ControlPlayer1->Diving( ballPoint.x );
	}


	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				game->Quit();
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {

					case SDLK_ESCAPE:
                        ChangeState( game, CMapSelectState::CreateInstance() );

						break;

                    case SDLK_o:
                        m_ControlPlayer->Setaction(1);
                        break;
                    case SDLK_p:
                        m_ControlPlayer->Setaction(1);
                        break;
                    case SDLK_v:
                        m_ControlPlayer1->Setaction(1);
                        break;
                    case SDLK_b:
                        m_ControlPlayer1->Setaction(1);
                        break;
			}
				break;

				case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        m_ControlPlayer->SetLeft(false);
                        break;
                    case SDLK_RIGHT:
                        m_ControlPlayer->SetRight(false);
                        break;
                    case SDLK_UP:
                        m_ControlPlayer->SetUp(false);
                        break;
                    case SDLK_DOWN:
                        m_ControlPlayer->SetDown(false);
                        break;

                    case SDLK_a:
                        m_ControlPlayer1->SetLeft(false);
                        break;
                    case SDLK_d:
                        m_ControlPlayer1->SetRight(false);
                        break;
                    case SDLK_w:
                        m_ControlPlayer1->SetUp(false);
                        break;
                    case SDLK_s:
                        m_ControlPlayer1->SetDown(false);
                        break;

                    case SDLK_o:
                        m_ControlPlayer->Setaction(0);
                        break;
                    case SDLK_p:
                        m_ControlPlayer->Setaction(0);
                        break;
                    case SDLK_v:
                        m_ControlPlayer1->Setaction(0);
                        break;
                    case SDLK_b:
                        m_ControlPlayer1->Setaction(0);
                        break;

                }
                m_ControlPlayer1->Stand();
                m_ControlPlayer->Stand();
                break;

		}
	}

}