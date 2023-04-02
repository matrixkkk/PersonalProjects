#include "Player.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define YMAX 1700
#define XMAX 220
#define Xrange 200;

void CPlayer::Init( int type, std::string filename )
{
    SDL_Surface *temp =SDL_LoadBMP( filename.c_str()  );

    m_Image = SDL_DisplayFormat( temp );

    SDL_FreeSurface( temp );

    if( NULL != m_Image )
    {
        if( type == MAN )
            SDL_SetColorKey( m_Image, SDL_SRCCOLORKEY, SDL_MapRGB( m_Image->format, 0, 0, 0) );
        else if( type == WOMAN )
            SDL_SetColorKey( m_Image, SDL_SRCCOLORKEY, SDL_MapRGB( m_Image->format, 255, 0, 0) );
    }

    SDL_ShowCursor( false );

    m_bLeft             = m_bRight = m_bUp = m_bDown = false;
    bSwing              = false;
    frame               = 0;
    action              = STAND;
    previous_action     = STAND;
    state               = PLAYMODE;
    direction           = NODIRECTION;
    _state              = 0;
    frame_size          = type;

}

void CPlayer::Reset(int type)
{
    m_bLeft             = m_bRight = m_bUp = m_bDown = false;
    bSwing              = false;
    frame               = 0;
    action              = STAND;
    previous_action     = STAND;
    state               = PLAYMODE;
    direction           = NODIRECTION;
    _state              = 0;
    frame_size          = type;

}

void CPlayer::Release()
{
    SDL_FreeSurface( m_Image );

}
void CPlayer::SetPosition( int x, int y)
{
    m_Rect.x  = x;
    m_Rect.y  = y;

}


void CPlayer::Stand()
{
    if( action != STAND && state != DELAYMODE  && !bSwing )
    {
        bSwing   = false;
        action   = STAND;
        frame    = 0;
    }

}

void CPlayer::LeftWalk()
{

        if( action != LEFTMOVE && state != DELAYMODE  && !bSwing )
        {
            if( m_bUp == true )
                action = LEFTMOVE;
            if( m_bDown == true )
                action = LEFTMOVE;

            action = LEFTMOVE;
            frame = 0;
            bSwing = false;

            previous_action = LEFTMOVE;
        }

        if(  state != DELAYMODE )
            m_Rect.x -= m_Speed;
}

void CPlayer::RightWalk()
{
        if( action != RIGHTMOVE  && state != DELAYMODE  && !bSwing )
        {
            if( m_bUp == true );
                action = RIGHTMOVE;
            if( m_bDown == true );
                action = RIGHTMOVE;

            action = RIGHTMOVE;
            frame = 0;
            bSwing = false;

            previous_action = RIGHTMOVE;
        }

        if(  state != DELAYMODE )
            m_Rect.x += m_Speed;

}

void CPlayer::UpWalk()
{
    if( CurrentRule == BATTLEMODE)
    {
        if( action != UPMOVE && state != DELAYMODE  && !bSwing  )
        {

            if( m_bLeft == true )
                action = LEFTMOVE;
            else if( m_bRight == true )
                action = RIGHTMOVE;
            else
            {
                action = UPMOVE;
                frame = 0;
            }
            bSwing = false;
        }

        if( state != DELAYMODE && frame_size == WOMAN && m_Rect.y < 1500 )
            m_Rect.y += m_Speed;
        else if( state != DELAYMODE && frame_size == MAN )
            m_Rect.y += m_Speed;

    }
}

void CPlayer::DownWalk()
{
    if( CurrentRule == BATTLEMODE)
    {
        if( action != DOWNMOVE && state != DELAYMODE   && !bSwing  )
        {
            if( m_bLeft == true )
                action = LEFTMOVE;
            else if( m_bRight == true )
                action = RIGHTMOVE;
            else
            {
                action = DOWNMOVE;
                frame = 0;
            }
            bSwing = false;
        }

        if( state != DELAYMODE && frame_size == MAN && m_Rect.y > 1500 )
            m_Rect.y -= m_Speed;
        else if( state != DELAYMODE && frame_size == WOMAN )
            m_Rect.y -= m_Speed;

    }
}

