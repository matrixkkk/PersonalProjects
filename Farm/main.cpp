#include "gameengine.h"
#include "introstate.h"
//#include <windows.h>
extern CIntroState introState;

int main(int argc, char *argv[])
{
 //LARGE_INTEGER freq, Start, End;
 //time_t start, finish;

  Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);

  CGameEngine game;

  game.Init("Engine Test v1.0");

  game.ChangeState(&introState);

  while ( game.Running() )
  {
	//QueryPerformanceFrequency(&freq); 
   // time(&start);
	// 시간 측정 시작
//	QueryPerformanceCounter(&Start);
    game.HandleEvents();
   
	game.Update();
	game.Draw();    
    game.RegulateFPS();
    game.UpdateScreen();
	//time(&finish);
  
  //시간 측정 끝
  //QueryPerformanceCounter(&End);
 // printf("***%g***\n", difftime(start, finish));

  //printf("\n%f ms\n", (double)(End.QuadPart-Start.QuadPart)/freq.QuadPart);
  }
  game.Cleanup();
  Mix_CloseAudio();

  return 0;
}
 