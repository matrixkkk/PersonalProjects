
#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <SDL/SDL.h>
#include "gamestate.h"
#include "common.h"
#include "block.h"
#include "bar.h"
#include "ball.h"
#include "item.h"
#include "OtherPlayer.h"
#include "turtle.h"
#include "CEditBox.h"


class CPlayState : public CGameState
{
	SDL_Surface		*m_sBackGround;		//게임 배경 화면을 로딩할 표면	
	SDL_Surface     *m_sBlock;	        //블럭을 로딩할 표면
	SDL_Surface		*m_sBar;	     	//bar 이미지를 로딩할 표면
	SDL_Surface		*m_sBall;			//ball 이미지를 로딩할 표면.
	SDL_Surface		*m_sBlockAnim;		//블럭 파괴 애니메이션을 로딩할 표면
	SDL_Surface		*m_sScore;			//스코어 시트
	SDL_Surface		*m_sItemSheet;		//아이템 시트
	SDL_Surface		*m_sLaser;			//레이저 시트
	SDL_Surface		*s_Obstarcle;		//장애물의 이미지 로딩할 표면
	
	//이미지 포인터 리스트
	SDL_Surface		*imageBuffer[6];
	Mix_Music       *m_mBgm[2];			//음악 로딩.
	
	//효과음을 저장할 포인터 변수들 
	Mix_Chunk		*m_wCow;
	Mix_Chunk		*m_wChiken;
	Mix_Chunk		*m_wLion;
	Mix_Chunk		*m_wSheep;
	Mix_Chunk		*m_wPig;
	Mix_Chunk		*m_wDog;
	Mix_Chunk		*m_wBump;
	

	///-------레이저 아이템 관련 변수------
	SDL_Rect		m_rLaser;			//레이저를 그릴 rect
	SDL_Rect		m_rSheetLaser;	    // 시트 상의 레이저 위치
	CPoint2			m_cpLaserPos;
	int				m_LaserFrame;    	//레이저 애니메이션 프레임

    //공을 받을 Bar 변수들 
	CBar			*m_pCBPlayer;          //Player라 명명 
	
	CTurtle			turtle;					//거북이

	int				m_IDReset;		//볼의 ID가 리셋시키는 카운터	
	int				m_score;	       //볼의 점수
	int				m_ReachScore;
	int				m_GenerateItem;	//생성되는 아이템의 종류.
	int				m_map;			//맵의 종류
	
	bool			m_bLaserOn;		//레이저를 쏘기 위한 변수
	
	bool			m_bObstarcle;                //장애물을 출력 하기 위한 변수
	int             m_LifetimeObstarcle,m_LiftimeControlShock ;      //장애물 출력 시간, 콘트롤쇼크 지속 시간
	
	double			m_duration;				     //블록 추가 타이머
	int				m_addBlockLine;				 //추가된 블록 라인 수
	int				m_minCollisionLine;			//최소 충돌 라인
	
	//(기획서 추가) 다른 플레이어의 정보 처리 객체
	COtherPlayer    m_otherPlayer;
	//int				numOfPlayer;		//플레이어의 수 ( 현재 사용되지 않음)
	
	MYDATA			colData;			// 게임정보 수집을 위한 변수
	CTimer			m_timer;			//타이머 클래스
	char			itemDropTable[100];		//아이템 드랍률

	Argument        *pArgs;
	//채팅 메세지 
	CEditBox		chatEditBox;
public:
	deque<int>		m_brokenBlockIdx;		//부셔진 블록의 인덱스
	deque<CBlock>	m_deqBlock;				//블럭을 담을 컨테이너
	deque<CBall>	m_deqBall;				//1p 볼 클래스를 담을 컨테이너 , 볼의 숫자는 증가하기 때문에 컨테이너로 만든다.
	deque<Citem*>   m_vecAbtainItem;		//1p 아이템을 담을 컨테이너
	vector<Citem*>  m_vecPublicItem;		//생성된 아이템을 보관 하는 컨테이너
	
	CPlayState() { }
	void Init(CGameEngine* game);
	void Cleanup();

	void Pause();
	void Resume();
	
	MYDATA* Collect();			//데이터 수집 함수

	void HandleEvents(CGameEngine* game);
	void Update(CGameEngine* game);
	void Draw(CGameEngine* game);
	void UpdateScore();
	
	//일반 함수들
	void SetArgument(Argument *arg);
	void NonBrokenBlockCreate();			//부셔지지않는 블럭 생성.
	void CreateMap(int map);          // 맵 생성

	inline void CheckColision();	  //충돌 체크
	inline void AddScore(deque<CBlock>::iterator pos,int &score);		//점수 증가.
	inline void DrawItemBox();		  //아이템 박스 그리기
	inline void TargetItemProcess();  //공격당한 아이템 처리

};

#endif
