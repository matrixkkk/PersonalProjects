#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL/SDL.h>
#include "GameApp.h"
#include "IntroState.h"


int main ( int argc, char** argv )
{
    CGameApp Game;

    Game.Init();

    Game.ChangeState( CIntroState::CreateInstance() );

    while( Game.Run() )
    {
        Game.HandleEvents();
        Game.Update();
        Game.Draw();
        Game.RegulateFPS();
        Game.UpdateScreen();
    }

    Game.Release();

    return 0;


}
