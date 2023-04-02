#include <SDL/SDL.h>
#include "GameApp.h"
#include "gamestate.h"
#include "introstate.h"
#include "playstate.h"
#include "ball.h"



CBall::CBall()
{
    SDL_Surface* temp = SDL_LoadBMP("BMP/ball.bmp");
  image = SDL_DisplayFormat(temp);
  temp=SDL_LoadBMP("BMP/shadow.bmp");
  shadow = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  Uint32 colorkey = SDL_MapRGB(image->format, 255, 0, 0);  //�÷�Ű ����
  SDL_SetColorKey(image, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey);

  colorkey = SDL_MapRGB(shadow->format, 255, 0, 0);  //�÷�Ű ����
  SDL_SetColorKey(shadow, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey);
  Skill=false;

}

CBall::~CBall()
{
  SDL_FreeSurface(image);
  SDL_FreeSurface(shadow);
}

void CBall::init(int x,int y,int z)
{
	bool Inbound=false;           //�ٿ�� ���κ

	serveok=false;      //true : ������
	Service=false;
	time=0;				//Ÿ�� �ʱ�ȭ
	BallDir=0;		//���� �ʱ�ȭ
	original_spot.x=x;  //��ġ �ʱ�ȭ
	original_spot.y=y;
	original_spot.z=z;
	quant.v=0;			//�ӵ� �ʱ�ȭ
	quant.height_degree=0;
	quant.width_degree=0;
	r=7;				// ������
	shadow_position=ball_position=original_spot; //�׸��ڿ� ���� ��ġ�� ó�� ��ġ�� �ʱ�ȭ.
}

void CBall::ball_movement()
{
	float temp;
	time+=0.1f; //�ð� ��ȭ��


	shadow_vector.y=quant.v*cos(R(quant.height_degree))*time;					 //�׸����� y ����
	shadow_position.y=original_spot.y+shadow_vector.y*FPM;		 //y ���� ��ŭ y��ġ �̵�
	shadow_vector.z=(quant.v*sin(R(quant.height_degree))*time)-(0.5f*9.8f*time*time); //�׸����� z ���� (��� �׸��ڴ� ���̰� �����Ƿ� ���� ���̳� �ٸ�����)
	temp=shadow_position.z;
	shadow_position.z=original_spot.z+shadow_vector.z*5;	     //z
	shadow_vector.x=shadow_vector.y/tan(R(-quant.width_degree));	     //x ����
	shadow_position.x=original_spot.x+shadow_vector.x*FPM;		 //���� ��ŭ x�� ��ġ �̵�

	///////////////�ְ� ������ �϶��� ���� z///////////
	if(shadow_position.z-temp<=0)
		serveok=true;          //�̶� ���� ���갡 ��������
    if(Skill) //��ų �ߵ���
    {
       SkillPos=shadow_position;
       float tmp=shadow_vector.y/tan(R(quant.width_degree));	     //x ����
	   SkillPos.x=original_spot.x+tmp*FPM;

    }
	////////////////////////////////////
	///���� z�� ����� ���� ///////////
	//////////////////////////////////
	if(shadow_position.z<0)           //���̰� 0���� ������.
	{

		shadow_position.z=0;		  //1���� ����
		quant.v=quant.v*0.7;					  // �ӵ� ����
		original_spot=ball_position;  //���� �ٽ� ����� ���� �ʱ� ��ġ�� ���� ���� (z=0)�� ���� ����
		time=0.2f;						  //�ð��� �ʱ�ȭ

		if(quant.height_degree<=10 && quant.v>0)    // �ٿ�� ������ �ݴ��.
		{
			quant.height_degree*=-1;
		}
		else if(quant.height_degree>=10 && quant.v<0)
		{
		    quant.height_degree*=-1;
		}
	}
	ball_position.x=shadow_position.x;				//���� ��ġ�� �׸����� ��ġ�� ����
	ball_position.y=shadow_position.y;
	ball_position.z=shadow_position.z;

	//////////// ball ���� ///////////
	ball.x=ball_position.x-r;
	ball.y=ball_position.y-r;

	ball.w=r;
	ball.h=r;

}

