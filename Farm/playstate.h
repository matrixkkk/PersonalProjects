
#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <SDL/SDL.h>
//#include <SDL/SDL_ttf.h>
#include "gamestate.h"
#include "common.h"
#include "block.h"
#include "bar.h"
#include "ball.h"
#include "item.h"

class CPlayState : public CGameState
{
	SDL_Surface		*bg;			//게임 배경 화면을 로딩할 표면	
	SDL_Surface     *s_block;		//블럭을 로딩할 표면
	SDL_Surface		*s_bar1;		//bar 이미지를 로딩할 표면
	SDL_Surface		*s_bar2;
	SDL_Surface		*s_ball1;		//ball 이미지를 로딩할 표면.
	SDL_Surface		*s_ball2;		//ball 이미지를 로딩할 표면.
	SDL_Surface		*s_block_ani;	//블럭 파괴 애니메이션을 로딩할 표면
	SDL_Surface		*s_score;		//스코어 시트
	SDL_Surface		*item_sheet;	//아이템 시트
	SDL_Surface		*s_Laser;		//레이저 시트

	SDL_Surface		*s_Nbar;		//장애물의 이미지 로딩할 표면
	
	Mix_Music       *m_Bgm;			//음악 로딩.
	
	Mix_Chunk		*cow;
	Mix_Chunk		*dak;
	Mix_Chunk		*lion;
	Mix_Chunk		*sheep;
	Mix_Chunk		*pig;
	Mix_Chunk		*dog;

	Mix_Chunk		*ball_high;
	Mix_Chunk		*ball_law;
	Mix_Chunk		*ball_spup;
	Mix_Chunk		*ball_spdown;
	Mix_Chunk		*ball_stop;
	Mix_Chunk		*ball_hide;
	Mix_Chunk		*key_revers;
	Mix_Chunk		*ball_die;
	Mix_Chunk		*ball_saperate;
	Mix_Chunk       *key_raiser;
	Mix_Chunk       *ball_point;
	Mix_Chunk		*ball_bar;

	///-------레이저 아이템 관련 변수------
	SDL_Rect		r_Laser1,r_Laser2;			//레이저를 그릴 rect
	SDL_Rect		sheet_Laser1,sheet_Laser2;	// 시트 상의 레이저 위치
	CPoint2			laser_pos1,laser_pos2;
	int				laser_frame1,laser_frame2;	//레이저 애니메이션 프레임

	CBar			*player1,*player2;
	CBar			*N_play;            // 장애물


	int				ID_reset;		//볼의 ID가 리셋시키는 카운터	
	int				score1,score2;	//볼의 점수
	int				ReachScore1,ReachScore2;
	int				GenerateItem;	//생성되는 아이템의 종류.
	int				map;			//맵의 종류
	int				b_pause;		//tip을 보여주기 위해 일시 정지 하는 변수

	bool			LaserOn1,LaserOn2;		//레이저를 쏘기 위한 변수
	
	bool			Nbar_On;                //장애물을 출력 하기 위한 변수
	int             tmp_bar, tmp_ubar;      //장애물 출력 시간, 콘트롤쇼크 지속 시간

public:
	deque<CBlock>	Block;			//블럭을 담을 컨테이너
	deque<CBall>	Ball1;			//1p 볼 클래스를 담을 컨테이너 , 볼의 숫자는 증가하기 때문에 컨테이너로 만든다.
	deque<CBall>	Ball2;			//2p 볼 클래스를 담을 컨테이너
	vector<Citem*>  item1;			//1p 아이템을 담을 컨테이너
	vector<Citem*>  item2;			//2p 아이템을 담을 컨테이너
	vector<Citem*>  G_item;			//생성된 아이템을 보관 하는 컨테이너
	
	CPlayState() { }
	void Init(CGameEngine* game);
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(CGameEngine* game);
	void Update(CGameEngine* game);
	void Draw(CGameEngine* game);
	void ScoreDraw(CGameEngine* game);
	void UpdateScore();

	void MapCreate(int A_Stage);  // 맵 생성
	inline void CheckColision();	//충돌 체크
	inline void AddScore(deque<CBlock>::iterator pos,int &score);		//점수 증가.
	inline void DrawItemBox();		//아이템 박스 그리기

};

#endif