void CPlayer::Diving( int x )
{
    if( CurrentRule == BATTLEMODE)
    {
        if( ( m_bLeft == true && m_bUp == true ) || m_bLeft == true )
        {
            //LEFT DIVING
            if( previous_action != LEFTDIVING && action != LEFTDIVING && state == PLAYMODE   &&  !bSwing )
            {
                action = LEFTDIVING;
                previous_action = LEFTDIVING;
                frame        = 0;
                m_Rect.x -= m_Speed * 6;
            }

           bSwing = true;
           SetState( DELAYMODE );
        }
        else if( ( m_bRight == true && m_bUp == true ) || m_bRight == true )
        {

            if( previous_action != RIGHTDIVING && action != RIGHTDIVING && state == PLAYMODE  &&  !bSwing  )
            {
                action = RIGHTDIVING;
                previous_action = RIGHTDIVING;
                frame        = 0;
                m_Rect.x    += m_Speed * 6;
            }

            bSwing = true;
            SetState( DELAYMODE );

        }

    }
}

void CPlayer::Swing( int x )
{

    if( CurrentRule == BATTLEMODE)
    {

        if( ( m_bLeft == true && m_bUp == true ) || m_bLeft == true )
        {
                if( previous_action != LEFTSWING && action != LEFTSWING && state != DELAYMODE &&  !bSwing  )
                {

                    if( x >  m_Rect.x + 75   )
                    {
                        action = RIGHTSWING;
                        previous_action = RIGHTSWING;
                    }
                    else
                    {
                        action = LEFTSWING;
                        previous_action = LEFTSWING;
                    }

                    frame  = 0;
             //       SetBoundBox( m_Rect.x, m_Rect.y );
                }

//                previous_action = LEFTSWING;
                bSwing = true;
                SetState( DELAYMODE );
        }
        else if( ( m_bRight == true && m_bUp == true ) || m_bRight == true )
        {

            if( previous_action != RIGHTSWING && action != RIGHTSWING && state != DELAYMODE  &&  !bSwing )
            {

                    if( x <  m_Rect.x + 75   )
                    {
                        action = LEFTSWING;
                        previous_action = LEFTSWING;
                    }
                    else
                    {
                        action = RIGHTSWING;
                        previous_action = RIGHTSWING;
                    }


                frame  = 0;
            //    SetBoundBox( m_Rect.x, m_Rect.y );
            }

//            previous_action = RIGHTSWING;
            bSwing = true;
            SetState( DELAYMODE );


        }
        else if( m_bUp == true && m_bLeft == false && m_bRight == false )
        {
            if( previous_action != SMASHING    && action != SMASHING && state != DELAYMODE && !bSwing )
            {
                action = SMASHING;
                frame = 0;
            }

            previous_action = SMASHING;
            bSwing = true;
            SetState( DELAYMODE );
        }
    }
    else if( CurrentRule == SURVEMODE)
    {
         if( action != SURVE && bSwing == false)
        {
            action = SURVE;
            frame    = 0;
         }
        else if( action == SURVE && bSwing == true)
        {
            previous_action  = SURVE;
            bSwing = false;
            frame    = 4;
        }
        SetState( DELAYMODE );

    }

}


void CPlayer::SetBoundBox( int x, int y)
{
    BoundBox.x = x-50;
    BoundBox.y = y-20;
    BoundBox.w = frame_size + 50 ;
    BoundBox.h = frame_size + 50 ;


    /*
    if( frame_size == MAN )
    {
        if( CurrentRule == BATTLEMODE )
            y += (frame_size / 3);
    }
    else if( frame_size == WOMAN )
    {
        if( CurrentRule == BATTLEMODE )
            y += (frame_size / 5);
    }


    if( action == LEFTSWING )
    {
        BoundBox.x = x - (frame_size / 2 );
        BoundBox.y = y;
        BoundBox.w = (frame_size + frame_size/2) ;
        BoundBox.h = (frame_size / 2 );

    }
    else if( action == RIGHTSWING )
    {
        x = x + (frame_size / 2 );

        BoundBox.x = x;
        BoundBox.y = y;
        BoundBox.w = ( frame_size + frame_size/2 );
        BoundBox.h = (frame_size / 2 );

    }
    else if( action == RIGHTDIVING )
    {
        x = x + (frame_size / 3 );

        BoundBox.x = x ;
        BoundBox.y = y ;
        BoundBox.w = frame_size+ (frame_size / 2);
        BoundBox.h = (frame_size );

    }
    else if( action == LEFTDIVING )
    {
        x = x - ( frame_size );

        BoundBox.x = x;
        BoundBox.y = y;
        BoundBox.w = 2*frame_size ;
        BoundBox.h = (frame_size  );

    }
    else
    {
       // x =  x + (frame_size / 2 );
        BoundBox.x = x;
        BoundBox.y = y;
        BoundBox.w = frame_size;//(frame_size / 2 );
        BoundBox.h = frame_size;//(frame_size / 2 );
    }
    */
}

