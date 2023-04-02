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

  Uint32 colorkey = SDL_MapRGB(image->format, 255, 0, 0);  //ÄÃ·¯Å° °ËÁ¤
  SDL_SetColorKey(image, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey);

  colorkey = SDL_MapRGB(shadow->format, 255, 0, 0);  //ÄÃ·¯Å° °ËÁ¤
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
	bool Inbound=false;           //¹Ù¿îµå ¿©ºÎº

	serveok=false;      //true : ¼­ºêÁß
	Service=false;
	time=0;				//Å¸ÀÓ ÃÊ±âÈ­
	BallDir=0;		//Â÷·Ê ÃÊ±âÈ­
	original_spot.x=x;  //À§Ä¡ ÃÊ±âÈ­
	original_spot.y=y;
	original_spot.z=z;
	quant.v=0;			//¼Óµµ ÃÊ±âÈ­
	quant.height_degree=0;
	quant.width_degree=0;
	r=7;				// ¹İÁö¸§
	shadow_position=ball_position=original_spot; //±×¸²ÀÚ¿Í º¼ÀÇ À§Ä¡¸¦ Ã³À½ À§Ä¡·Î ÃÊ±âÈ­.
}

void CBall::ball_movement()
{
	float temp;
	time+=0.1f; //½Ã°£ º¯È­À²


	shadow_vector.y=quant.v*cos(R(quant.height_degree))*time;					 //±×¸²ÀÚÀÇ y º¤ÅÍ
	shadow_position.y=original_spot.y+shadow_vector.y*FPM;		 //y º¤ÅÍ ¸¸Å­ yÀ§Ä¡ ÀÌµ¿
	shadow_vector.z=(quant.v*sin(R(quant.height_degree))*time)-(0.5f*9.8f*time*time); //±×¸²ÀÚÀÇ z º¤ÅÍ (»ç½Ç ±×¸²ÀÚ´Â ³ôÀÌ°¡ ¾øÀ¸¹Ç·Î º¼ÀÇ ³ôÀÌ³ª ´Ù¸§¾øÀ½)
	temp=shadow_position.z;
	shadow_position.z=original_spot.z+shadow_vector.z*5;	     //z
	shadow_vector.x=shadow_vector.y/tan(R(-quant.width_degree));	     //x º¤ÅÍ
	shadow_position.x=original_spot.x+shadow_vector.x*FPM;		 //º¤ÅÍ ¸¸Å­ xÀÇ À§Ä¡ ÀÌµ¿

	///////////////ÃÖ°í ³ôÀÌÁ¡ ÀÏ¶§ÀÇ ³ôÀÌ z///////////
	if(shadow_position.z-temp<=0)
		serveok=true;          //ÀÌ¶§ ºÎÅÍ ¼­ºê°¡ °¡´ÉÇØÁü
    if(Skill) //½ºÅ³ ¹ßµ¿½Ã
    {
       SkillPos=shadow_position;
       float tmp=shadow_vector.y/tan(R(quant.width_degree));	     //x º¤ÅÍ
	   SkillPos.x=original_spot.x+tmp*FPM;

    }
	////////////////////////////////////
	///³ôÀÌ zÀÇ ¾ç¼ö°ª °íÁ¤ ///////////
	//////////////////////////////////
	if(shadow_position.z<0)           //³ôÀÌ°¡ 0º¸´Ù ÀÛÀ»¶§.
	{

		shadow_position.z=0;		  //1À¸·Î °íÁ¤
		quant.v=quant.v*0.7;					  // ¼Óµµ °¨¼Ò
		original_spot=ball_position;  //½ÄÀ» ´Ù½Ã ¼¼¿ì±â À§ÇØ ÃÊ±â À§Ä¡¸¦ ¶¥¿¡ ´êÀº (z=0)ÀÏ ¶§·Î º¯°æ
		time=0.2f;						  //½Ã°£ÀÇ ÃÊ±âÈ­

		if(quant.height_degree<=10 && quant.v>0)    // ¹Ù¿îµù °¢µµ¸¦ ¹İ´ë·Î.
		{
			quant.height_degree*=-1;
		}
		else if(quant.height_degree>=10 && quant.v<0)
		{
		    quant.height_degree*=-1;
		}
	}
	ball_position.x=shadow_position.x;				//º¼ÀÇ À§Ä¡¿¡ ±×¸²ÀÚÀÇ À§Ä¡¸¦ ´ëÀÔ
	ball_position.y=shadow_position.y;
	ball_position.z=shadow_position.z;

	//////////// ball ¼³Á¤ ///////////
	ball.x=ball_position.x-r;
	ball.y=ball_position.y-r;

	ball.w=r;
	ball.h=r;

}

void CBall::draw()
{
  SDL_Surface* screen = SDL_GetVideoSurface();

  CPoint3 temp; //ÀÓ½Ã ÀúÀå º¯¼ö
  SDL_Rect ball_rect;   //±×¸®±â À§ÇÑ º¯¼öµé
  SDL_Rect shadow_rect;

  temp=transCoordinate(shadow_position.x-r,shadow_position.y-r);
  shadow_rect.x=temp.x;
  shadow_rect.y=temp.y;
  shadow_rect.w=7;
  shadow_rect.h=7;
  ball_Bound=shadow_rect;
  SDL_BlitSurface(shadow, NULL, screen, &shadow_rect); //±×¸²ÀÚ ±×¸®±â

  temp=transCoordinate(ball.x,ball.y); //³í¸®Àû ->¹°¸®Àû ÁÂÇ¥ º¯È¯

  ball_rect.x=temp.x;
  ball_rect.y=temp.y-(ball_position.z);
  ball_rect.w=7;
  ball_rect.h=7;
  SDL_BlitSurface(image, NULL, screen, &ball_rect);   //º¼ ±×¸®±â
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
	this->quant.v=fm.v;					    	//¼Óµµ
	this->quant.width_degree=fm.width_degree;    //¼öÆò°¢
	this->quant.height_degree=fm.height_degree;  //¼öÁ÷°¢
	original_spot=ball_position;
	time=0.2f; //½Ã°£ ÃÊ±âÈ­
}

