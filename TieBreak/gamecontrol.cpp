
#include "gamecontrol.h"
#include "windows.h"


Cgameplaycontrol::Cgameplaycontrol()	//생성자
{
    //////코트 영역 초기화////
    SetSDLRect(&DownCenterMark,-280,20,560,500);
    SetSDLRect(&UpCenterMark,-280,1380,560,320);
    SetSDLRect(&DownLeftServe,-280,585,280,450);
    SetSDLRect(&UpLeftServe,-280,1035,280,345);
    SetSDLRect(&DownRightServe,0,585,280,450);
    SetSDLRect(&UpRightServe,0,1035,280,345);
    SetSDLRect(&NET,-365,1065,730,100);
    //////////////////////////
    int UserSp=0;
	int ComSp=0;

	GameState=1;	            //게임의 상태 초기화 , 1=user 서브중 2=com 서브중 0=default
	UserPlayerGameScore=0;	    //유저 플레이어 게임 스코어
	UserPlayerSetScore=0;		//유저 플레이어 세트 스코어
	ComPlayerGameScore=0;		//com 플레이어 게임 스코어
	ComPlayerSetScore=0;		//com 플레이어 세트 스코어
	UserPlayerPointScore = 0;
	ComPlayerPointScore = 0;

	Fault=0;
	Serve=0;					//user 서브로 시작 초기화
	TimerSignal=0;				//0 :작동 x
	DelayTimer=0;				//시그널 타이머 초기화.
	net = false;
	eventMessage = 3;
}

void Cgameplaycontrol::ServeInit(CBall *ball,CPlayer *player)	//서비스 초기화
{
	SDL_Rect tmp_pl;

	tmp_pl=player->getRect();
	ball_physics tmp={Km(80),90,90};	//90,90 수직으로 던짐
	if(GameState==1) ball->init(tmp_pl.x+75,tmp_pl.y-200,0);
	else if(GameState==2) ball->init(tmp_pl.x+30,tmp_pl.y-120,0);
	ball->get_v(tmp);
	ball->SetService(true);
	eventMessage = 3;
}
int Cgameplaycontrol::GetState()
{
	return GameState;
}

void Cgameplaycontrol::SetState(int value)
{
	GameState=value;
}