void CPlayer::Update( CGameApp* game )
{
//    SetBoundBox( m_Rect.x, m_Rect.y );
    static int count = 0;
    static int Delay = 0;
    static int dcount=0;


    if( CurrentRule == BATTLEMODE )
    {
         if( previous_action != RIGHTDIVING  &&  previous_action != LEFTDIVING )
        {
            if ( 0 == ( count % SPRITEFRMAE_SPEED )  )
                frame = (frame + 1) % 8;
        }
        else if( previous_action == RIGHTDIVING || previous_action == LEFTDIVING  )
        {
            if ( 0 == ( count % SPRITEFRMAE_SPEED )  )
                frame = (frame + 1) % 8;
            /*
            if ( 0 == ( count % SPRITEFRMAE_SPEED )  )
            {
                dcount++;

                if( !( dcount %= 2 ) )
                    frame = (frame + 1) % 8;
            }*/
        }

    //������ ��忡���� �̵��� �Ұ����ϴ�.
        if( state == DELAYMODE )
        {
            if( previous_action == LEFTSWING || previous_action == RIGHTSWING )
            {
                if( 0 == ( count % SPRITEFRMAE_SPEED )  )
                    Delay++;

                if( Delay >= 8 )
                {
                    Delay                   = 0;
                    previous_action         = STAND;
                    bSwing                  = false;
                    state                   = PLAYMODE;
                    Stand();
                }
            }
            else if( previous_action == LEFTDIVING || previous_action == RIGHTDIVING )
            {

                if ( 0 == ( count % SPRITEFRMAE_SPEED )  )
                    Delay++;

                if( Delay >= 8 )
                {
                    Delay                  = 0;
                    previous_action  = STAND;
                    bSwing               = false;
                    state                   = PLAYMODE;
                    Stand();
                }

                if( previous_action == LEFTDIVING )
                    m_Rect.x -= m_Speed - 3;
                else if( previous_action == RIGHTDIVING )
                    m_Rect.x += m_Speed - 3;
            }
            else if( previous_action == SMASHING )      //���Ž�
            {
                if( 0 == ( count % SPRITEFRMAE_SPEED )  )
                    Delay++;

                if( Delay >= 8 )
                {
                    Delay                  = 0;
                    previous_action  = STAND;
                    bSwing               = false;
                    state                   = PLAYMODE;
                    Stand();
                }

                m_Rect.y -= 2;

            }
        }
    }
    else if( CurrentRule == SURVEMODE )
    {
        if( action != SURVE  )
        {
            if ( 0 == ( count % SPRITEFRMAE_SPEED )  )
                frame = (frame + 1) % 8;
        }
        else if( action == SURVE && previous_action != SURVE )
        {
            //MessageBox( NULL, "SDF", "SDF", NULL );
            if ( 0 == ( count % SPRITEFRMAE_SPEED*20 )  )
            {
                Delay++;

                if( Delay <= 8 )
                {
                    frame++;

                    if( frame >= 4 )    //������ 4������ �����Ͽ� ���긦 ĥ�� �ִ�.
                    {
                        frame = 3;
                        bSwing = true;
                    }

                }
                else if( Delay > 8 )  //������ 8 ���� ������ ���ϸ� �����·�
                {
                    frame-- ;

                    if( frame <= 0 )
                    {
                        Delay               = 0;
                        state               = PLAYMODE;
                        previous_action     = STAND;
                        bSwing              = false;
                        Stand();

                    }
                }
            }
        }
        else if(  previous_action == SURVE && bSwing == false )
        {

            if ( 0 == ( count % SPRITEFRMAE_SPEED )  )
            {
                frame++;
                if( frame >= 8 )
                {
                    Delay = 0;
                    state                   = PLAYMODE;
                    previous_action         = STAND;
                    bSwing                  = false;
                    SetRuleMode( BATTLEMODE );
                    Stand();
                    SetBoundBox( m_Rect.x, m_Rect.y );
                }
            }
        }
    }

    count++;

}

void CPlayer::Draw( CGameApp* game )
{
    SDL_Rect sheetRect, destRect;
    SDL_Rect playerRect;                      // ��ǥ ��ȯ �� ���� ����
    CPoint3 temp;                             // �ӽ� ���� ����
    temp=transCoordinate(m_Rect.x,m_Rect.y);  //��ǥ ��ȯ
    playerRect.x = temp.x;
    playerRect.y = temp.y;

    SetBoundBox( playerRect.x, playerRect.y);

    sheetRect.x  = frame  * frame_size;
    sheetRect.y  = action * frame_size;
    sheetRect.w  = frame_size;
    sheetRect.h  = frame_size;

    SDL_BlitSurface( m_Image, &sheetRect, game->getScreen(), &playerRect );

}