void CBall::Ball_collision(CPlayer *user,CPlayer *com, int state )
{
	ball_physics temp;		//ÀÓ½Ã·Î °øÀÇ ¹°¸®·®À» ÀúÀåÇÏ´Â º¯¼ö
	if(serveok==true && ball_position.z<=150)	//°øÀÌ ÈûÀ» ¹Ş¾Æ ¿òÁ÷ÀÌ°í ÀÖÀ» °æ¿ì
	{
		if(check_collision(ball_Bound,user->getBoundingBox()) && BallDir!=1)	//user¿Í¿ì Ãæµ¹ check
		{
		    if(user->Getaction()!=0 && user->Getaction()!=1)
		    {
		        if(Skill) Skill=false;
		    }
			if(user->Getaction()==5) //Rightê
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERRIGHTHIT,ball_position); //user °´Ã¼·ÎºÎÅÍ °øÀÇ ¹°¸®·®À» ¹ŞÀ½
				get_v(temp);
				BounceReset();//¹Ù¿î½º ÃÊ±âÈ­.
			}
			else if(user->Getaction()==6) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERLEFTHIT,ball_position); //user °´Ã¼·ÎºÎÅÍ °øÀÇ ¹°¸®·®À» ¹ŞÀ½
				get_v(temp);
				BounceReset();//¹Ù¿î½º ÃÊ±âÈ­.
			}
			else if(user->Getaction()==7) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=1;
				temp=user->Hit_Ball(USERLEFTDIVEHIT,ball_position); //user °´Ã¼·ÎºÎÅÍ °øÀÇ ¹°¸®·®À» ¹ŞÀ½
				get_v(temp);
				BounceReset();//¹Ù¿î½º ÃÊ±âÈ­.
			}
			else if(user->Getaction()==8) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=1;
				temp=user->Hit_Ball(USERRIGHTDIVEHIT,ball_position); //user °´Ã¼·ÎºÎÅÍ °øÀÇ ¹°¸®·®À» ¹ŞÀ½
				get_v(temp);
				BounceReset();//¹Ù¿î½º ÃÊ±âÈ­.
			}
			else if(user->Getaction()==9) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERSMASH,ball_position); //user °´Ã¼·ÎºÎÅÍ °øÀÇ ¹°¸®·®À» ¹ŞÀ½
				get_v(temp);
				BounceReset();//¹Ù¿î½º ÃÊ±âÈ­.
			}
			else if(user->Getaction()==10 && user->GetbSwing()==false) //lefty
			{
			    if(state==0 && Service==true && BallDir==2)
			    {
			        Service=false;
			    }
				BallDir=1;
				temp=user->Hit_Ball(USERHITSERVE,ball_position); //user °´Ã¼·ÎºÎÅÍ °øÀÇ ¹°¸®·®À» ¹ŞÀ½
				get_v(temp);
				BounceReset();//¹Ù¿î½º ÃÊ±âÈ­.
			}
        }
		else if(check_collision(ball_Bound,com->getBoundingBox()) && BallDir!=2)
		{
		    if(com->Getaction()!=0 && com->Getaction()!=1)
		    {
		        if(Skill) Skill=false;
		    }
			if(com->Getaction()==5) //ÀÓ½Ã·Î
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMRIGHTHIT,ball_position);	// COMHITÀº ±¸ºĞÀÚ comÀ» ³ªÅ¸³»´Â ±¸ºĞÀÚÀÌ´Ù.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==6) //ÀÓ½Ã·Î
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMLEFTHIT,ball_position);	// COMHITÀº ±¸ºĞÀÚ comÀ» ³ªÅ¸³»´Â ±¸ºĞÀÚÀÌ´Ù.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==7) //ÀÓ½Ã·Î
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=2;
				temp=com->Hit_Ball(COMLEFTDIVEHIT,ball_position);	// COMHITÀº ±¸ºĞÀÚ comÀ» ³ªÅ¸³»´Â ±¸ºĞÀÚÀÌ´Ù.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==8) //ÀÓ½Ã·Î
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
			    Skill=true;
				BallDir=2;
				temp=com->Hit_Ball(COMRIGHTDIVEHIT,ball_position);	// COMHITÀº ±¸ºĞÀÚ comÀ» ³ªÅ¸³»´Â ±¸ºĞÀÚÀÌ´Ù.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==9) //ÀÓ½Ã·Î
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMSMASH,ball_position);	// COMHITÀº ±¸ºĞÀÚ comÀ» ³ªÅ¸³»´Â ±¸ºĞÀÚÀÌ´Ù.
				get_v(temp);
				BounceReset();
			}
			else if(com->Getaction()==10 && com->GetbSwing()==false) //ÀÓ½Ã·Î
			{
			    if(state==0 && Service==true && BallDir==1)
			    {
			        Service=false;
			    }
				BallDir=2;
				temp=com->Hit_Ball(COMHITSERVE,ball_position);	// COMHITÀº ±¸ºĞÀÚ comÀ» ³ªÅ¸³»´Â ±¸ºĞÀÚÀÌ´Ù.
				get_v(temp);
				BounceReset();
			}
		}
	}
}



