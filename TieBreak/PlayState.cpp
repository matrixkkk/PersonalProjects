#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
#include <windows.h>
#include <stdio.h>

#include "GameApp.h"
#include "PlayState.h"
#include "DBPlayer.h"
#include "StageParameter.h"
#include "MapSelectState.h"

CPlayState *CPlayState::m_Instance = 0;

void CPlayState::Init()
{

    CStageParameter  *GameInfo = m_Game->GetStageParameter();

    std::string mapname = GameInfo->GetMap();

    mapname = "BMP/" + mapname + ".bmp";

    SDL_Surface *temp = SDL_LoadBMP(mapname.c_str() );

    m_BgSurface = SDL_DisplayFormat( temp );

    temp = SDL_LoadBMP("BMP/eventmessage.bmp");

    m_Event = SDL_DisplayFormat( temp );

    SDL_SetColorKey( m_Event, SDL_SRCCOLORKEY, SDL_MapRGB( m_Event->format, 255, 255, 255) );

    temp = SDL_LoadBMP("BMP/Score.bmp");

    m_Score = SDL_DisplayFormat( temp );

    SDL_SetColorKey( m_Score, SDL_SRCCOLORKEY, SDL_MapRGB( m_Score->format, 255, 255, 255) );

    SDL_FreeSurface( temp );



    m_CharacterOrder = GameInfo->GetCharacter();

    std::vector<CDBPlayer*> PlayerNameList;

    m_Game->GetPlayerList()->GetPlayerList( GameInfo->GetTeam(), PlayerNameList );

    std::deque< std::string >::const_iterator charName = m_CharacterOrder.begin();

    for( std::vector< CDBPlayer* >::const_iterator it  = PlayerNameList.begin(); it != PlayerNameList.end(); ++it )
    {
        if( (*charName) == (*it)->GetName() )
        {
            m_ControlPlayer = new CPlayer( (*it)->GetStrength(), (*it)->GetSpeed(), (*it)->GetDefneceRange() );
            break;
        }
    }
     ball        =   new CBall();
     Control     =   new Cgameplaycontrol();


    //캐릭터 생성
    if( NULL != m_ControlPlayer )       //1p 초기화
        m_ControlPlayer->Init( WOMAN, "BMP/woman.bmp" );

    m_ControlPlayer->SetPosition( 250, 320 );
    m_ControlPlayer->SetRuleMode(SURVEMODE);


    m_ControlPlayer1 = new CPlayer( 10,10,10 );
    m_ControlPlayer1->Init( MAN, "BMP/man.bmp" );
    m_ControlPlayer1->SetPosition( -250, 2000 );
    m_ControlPlayer1->SetRuleMode(BATTLEMODE);

    eventtype   =   NOEVENT;
    //
    TTF_Init();
    font = TTF_OpenFont("HYNAMB.ttf", 50);

    SDL_ShowCursor( false );
}

void CPlayState::Draw( CGameApp* game )
{
    SDL_BlitSurface( m_BgSurface, NULL, game->getScreen(), NULL );

    m_ControlPlayer1->Draw( game );

    if(Control->GetState()==0) ball->draw();

    m_ControlPlayer->Draw( game );

    if( eventtype != NOEVENT )
        DrawEvent(game);

    DrawScore(game);


    SDL_Flip( game->getScreen() );
}


void CPlayState::Release()
{
	SDL_FreeSurface(m_BgSurface);
	SDL_FreeSurface(m_Event);
	SDL_FreeSurface(m_Score);
	SDL_FreeSurface(userscore);
	SDL_FreeSurface(comscore);
	m_ControlPlayer->Release();
	m_ControlPlayer1->Release();
	delete ball;

    TTF_Quit();
	SDL_ShowCursor(false);
}
void CPlayState::DrawScore( CGameApp* game )
{

    SDL_Rect dest;

    dest.x = 630;
    dest.y = 20;

    SDL_BlitSurface( m_Score, NULL, game->getScreen(), &dest );

    SDL_Color color={0,0,0};
    char temp[3];

    sprintf(temp, "%d", Control->GetUserScore() );
    userscore = TTF_RenderText_Blended(font,temp,color);
    dest.x = 725;
    dest.y = 70;
    SDL_BlitSurface( userscore, NULL, game->getScreen(), &dest );

    sprintf(temp, "%d", Control->GetComScore() );
    comscore = TTF_RenderText_Blended(font,temp,color);
    dest.x = 725;
    dest.y = 120;
    SDL_BlitSurface( comscore, NULL, game->getScreen(), &dest );

    //userscore = TTF_RenderText_Solid(font,"English Font Output Solid",color);

}

void CPlayState::DrawEvent( CGameApp* game )
{
    static int eventtime = 0;
    static int count = 0;
    static SDL_Rect r;
    static SDL_Rect dest;

    if( (count % 100 ) )
        eventtime++;

    if( eventtime >= 10 )
    {
        eventtime = 0;
        eventtype = NOEVENT;
    }

    r.x = 0;
    r.y = 56 * eventtype;
    r.w = 340;
    r.h = 56;

    switch( eventtype )
    {
        case SERVEFAULT:
            dest.x = 230;
            break;
        case MANWIN:
        case WOMANWIN:
            dest.x = 300;
            break;
    }
    dest.y = 240;
    count++;

    SDL_BlitSurface( m_Event, &r, game->getScreen(), &dest );

}

void CPlayState::Update( CGameApp* game )
{
    Control->CheckGameState(ball, m_ControlPlayer, m_ControlPlayer1 );
    //AI->AIaction(ball,m_ControlPlayer,m_ControlPlayer1);
    eventtype = Control->GetEventMessage();
    m_ControlPlayer->Update( game );
    m_ControlPlayer1->Update( game );
    ball->Ball_collision(m_ControlPlayer, m_ControlPlayer1, Control->GetState());
}

void CPlayState::HandleEvents( CGameApp* game )
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


        //플레이어 2
        if( GetAsyncKeyState('A') & 0x8000f )
        {
            m_ControlPlayer1->SetLeft(true);
            m_ControlPlayer1->SetDirection( LEFTDIRECTION );
            m_ControlPlayer1->LeftWalk();
        }

        if( GetAsyncKeyState('D') & 0x8000f )
        {
            m_ControlPlayer1->SetRight(true);
            m_ControlPlayer1->SetDirection( RIGHTDIRECTION );
            m_ControlPlayer1->RightWalk();
        }

        if( GetAsyncKeyState('S') & 0x8000f )
        {
            m_ControlPlayer1->SetDown(true);
            m_ControlPlayer1->SetDirection( FRONTDIRECTION );
            m_ControlPlayer1->DownWalk();
        }
        if( GetAsyncKeyState('W') & 0x8000f )
        {
            m_ControlPlayer1->SetUp(true);
            m_ControlPlayer1->SetDirection( FRONTDIRECTION );
            m_ControlPlayer1->UpWalk();
        }

        ///////////////////

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
                        /*
                    case SDLK_v:
                        m_ControlPlayer1->Setaction(0);
                        break;
                    case SDLK_b:
                        m_ControlPlayer1->Setaction(0);
                        break;
                        */

                }
                m_ControlPlayer1->Stand();
                m_ControlPlayer->Stand();
                break;

		}
	}

}