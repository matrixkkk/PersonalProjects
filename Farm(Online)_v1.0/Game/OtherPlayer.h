#ifndef OTHERPLAYER_H_
#define OTHERPLAYER_H_

#include "common.h"
#include "block.h"
#include "gameengine.h"
#include "scoreboard.h"


//다른 플레이어 정보 구조체
struct OtherPlayerGameInform
{
	SDL_Rect		bar;				//bar를 그릴 위치
	SDL_Rect		ballPos[4];			//ball을 그릴 위치
	
	ITEMINFO		itemInfo;			//아이템 정보
	
	deque<CBlock> otherBlocks;				//블럭들

	int				brokenBlock[10];		//최근 깨진 블록들
	int				score;				//점수
	int				reachScore;			//도달 점수
	int				playerNum;			//플레이어 넘버
	UINT			numOfBall;			//볼의 수
	
	OtherPlayerGameInform()
	{
		bar.x		 = LEFTLINE+180; bar.y = BOTTOMLINE-20; bar.w = 100; bar.h = 20;
		ballPos[0].x = LEFTLINE+220; ballPos[0].y = BOTTOMLINE-35; ballPos[0].w = 20, ballPos[0].h = 20;
		score = 0;
		reachScore = 0;
		numOfBall = 0;		
		memset(brokenBlock,-1,sizeof(brokenBlock));
	}
};

//다른 플레이어의 게임 정보와 처리를 위한 클래스
class COtherPlayer
{
	vector<OtherPlayerGameInform>	otherPlayer;		//다른 플레이어 정보들
	SDL_Surface						*m_pScreen;			//스크린 포인터
	SDL_Surface						**surfaceArray;		//이미지 표면 포인터 리스트
	
	int		numOfOtherPlayer;						//다른 플레이어들의 수
	int		currPlayerNum;								//현재 유저의 번호
	
	ITEMINFO	targetedItem;							//현재 유저가 공격당한 아이템의 정보
	
	//폰트 관련
	TTF_Font *font;
	SDL_Surface *ttfBlended[6];
	SDL_Rect idRect[6];				//아이디를 그릴 위치.
	SDL_Rect *idRectArray[6];
	
public:
	
	vector<ScoreBoard>				board;					//스코어 보드
	
	void	init(SDL_Surface ** images,SDL_Surface *screen,int players,deque<CBlock> blocks,Argument *arg);	//클래스 초기화
	void	Draw				();				//다른 플레이어 공 그리기
	void	ChangePort			(int idx,SDL_Rect *rect);		// 변환해서 그리기
	void	SetPlayersInform	(CGameEngine *pGame);	//다른 플레이어 데이터 저장
	void	CleanUp();			//메모리 소거
	void	NonBrokenBlockCreate();						

	int		GetCurrPlayerNum()		{ return currPlayerNum; }

	ITEMINFO* GetTargetItemInfo()	{ return &targetedItem; }

};


#endif
