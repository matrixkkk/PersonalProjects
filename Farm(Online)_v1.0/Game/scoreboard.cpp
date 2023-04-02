#include "scoreboard.h"

ScoreBoard::ScoreBoard(int PlayerNum)
{
	score = 0;
	reachScore = 0;
	rank = 0;
	playNum = PlayerNum;
	//���� ��Ʈ �ε�
	SDL_Surface *temp=SDL_LoadBMP("../../Game/image/number_sheet6.bmp");
	scoreSheet=SDL_DisplayFormat(temp);
	SDL_SetColorKey( scoreSheet, SDL_SRCCOLORKEY, SDL_MapRGB( scoreSheet->format, 0, 0, 100 ));
	
	//�÷��̾� �ѹ� ��Ʈ �ε�
	temp=SDL_LoadBMP("../../Game/image/id_sheet.bmp");
	playerNumSheet=SDL_DisplayFormat(temp);
	SDL_SetColorKey( playerNumSheet, SDL_SRCCOLORKEY, SDL_MapRGB( playerNumSheet->format, 0, 0, 100 ));
	
	//�÷��̾� �ѹ��� ���� ���� ��ġ ����(�ʱ���ġ)
	scorePos.x = 580;
	scorePos.y = 105 + PlayerNum * 30;
	scorePos.w = 15;
	scorePos.h = 20;
	

	//�÷��̾� �ѹ��� ���� �ѹ� ��ġ ����(�ʱ���ġ)
	playerNumPos.x = 505;
	playerNumPos.y = 105 + PlayerNum * 30;
	playerNumPos.w = 20;
	playerNumPos.h = 20;

	//�÷��̾� �ѹ� ��Ʈ
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
	int divideScore[4];	//������ �ڸ������� �и�
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
		
	//���� �׸���
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
	//�÷��̾� �ѹ� �׸���
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
