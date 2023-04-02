#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

#include "common.h"

/* 스코어 보드
 점수판에 대한 모든 처리를 하는 클래스

 */

class ScoreBoard
{
	int			playNum;		//플레이어 넘버
	int			score;			//현재의 점수
	int			reachScore;		//도달 점수
	int			rank;			//등급

	//char		divideScore[4];	//점수를 자리수마다 분리

	SDL_Surface *scoreSheet;		//점수 시트
	SDL_Surface *playerNumSheet;	//플레이어 번호 시트
 
	SDL_Rect	scorePos;			//점수를 그릴 위치
	SDL_Rect	ScoresheetPos;		//시트상의 위치

	SDL_Rect	playerNumPos;		//플레이어 번호를 그릴 위치
	SDL_Rect	playerNumSheetPos;	//시트상의 플레이어 번호 위치
	
public:
	ScoreBoard(int PlayerNum);			//플레이어 넘버를 넘겨주고 초기화
		
	void Draw(SDL_Surface* pScreen);	//스코어 보드 그리기
	void ScoreUpdate();					//점수 업데이트
//	void PositionResetByRank();			//등급에 따른 위치 재조정
	void SetReachScore(int ReachScore);					//점수 더하거나 빼기
	void CleanUp();

};


#endif
