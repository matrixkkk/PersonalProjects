#include <SDL/SDL.h>
#include <SDL/SDL_Mixer.h>
#include <windows.h>
#include "GameApp.h"
#include "GameState.h"


void CGameApp::Init()
{

    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        printf(" SDL Init Failed!!");
        return;
    }

    m_Screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE|SDL_DOUBLEBUF );

     if( NULL == m_Screen  )
     {
         printf( "Unable to set 800x680 video: %s\n", SDL_GetError() );

         m_Screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

         if( NULL == m_Screen  )
         {
            printf("Unable to set 800x680 video: %s\n", SDL_GetError() );
            SDL_Quit();
         }
     }

     SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

     m_Running     = true;
     m_GameState = 0;

     if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0 )
    {
        MessageBox( NULL, "OpenAudio Fail", "OpenAudio Fail", MB_OK );
        return;
    }
//-----------------------------------------
    m_PlayerList.Init();

    m_CurTick = SDL_GetTicks();
    TTF_Init();
    font = TTF_OpenFont("HYNAMB.ttf", 20);

}

void CGameApp::ChangeState( CGameState *pGameState )
{
    if( NULL != m_GameState )
    {
        m_GameState->Release();
        m_GameState = 0;
    }

    m_GameState = pGameState;

    m_GameState->Init();
}

void CGameApp::Draw()
{
    m_GameState->Draw( this);
}


void CGameApp::HandleEvents()
{
    m_GameState->HandleEvents( this );
}


void CGameApp::Release()
{
    SDL_FreeSurface( m_Screen );
    Mix_CloseAudio();
    m_PlayerList.Release();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}


void CGameApp::Update()
{
    m_GameState->Update( this );

}

void CGameApp::RegulateFPS()
{
    SDL_Color fgColor={0,0,0}, bgColor = {128, 128, 128};
    char fps[40];

    Uint32 elapsedTicks = SDL_GetTicks() - m_CurTick;

    if (1000 / elapsedTicks > TARGET_FPS)
        SDL_Delay(1000 / TARGET_FPS - elapsedTicks);

    elapsedTicks = SDL_GetTicks() - m_CurTick;
    m_CurTick += elapsedTicks;

    sprintf(fps, "%3d ms, %6.1f fps", elapsedTicks, 1000.0 / (float)elapsedTicks);
    SDL_Surface *fpsMsg = TTF_RenderText_Shaded(font, fps, fgColor, bgColor);
    SDL_Rect pos = {10,550, 0, 0};
    SDL_BlitSurface(fpsMsg, NULL, m_Screen, &pos);
    SDL_FreeSurface(fpsMsg);

}

 void CGameApp::UpdateScreen()
 {
     SDL_Flip( m_Screen );

 }





