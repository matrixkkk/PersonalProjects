#include "scoreboard.h"

ScoreBoard::ScoreBoard(int PlayerNum)
{
	score = 0;
	reachScore = 0;
	rank = 0;
	playNum = PlayerNum;
	//점수 시트 로딩
	SDL_Surface *temp=SDL_LoadBMP("../../Game/image/number_sheet6.bmp");
	scoreSheet=SDL_DisplayFormat(temp);
	SDL_SetColorKey( scoreSheet, SDL_SRCCOLORKEY, SDL_MapRGB( scoreSheet->format, 0, 0, 100 ));
	
	//플레이어 넘버 시트 로딩
	temp=SDL_LoadBMP("../../Game/image/id_sheet.bmp");
	playerNumSheet=SDL_DisplayFormat(temp);
	SDL_SetColorKey( playerNumSheet, SDL_SRCCOLORKEY, SDL_MapRGB( playerNumSheet->format, 0, 0, 100 ));
	
	//플레이어 넘버에 따른 점수 위치 설정(초기위치)
	scorePos.x = 580;
	scorePos.y = 105 + PlayerNum * 30;
	scorePos.w = 15;
	scorePos.h = 20;
	

	//플레이어 넘버에 따른 넘버 위치 설정(초기위치)
	playerNumPos.x = 505;
	playerNumPos.y = 105 + PlayerNum * 30;
	playerNumPos.w = 20;
	playerNumPos.h = 20;

	//플레이어 넘버 시트
	playerNumSheetPos.x = 0;
	playerNumSheetPos.y = 20 * playNum;
	playerNumSheetPos.w = 20;
	playerNumSheetPos.h = 20;
}

void ScoreBoard::ScoreUpdate()
{
	if(reachScore > score)
	{
		score+=2;
	}
	else if(reachScore < score)
	{
		score-=2;
	}	
}

void ScoreBoard::Draw(SDL_Surface *pScreen)
{
	int divideScore[4];	//점수를 자리수마다 분리
	int tmp;

	SDL_Rect scoreRect;

	if(score < 0)
		score=0;	

	divideScore[0]=score/1000;
	
	tmp=score%1000;
	divideScore[1]=tmp/100;
	tmp=tmp%100;
	divideScore[2]=tmp/10;
	divideScore[3]=tmp%10;
		
	//점수 그리기
	for(int i=0;i<4;i++)
	{
		ScoresheetPos.x=divideScore[i]*15;
		ScoresheetPos.y=playNum * 20;
		ScoresheetPos.w=15;
		ScoresheetPos.h=20;

		scoreRect.x=scorePos.x + i*15;
		scoreRect.y=scorePos.y;
		scoreRect.w=15;
		scoreRect.h=20;
		SDL_BlitSurface(scoreSheet, &ScoresheetPos, pScreen, &scoreRect);
	}	
	//플레이어 넘버 그리기
	SDL_BlitSurface(playerNumSheet,&playerNumSheetPos,pScreen,&playerNumPos);
}

void ScoreBoard::SetReachScore(int ReachScore)
{
	reachScore = ReachScore;
}

void ScoreBoard::CleanUp()
{
	SDL_FreeSurface(scoreSheet);
	SDL_FreeSurface(playerNumSheet);
}
