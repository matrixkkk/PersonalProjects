
#include "gamecontrol.h"
#include "windows.h"


Cgameplaycontrol::Cgameplaycontrol()	//������
{
    //////��Ʈ ���� �ʱ�ȭ////
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

	GameState=1;	            //������ ���� �ʱ�ȭ , 1=user ������ 2=com ������ 0=default
	UserPlayerGameScore=0;	    //���� �÷��̾� ���� ���ھ�
	UserPlayerSetScore=0;		//���� �÷��̾� ��Ʈ ���ھ�
	ComPlayerGameScore=0;		//com �÷��̾� ���� ���ھ�
	ComPlayerSetScore=0;		//com �÷��̾� ��Ʈ ���ھ�
	UserPlayerPointScore = 0;
	ComPlayerPointScore = 0;

	Fault=0;
	Serve=0;					//user ����� ���� �ʱ�ȭ
	TimerSignal=0;				//0 :�۵� x
	DelayTimer=0;				//�ñ׳� Ÿ�̸� �ʱ�ȭ.
	net = false;
	eventMessage = 3;
}

void Cgameplaycontrol::ServeInit(CBall *ball,CPlayer *player)	//���� �ʱ�ȭ
{
	SDL_Rect tmp_pl;

	tmp_pl=player->getRect();
	ball_physics tmp={Km(80),90,90};	//90,90 �������� ����
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

void Cgameplaycontrol::CheckGameState(CBall *ball,CPlayer *user,CPlayer *com) //���ӻ��� üũ
{
	CPoint3 BallPos;
	BallPos=ball->GetBallPos(); //���� ��ġ ����

	if(TimerSignal!=0) //Ÿ�̸� �߻�
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
		if(DelayTimer>=30) //���ͷ�Ʈ ������
		{
			DelayTimer=0;
			StartTimer(ball,user,com);	//���ͷ�Ʈ ����
			TimerSignal=0;
		}
	}
	if(GameState==0) //���� ���°� ���� ����.
	{
		ball->ball_movement();	//�� �̵�
	}
	else if(GameState==1) //������ ���°� user ���� �غ�����
	{
		ServeInit(ball,user); //���� �ʱ�ȭ
		if(user->Getaction()==10) //��Ʈ��ũ ����
		{
			GameState=0;
		}
	}
	else if(GameState==2) //������ ���°� com ���� ����
	{
		ServeInit(ball,com); //���� �ʱ�ȭ
		com->Swing(0);
		if(com->Getaction()==10) //��Ʈ��ũ ����
		{
			GameState=0;
		}
	}

	///////////////////////////////////
	///////////////////////////////////
	///1p�� ģ ���� ��Ʈ�� �ɷ��� ��///
	//////////////////////////////////
	if(ball->GetBallDir()==1 && BallPos.y>=1035 && net==false)
	{

		if( check_collision(ball->GetBallBounceBox(),NET)==true && BallPos.z<30)// 1034 : ��Ʈ�� ��ġ
		{
			net=true;
			ball_physics tmp;
			tmp.v=-10;
			tmp.height_degree=90;
			tmp.width_degree=90;
			ball->get_v(tmp);

			if(ball->GetServiceState()==true) //���� ���� �϶�
			{
				if(Fault==0)
				{
					printf("1p Fault!\n");
					Fault=1; //��Ʈ ���·� �ٲٰ� ����
					TimerSignal=IT_USER_FAULT; //user fault �߻�

				}
				else if(Fault==1)
				{
					printf("1p Double Fault!\n");
					Fault=0; //���� ��Ʈ �߻�
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
	///2p�� ģ ���� ��Ʈ�� �ɷ��� ��//
	//////////////////////////////////
	if(ball->GetBallDir()==2 && BallPos.y<=1035 && net==false)// 1035 : ��Ʈ�� ��ġ
	{
		if( check_collision(ball->GetBallBounceBox(),NET)==true && BallPos.z<30)// 1034 : ��Ʈ�� ��ġ
		{
			ball_physics tmp;
			tmp.v=-10;
			tmp.height_degree=90;
			tmp.width_degree=90;
			ball->get_v(tmp);

			if(ball->GetServiceState()==true) //���� ���� �϶�
			{
				if(Fault==0)
				{
					printf("2p Fault!\n");
					Fault=1; //��Ʈ ���·� �ٲٰ� ����
					TimerSignal=IT_COM_FAULT; //user fault �߻�

				}
				else if(Fault==1)
				{

					printf("2p Double Fault!\n");
					Fault=0; //���� ��Ʈ �߻�
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

	if(TimerSignal==0 && BallPos.z<=0) //�ٴڿ� �ٿ�尡 �� ��
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
			else //��Ʈ �ٱ���
			{
			    if(ball->GetBounce()>=2) //1�̻��� �ٿ��� ��Ʈ �ٱ��ʿ� �������� ��
				{
					TimerSignal=IT_USER_WIN; //���� �¸�

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
			else //��Ʈ �ٱ���
			{
			    if(ball->GetBounce()>=2) //1�̻��� �ٿ��� ��Ʈ �ٱ��ʿ� �������� ��
				{
					TimerSignal=IT_COM_WIN; //���� �¸�


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


void Cgameplaycontrol::PlusComPoint()		//Com�� ����Ʈ�� �÷��ִ� �Լ�.
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
			if(UserPlayerPointScore==40)  //user�� 40�� �� �� �� ���� �� ��
			{
				 if(AdUserplayer==true)
					AdUserplayer=false; //reduece
				 else if(AdComplayer==true)  //Advantage com �� ��
				 {
					 ComPlayerGameScore++;  //Com game
                     ComPlayerPointScore=0;
                     UserPlayerPointScore=0;
                     if(Serve) Serve=0;
                     else Serve=1;   //����� ����



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
                else Serve=1;   //����� ����

			}
	}
	printf("ComPlayerPointScore = %d\n",ComPlayerPointScore);
	printf("ComPlayerGameScore = %d\n",ComPlayerGameScore);

	/////////////Com player ���� ���//////////////////
	if(ComPlayerGameScore==6)
	{
		ComPlayerSetScore++;
		ComPlayerGameScore=0;
		UserPlayerGameScore=0;
	}
}

void Cgameplaycontrol::PlusUserPoint()		//user�� ����Ʈ�� �÷��ִ� �Լ�.
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
			if(ComPlayerPointScore==40)  //com�� 40�� �� �� �� ���� �� ��
			{
				 if(AdComplayer==true)
					AdComplayer=false; //reduece
				 else if(AdUserplayer==true)  //Advantage user �� ��
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


	/////////////User player ���� ���//////////////////
	if(UserPlayerGameScore==6)   //game score�� 6���� �Ǹ� ��Ʈ ���ھ 1 �ø���.
	{
		UserPlayerSetScore++;
		UserPlayerGameScore=0;
		ComPlayerGameScore=0;
	}
}


void Cgameplaycontrol::Reset(CBall *ball,CPlayer *user,CPlayer *com)//����
{
	user->SetPosition(250,320); //�ʱ� ��ġ�� ũ��.
	user->Reset(WOMAN);
	user->SetRuleMode(SURVEMODE);

	com->SetPosition(-250,2000);
	com->Reset(MAN);
	com->SetRuleMode(BATTLEMODE);
	ball->Clear();
	eventMessage = 3;

	net=false;

	if(Serve==0)  //user�� ���� �� �� ��
	{
		GameState=1;		  //���� ���� ����
		ServeInit(ball,user); //	���� �ʱ�ȭ
		user->SetRuleMode(SURVEMODE);
		com->SetRuleMode(BATTLEMODE);
	}
	else		  //com
	{
		GameState=2;		//���� ���� ����
		ServeInit(ball,com);
		com->SetRuleMode(SURVEMODE);
		user->SetRuleMode(BATTLEMODE);
	}
}

void Cgameplaycontrol::StartTimer(CBall *ball,CPlayer *user,CPlayer *com)
{
	//timer ������..
	switch(TimerSignal)
	{
	case 1: //user�� 1���� ��.
		PlusUserPoint();

		Reset(ball,user,com);

		break;
	case 2: //com�� 1���� ��.
		PlusComPoint();

		Reset(ball,user,com);
		break;
	case 3: //user ���� fault
		Reset(ball,user,com);

		break;
	case 4: //user ���� double fault
        PlusComPoint();
		Reset(ball,user,com);

		break;
	case 5: //com ���� fault
		Reset(ball,user,com);

		break;
	case 6: //com ���� double fault
        PlusUserPoint();
		Reset(ball,user,com);

        break;
	}
}