void CBall::draw()
{
  SDL_Surface* screen = SDL_GetVideoSurface();

  CPoint3 temp; //�ӽ� ���� ����
  SDL_Rect ball_rect;   //�׸��� ���� ������
  SDL_Rect shadow_rect;

  temp=transCoordinate(shadow_position.x-r,shadow_position.y-r);
  shadow_rect.x=temp.x;
  shadow_rect.y=temp.y;
  shadow_rect.w=7;
  shadow_rect.h=7;
  ball_Bound=shadow_rect;
  SDL_BlitSurface(shadow, NULL, screen, &shadow_rect); //�׸��� �׸���

  temp=transCoordinate(ball.x,ball.y); //���� ->������ ��ǥ ��ȯ

  ball_rect.x=temp.x;
  ball_rect.y=temp.y-(ball_position.z);
  ball_rect.w=7;
  ball_rect.h=7;
  SDL_BlitSurface(image, NULL, screen, &ball_rect);   //�� �׸���
  if(Skill)
  {
      SkillPos=transCoordinate(SkillPos.x,SkillPos.y);
      SDL_Rect skillBall;
      skillBall.x=SkillPos.x;
      skillBall.y=SkillPos.y;
      SDL_BlitSurface(image,NULL,screen,&skillBall);
  }
}
void CBall::get_v(ball_physics fm)
{
	this->quant.v=fm.v;					    	//�ӵ�
	this->quant.width_degree=fm.width_degree;    //����
	this->quant.height_degree=fm.height_degree;  //������
	original_spot=ball_position;
	time=0.2f; //�ð� �ʱ�ȭ
}

void CBall::Ball_collision(CPlayer *user,CPlayer *com, int state )
{
	ball_physics temp;		//�ӽ÷� ���� �������� �����ϴ� ����
	if(serveok==true && ball_position.z<=150)	//���� ���� �޾� �����̰� ���� ���
	{
		if(check_collision(ball_Bound,user->getBoundingBox()) && BallDir!=1)	//user�Ϳ� �浹 check
		{
		    if(user->Getaction()!=0 && user->Getaction()!=1)
		    {
		        if(Skill) Skill=false;
		    }
			if(user->Getaction()==5) //Right�
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERRIGHTHIT,ball_position); //user ��ü�κ��� ���� �������� ����
				get_v(temp);
				BounceReset();//�ٿ �ʱ�ȭ.
			}
			else if(user->Getaction()==6) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERLEFTHIT,ball_position); //user ��ü�κ��� ���� �������� ����
				get_v(temp);
				BounceReset();//�ٿ �ʱ�ȭ.
			}
			else if(user->Getaction()==7) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=1;
				temp=user->Hit_Ball(USERLEFTDIVEHIT,ball_position); //user ��ü�κ��� ���� �������� ����
				get_v(temp);
				BounceReset();//�ٿ �ʱ�ȭ.
			}
			else if(user->Getaction()==8) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=1;
				temp=user->Hit_Ball(USERRIGHTDIVEHIT,ball_position); //user ��ü�κ��� ���� �������� ����
				get_v(temp);
				BounceReset();//�ٿ �ʱ�ȭ.
			}
			else if(user->Getaction()==9) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERSMASH,ball_position); //user ��ü�κ��� ���� �������� ����
				get_v(temp);
				BounceReset();//�ٿ �ʱ�ȭ.
			}
			else if(user->Getaction()==10 && user->GetbSwing()==false) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERHITSERVE,ball_position); //user ��ü�κ��� ���� �������� ����
				get_v(temp);
				BounceReset();//�ٿ �ʱ�ȭ.
			}
        }
		else if(check_collision(ball_Bound,com->getBoundingBox()) && BallDir!=2)
		{
		    if(com->Getaction()!=0 && com->Getaction()!=1)
		    {
		        if(Skill) Skill=false;
		    }
			if(com->Getaction()==5) //�ӽ÷�
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMRIGHTHIT,ball_position);	// COMHIT�� ������ com�� ��Ÿ���� �������̴�.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==6) //�ӽ÷�
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMLEFTHIT,ball_position);	// COMHIT�� ������ com�� ��Ÿ���� �������̴�.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==7) //�ӽ÷�
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=2;
				temp=com->Hit_Ball(COMLEFTDIVEHIT,ball_position);	// COMHIT�� ������ com�� ��Ÿ���� �������̴�.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==8) //�ӽ÷�
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=2;
				temp=com->Hit_Ball(COMRIGHTDIVEHIT,ball_position);	// COMHIT�� ������ com�� ��Ÿ���� �������̴�.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==9) //�ӽ÷�
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMSMASH,ball_position);	// COMHIT�� ������ com�� ��Ÿ���� �������̴�.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==10 && com->GetbSwing()==false) //�ӽ÷�
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMHITSERVE,ball_position);	// COMHIT�� ������ com�� ��Ÿ���� �������̴�.
				get_v(temp);
				BounceReset();
			}
		}
	}
}



