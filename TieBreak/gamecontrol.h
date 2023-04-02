#ifndef _GAMECONTROL_H_
#define _GAMECONTROL_H_

#include "common.h"
#include "ball.h"
#include "Player.h"
//#include "sprite.h"


/////////////////////////////////
////////���� ��Ģ ������/////////
/////////////////////////////////
/// 3������ 1��Ʈ ,


class Cgameplaycontrol// �״Ͻ� ������ ��Ʈ�� �ϴ� ��ü
{
private:
	int GameState;	//������ ���� , 1=user ������ 2=com ������  0=������
	int UserPlayerGameScore;	//���� �÷��̾� ���� ���ھ�
	int UserPlayerSetScore;		//���� �÷��̾� ��Ʈ ���ھ�
	int UserPlayerPointScore;	//���� �÷��̾� ����Ʈ ���ھ�
	int ComPlayerGameScore;		//com �÷��̾� ���� ���ھ�
	int ComPlayerSetScore;		//com �÷��̾� ��Ʈ ���ھ�
	int ComPlayerPointScore;    //com �÷��̾� ����Ʈ ���ھ�
	bool AdUserplayer;          //Advantage user;
	bool AdComplayer;			//Advantage com;
	int Fault;
	int Serve;					//����� 0: user 1:com
	int TimerSignal;			//TimerSignal ����
	int DelayTimer;				//Timer������ Ÿ��
	int eventMessage;
	bool net;
	int UserSp;
	int ComSp;
	///////��Ʈ�� �԰�////////////////
	SDL_Rect DownCenterMark,UpCenterMark;  // �� �Ʒ� CenterMark
	SDL_Rect DownLeftServe,UpLeftServe;	 // left serve
	SDL_Rect DownRightServe,UpRightServe;  // right serve
	SDL_Rect NET;
	//////////////////////////////////

public:
	Cgameplaycontrol::Cgameplaycontrol();	//���� �ʱ�ȭ ����
	void ServeInit(CBall *ball,CPlayer *player);	//���� �ʱ�ȭ ����
	int  GetState();		 //���� ���� ����
	void SetState(int value);     //���� ���� set
	void CheckGameState(CBall *ball,CPlayer *user,CPlayer *com);		//���� ���� üũ
	void PlusUserPoint();		  // user �� point�� �÷���.
	void PlusComPoint();		  //  com �� point������ �÷���.
	void Reset(CBall *ball,CPlayer *user,CPlayer *com);				//�״Ͻ����� ����
	int  GetComScore(){ return  ComPlayerPointScore;    }
	int  GetUserScore(){ return UserPlayerPointScore;   }


	int  GetEventMessage(){ return eventMessage; }
	void StartTimer(CBall *ball,CPlayer *user,CPlayer *com);	//Ÿ�̸Ӹ� start �Ѵ�. timer�� start�Ǹ�
																//�� �� �Ŀ� �־��� signal�� �ʱ�ȭ�� �ȴ�.
};

#endif