ball_physics CPlayer::Hit_Ball( int value,CPoint3 BallPos )
{
    ball_physics tmp;
    CPoint3 vs;
	double v,Arcsin;
	int x,y,xy,Width_degree;
	srand(time(NULL));

	if(BallPos.y<=0) BallPos.y=0;

	switch(value)
	{
        case USERRIGHTHIT:  //user ������ ����

            if( m_bLeft == true )
            {
                x=rand()%Xrange;
                y=YMAX;
				x*=-1;
            }
            else
            {
                x=m_Rect.x;
                y=YMAX;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

            tmp.height_degree=40;
			v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case USERLEFTHIT:
            if( m_bLeft == true )
            {
               x=m_Rect.x;
               y=YMAX;
            }
            else
            {
                x=rand()%Xrange;
                y=YMAX;
				x*=-1;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

			v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.height_degree=40;
			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case USERLEFTDIVEHIT:
            if( m_bLeft == true )
            {
               x=m_Rect.x;
               y=YMAX;
            }
            else
            {
                x=rand()%Xrange;
                y=YMAX;
				x*=-1;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

        	v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.height_degree=40;  //���̺� �г�Ƽ �����
			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case USERRIGHTDIVEHIT:      //���� ������ ���̺�
            if( m_bLeft == true )
            {
                x=rand()%Xrange;
                y=YMAX;
				x*=-1;
            }
            else
            {
                x=m_Rect.x;
                y=YMAX;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

            tmp.height_degree=40;  //���̺� �г�Ƽ
			v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case USERSMASH:
            if( m_bLeft == true )
            {
                x=rand()%Xrange;
                y=YMAX;
				x*=-1;
            }
            else
            {
                x=rand()%Xrange;
                y=YMAX;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

            tmp.height_degree=-15;

			tmp.width_degree=Width_degree;
			tmp.v=Km(150);
            break;

        case USERHITSERVE:  //����
			x=rand()%Xrange;
			y=1360;
			x*=-1;

			vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;

			tmp.height_degree=-15;
			tmp.width_degree=Width_degree;
			tmp.v=Km(150);
			break;
        case COMRIGHTHIT:  //com������ ����

            if( m_bLeft == true )
            {
                x=rand()%Xrange;
                y=100;
				x*=-1;
            }
            else
            {
                x=m_Rect.x;
                y=100;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

            tmp.height_degree=-40;
			v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case COMLEFTHIT:
            if( m_bLeft == true )
            {
               x=m_Rect.x;
               y=100;
            }
            else
            {
                x=rand()%Xrange;
                y=100;
				x*=-1;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

			v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.height_degree=-40;
			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case COMLEFTDIVEHIT:
            if( m_bLeft == true )
            {
               x=m_Rect.x;
               y=100;
            }
            else
            {
                x=rand()%Xrange;
                y=100;
				x*=-1;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

			v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.height_degree=-40;  //���̺� �г�Ƽ �����
			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case COMRIGHTDIVEHIT:      //���� ������ ���̺�
            if( m_bLeft == true )
            {
                x=rand()%Xrange;
                y=100;
				x*=-1;
            }
            else
            {
                x=m_Rect.x;
                y=100;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

            tmp.height_degree=-40;  //���̺� �г�Ƽ
			v=((y-BallPos.y)/25)/cos(R(40))*1.02f;

			tmp.width_degree=Width_degree;
			tmp.v=Km(v);
            break;
        case COMSMASH:
            if( m_bLeft == true )
            {
                x=rand()%Xrange;
                y=100;
				x*=-1;
            }
            else
            {
                x=rand()%240;
                y=100;
            }
            vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;
			if(m_bRight==true)      //���������� ġ�� ������ �ݴ� ����
                Width_degree*=-1;

            tmp.height_degree=15;

			tmp.width_degree=Width_degree;
			tmp.v=Km(-150);
            break;

        case COMHITSERVE:  //����
			x=rand()%200;
			y=585;
			x*=-1;

			vs.x=x-BallPos.x;
			vs.y=y-BallPos.y;

			xy=sqrt(vs.x*vs.x+vs.y*vs.y);
			Arcsin=asin(vs.y/xy);

			Width_degree=180/3.141592f*Arcsin;

			tmp.height_degree=15;
			tmp.width_degree=-Width_degree;
			tmp.v=Km(-150);
			break;
	}
	return tmp;

}

void CPlayer::CheckDirection()
{
    if( m_bLeft    == true )
		LeftWalk();
    if( m_bRight  == true )
		RightWalk();
	if(m_bUp       == true )
		UpWalk();
	if(m_bDown  == true )
		DownWalk();

}