void Cgameplaycontrol::CheckGameState(CBall *ball,CPlayer *user,CPlayer *com) //게임상태 체크
{
	CPoint3 BallPos;
	BallPos=ball->GetBallPos(); //볼의 위치 얻어옴

	if(TimerSignal!=0) //타이머 발생
	{
		DelayTimer++;
		switch(TimerSignal)
		{
		    case 1:
		    eventMessage=1;
		    break;
            case 2:
            eventMessage=2;
            break;
            default:
            eventMessage=0;
            break;

		}
		if(DelayTimer>=30) //인터럽트 딜레이
		{
			DelayTimer=0;
			StartTimer(ball,user,com);	//인터럽트 실행
			TimerSignal=0;
		}
	}
	if(GameState==0) //게임 상태가 게임 중임.
	{
		ball->ball_movement();	//볼 이동
	}
	else if(GameState==1) //게임의 상태가 user 서브 준비중임
	{
		ServeInit(ball,user); //서브 초기화
		if(user->Getaction()==10) //스트로크 중임
		{
			GameState=0;
		}
	}
	else if(GameState==2) //게임의 상태가 com 서브 중임
	{
		ServeInit(ball,com); //서브 초기화
		com->Swing(0);
		if(com->Getaction()==10) //스트로크 중임
		{
			GameState=0;
		}
	}

	///////////////////////////////////
	///////////////////////////////////
	///1p가 친 공이 네트에 걸렸을 때///
	//////////////////////////////////
	if(ball->GetBallDir()==1 && BallPos.y>=1035 && net==false)
	{

		if( check_collision(ball->GetBallBounceBox(),NET)==true && BallPos.z<30)// 1034 : 네트의 위치
		{
			net=true;
			ball_physics tmp;
			tmp.v=-10;
			tmp.height_degree=90;
			tmp.width_degree=90;
			ball->get_v(tmp);

			if(ball->GetServiceState()==true) //서비스 상태 일때
			{
				if(Fault==0)
				{
					printf("1p Fault!\n");
					Fault=1; //폴트 상태로 바꾸고 리셋
					TimerSignal=IT_USER_FAULT; //user fault 발생

				}
				else if(Fault==1)
				{
					printf("1p Double Fault!\n");
					Fault=0; //더블 폴트 발생
					TimerSignal=IT_USER_DOUBLE_FAULT; //user double fault start occured.

				}
			}
			else
			{
				printf("Net 1p out!\n");
				TimerSignal=IT_COM_WIN;  //com win's signal occured
			}
		}
	}
	///////////////////////////////////
	///2p가 친 공이 네트에 걸렸을 때//
	//////////////////////////////////
	if(ball->GetBallDir()==2 && BallPos.y<=1035 && net==false)// 1035 : 네트의 위치
	{
		if( check_collision(ball->GetBallBounceBox(),NET)==true && BallPos.z<30)// 1034 : 네트의 위치
		{
			ball_physics tmp;
			tmp.v=-10;
			tmp.height_degree=90;
			tmp.width_degree=90;
			ball->get_v(tmp);

			if(ball->GetServiceState()==true) //서비스 상태 일때
			{
				if(Fault==0)
				{
					printf("2p Fault!\n");
					Fault=1; //폴트 상태로 바꾸고 리셋
					TimerSignal=IT_COM_FAULT; //user fault 발생

				}
				else if(Fault==1)
				{

					printf("2p Double Fault!\n");
					Fault=0; //더블 폴트 발생
					TimerSignal=IT_COM_DOUBLE_FAULT; //user double fault start occured.

				}
			}
			else
			{
				printf("Net 2p out!\n");
				TimerSignal=IT_USER_WIN;  //com win's signal occured
			}
		}
	}

	if(TimerSignal==0 && BallPos.z<=0) //바닥에 바운드가 될 때
	{
	    ball->SetBounce();
        if(ball->GetBallDir()==1)  //1p->2p
		{
		   if(check_collision(ball->GetBallBounceBox(),UpCenterMark)
				|| check_collision(ball->GetBallBounceBox(),UpLeftServe)
				|| check_collision(ball->GetBallBounceBox(),UpRightServe))
            {
                if(ball->GetBounce()>=2)
                {
                char ch[128];
                TimerSignal=IT_USER_WIN;
                }

			}
			else //코트 바깥쪽
			{
			    if(ball->GetBounce()>=2) //1이상의 바운드로 코트 바깥쪽에 떨어졌을 때
				{
					TimerSignal=IT_USER_WIN; //유저 승리

				}
				else
				{
                TimerSignal=IT_COM_WIN;
				}

			}
		}
		else if(ball->GetBallDir()==2)
		{
		    if(check_collision(ball->GetBallBounceBox(),DownCenterMark)
				|| check_collision(ball->GetBallBounceBox(),DownLeftServe)
				|| check_collision(ball->GetBallBounceBox(),DownRightServe))
            {
                    if(ball->GetBounce()>=2)
                    {
					TimerSignal=IT_COM_WIN;

                    }

			}
			else //코트 바깥쪽
			{
			    if(ball->GetBounce()>=2) //1이상의 바운드로 코트 바깥쪽에 떨어졌을 때
				{
					TimerSignal=IT_COM_WIN; //유저 승리


				}
			    else{
                char ch[128];
			    sprintf(ch,"y = %f",BallPos.y);

                TimerSignal=IT_USER_WIN;

                }
			}

		}

	}

}


