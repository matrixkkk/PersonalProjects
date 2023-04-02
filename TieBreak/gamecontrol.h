#ifndef _GAMECONTROL_H_
#define _GAMECONTROL_H_

#include "common.h"
#include "ball.h"
#include "Player.h"
//#include "sprite.h"


/////////////////////////////////
////////게임 규칙 디자인/////////
/////////////////////////////////
/// 3게임이 1세트 ,


class Cgameplaycontrol// 테니스 게임을 컨트롤 하는 객체
{
private:
	int GameState;	//게임의 상태 , 1=user 서브중 2=com 서브중  0=게임중
	int UserPlayerGameScore;	//유저 플레이어 게임 스코어
	int UserPlayerSetScore;		//유저 플레이어 세트 스코어
	int UserPlayerPointScore;	//유저 플레이어 포인트 스코어
	int ComPlayerGameScore;		//com 플레이어 게임 스코어
	int ComPlayerSetScore;		//com 플레이어 세트 스코어
	int ComPlayerPointScore;    //com 플레이어 포인트 스코어
	bool AdUserplayer;          //Advantage user;
	bool AdComplayer;			//Advantage com;
	int Fault;
	int Serve;					//서브권 0: user 1:com
	int TimerSignal;			//TimerSignal 종류
	int DelayTimer;				//Timer딜레이 타임
	int eventMessage;
	bool net;
	int UserSp;
	int ComSp;
	///////코트의 규격////////////////
	SDL_Rect DownCenterMark,UpCenterMark;  // 위 아래 CenterMark
	SDL_Rect DownLeftServe,UpLeftServe;	 // left serve
	SDL_Rect DownRightServe,UpRightServe;  // right serve
	SDL_Rect NET;
	//////////////////////////////////

public:
	Cgameplaycontrol::Cgameplaycontrol();	//게임 초기화 설정
	void ServeInit(CBall *ball,CPlayer *player);	//서브 초기화 설정
	int  GetState();		 //게임 상태 리턴
	void SetState(int value);     //게임 상태 set
	void CheckGameState(CBall *ball,CPlayer *user,CPlayer *com);		//게임 상태 체크
	void PlusUserPoint();		  // user 의 point를 올려줌.
	void PlusComPoint();		  //  com 의 point점수를 올려줌.
	void Reset(CBall *ball,CPlayer *user,CPlayer *com);				//테니스게임 리셋
	int  GetComScore(){ return  ComPlayerPointScore;    }
	int  GetUserScore(){ return UserPlayerPointScore;   }


	int  GetEventMessage(){ return eventMessage; }
	void StartTimer(CBall *ball,CPlayer *user,CPlayer *com);	//타이머를 start 한다. timer가 start되면
																//몇 초 후에 주어진 signal로 초기화가 된다.
};

#endif