void Cgameplaycontrol::PlusComPoint()		//Com의 포인트를 올려주는 함수.
{
	switch(ComPlayerPointScore)
	{
		case 0:
			ComPlayerPointScore=15;
			break;
		case 15:
			ComPlayerPointScore=30;
			break;
		case 30:
			ComPlayerPointScore=40;
			break;
		case 40:
			if(UserPlayerPointScore==40)  //user도 40점 일 때 즉 동점 일 때
			{
				 if(AdUserplayer==true)
					AdUserplayer=false; //reduece
				 else if(AdComplayer==true)  //Advantage com 일 때
				 {
					 ComPlayerGameScore++;  //Com game
                     ComPlayerPointScore=0;
                     UserPlayerPointScore=0;
                     if(Serve) Serve=0;
                     else Serve=1;   //서브권 변경



				 }
				 else
					 AdComplayer=true;   //advantage com
			}
			else
			{
                ComPlayerGameScore++;  //Com game
                ComPlayerPointScore=0;
                UserPlayerPointScore=0;

                if(Serve) Serve=0;
                else Serve=1;   //서브권 변경

			}
	}
	printf("ComPlayerPointScore = %d\n",ComPlayerPointScore);
	printf("ComPlayerGameScore = %d\n",ComPlayerGameScore);

	/////////////Com player 점수 결산//////////////////
	if(ComPlayerGameScore==6)
	{
		ComPlayerSetScore++;
		ComPlayerGameScore=0;
		UserPlayerGameScore=0;
	}
}

void Cgameplaycontrol::PlusUserPoint()		//user의 포인트를 올려주는 함수.
{
	switch(UserPlayerPointScore)
	{
		case 0:
			UserPlayerPointScore=15;
			break;
		case 15:
			UserPlayerPointScore=30;
			break;
		case 30:
			UserPlayerPointScore=40;
			break;
		case 40:
			if(ComPlayerPointScore==40)  //com도 40점 일 때 즉 동점 일 때
			{
				 if(AdComplayer==true)
					AdComplayer=false; //reduece
				 else if(AdUserplayer==true)  //Advantage user 일 때
				 {
					 UserPlayerGameScore++; //user game
                     ComPlayerPointScore=0;
                     UserPlayerPointScore=0;
                      if(Serve) Serve=0;
                     else Serve=1;


				 }
				 else
					 AdUserplayer=true;   //advantage user
			}
			else
			{
				 UserPlayerGameScore++; //user game
				 UserPlayerPointScore=0;
				 ComPlayerPointScore=0;
				 if(Serve) Serve=0;
                 else Serve=1;

			}
	}
	printf("UserPlayerPointScore = %d\n",UserPlayerPointScore);
	printf("UserPlayerGameScore = %d\n",UserPlayerGameScore);


	/////////////User player 점수 결산//////////////////
	if(UserPlayerGameScore==6)   //game score가 6점이 되면 세트 스코어를 1 올린다.
	{
		UserPlayerSetScore++;
		UserPlayerGameScore=0;
		ComPlayerGameScore=0;
	}
}


void Cgameplaycontrol::Reset(CBall *ball,CPlayer *user,CPlayer *com)//리셋
{
	user->SetPosition(250,320); //초기 위치와 크기.
	user->Reset(WOMAN);
	user->SetRuleMode(SURVEMODE);

	com->SetPosition(-250,2000);
	com->Reset(MAN);
	com->SetRuleMode(BATTLEMODE);
	ball->Clear();
	eventMessage = 3;

	net=false;

	if(Serve==0)  //user의 서브 턴 일 때
	{
		GameState=1;		  //게임 상태 변경
		ServeInit(ball,user); //	서브 초기화
		user->SetRuleMode(SURVEMODE);
		com->SetRuleMode(BATTLEMODE);
	}
	else		  //com
	{
		GameState=2;		//게임 상태 변경
		ServeInit(ball,com);
		com->SetRuleMode(SURVEMODE);
		user->SetRuleMode(BATTLEMODE);
	}
}

void Cgameplaycontrol::StartTimer(CBall *ball,CPlayer *user,CPlayer *com)
{
	//timer 설정중..
	switch(TimerSignal)
	{
	case 1: //user가 1득점 함.
		PlusUserPoint();

		Reset(ball,user,com);

		break;
	case 2: //com가 1득점 함.
		PlusComPoint();

		Reset(ball,user,com);
		break;
	case 3: //user 서브 fault
		Reset(ball,user,com);

		break;
	case 4: //user 서브 double fault
        PlusComPoint();
		Reset(ball,user,com);

		break;
	case 5: //com 서브 fault
		Reset(ball,user,com);

		break;
	case 6: //com 서브 double fault
        PlusUserPoint();
		Reset(ball,user,com);

        break;
	}
}