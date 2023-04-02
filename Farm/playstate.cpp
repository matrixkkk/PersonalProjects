#include <SDL/SDL.h>
#include <iostream>

#include "gamestate.h"
#include "gameengine.h"
#include "introstate.h"
#include "playstate.h"
#include "collision.h"
#include "gameover.h"
#include <algorithm>


CPlayState playState;
extern CGameOver gameOver;

using std::cout;
using std::endl;


bool check_collision( CBlock &block, CBall &ball)
{
	int leftA,leftB;
	int rightA,rightB;
	int topA,topB;
	int bottomA,bottomB;
	SDL_Rect crossRect;		//������ �κ��� rect�� ���ϱ� ���� ����
	CPoint2 pos,p,vec;
	
	int degree;

	crossRect=block.GetRect();
	pos.x=crossRect.x+15;
	pos.y=crossRect.y+15;
	p=ball.GetPos();
	
	if(block.GetState()==false || block.Getbreak()==true)	//�̹� �ı��� ���� ��� �׳� false;
	{
		return false;
	}

	SDL_Rect A,B;
	A=block.GetRect();
	B=ball.GetRect();

	leftA=A.x;
	rightA=A.x+A.w;
	topA=A.y;
	bottomA=A.y+A.h;

	leftB=B.x;
	rightB=B.x+B.w;
	topB=B.y;
	bottomB=B.y+B.h;

	if(bottomA < topB) return false;
	if(topA > bottomB) return false;
	if(rightA < leftB) return false;
	if(leftA > rightB) return false;
	
	
	if(block.GetKind()==6)	//���� �Ⱥμ����� ���� �϶�
	{
		return true;
	}

	block.Setbreak(true);
	
	double tmp1=p.x-pos.x;
	double tmp2=p.y-pos.y;
	
	degree=atan2(tmp2,tmp1)* 180 / 3.14159265;
	
	vec=ball.GetVec();		//���� ���� ���͸� �����´�.
	
	if(degree > -135 && degree <= -45)			//���� ����
	{
		ball.InverseVec(CPoint2(1.0f,-1.0f));
	}
	else if(degree > -45 && degree <= 45) //������ ����
	{
		/*if(vec.y > 0)
			ball.InverseVec(CPoint2(-1.0f,1.0f));
		else
			ball.InverseVec(CPoint2(-1.0f,1.0f));*/
		ball.InverseVec(CPoint2(-1.0f,1.0f));

	}
	else if(degree > 45 && degree <= 135)		//�Ʒ� ����
	{
		ball.InverseVec(CPoint2(1.0f,-1.0f));
	}
	else if(degree > 135 || degree <= -135)	//���ʿ� ����
	{
		/*if(vec.y > 0)
			ball.InverseVec(CPoint2(-1.0f,1.0f));
		else
			ball.InverseVec(CPoint2(-1.0f,1.0f));*/
		ball.InverseVec(CPoint2(-1.0f,1.0f));
		
	}
			
	return true;
}
///////////////////
//���� ���� �浹�� ó���ϴ� ������.
template<typename T1,typename T2>
bool ball_collision( T1 &a, T2 &b)
{
	CPoint2 Nomal,center1,center2;
	
	if(collision(a.GetRect(),b.GetRect())==true && a.GetID()!=b.GetID())	//�� �ΰ��� �浹 �Ѱ��.
	{
		int ids;

		center1=a.GetPos();	//��1�� �߽��� ������
		center2=b.GetPos(); //��2�� �߽��� ������

		center1.x=center1.x-a.GetVec().x*a.GetSpeed();
		center1.y=center1.y-a.GetVec().y*a.GetSpeed();
		center2.x=center2.x-a.GetVec().x*a.GetSpeed();
		center2.y=center2.y-a.GetVec().y*a.GetSpeed();
		
		Nomal.x=center2.x-center1.x;
		Nomal.y=center2.y-center1.y;
		a.SetVector(Reflect_vector(Nomal,a.GetVec()));		//�ݻ纤�͸����ؼ� �� ���Ϳ� �����ش�.
		
		Nomal.x=center1.x-center2.x;
		Nomal.y=center1.y-center2.x;
		b.SetVector(Reflect_vector(Nomal,b.GetVec()));
		

		ids=rand()%1000000;
		a.SetID(ids);
		b.SetID(ids);

		return true;
	}
	
	return false;
}
/////�ı��� �� ã�Ƽ� ����� ������
bool check_break_block(CBlock &block)
{
	return !block.GetState();
}
/////�ٴ����� ������ ������ ����
bool check_item(Citem *item)
{
	if(item->GetRect().y > 600)
		return true;
	else
		return false;

}
////////////////////
//�� ���� ������
bool Ball_destroy(CBall &ball)
{
	int y;
	y=ball.GetPos().y;
	if(y > 800)
	{		
		return true;
	}

	return false;
}


void CPlayState::Init(CGameEngine* game) //�ʱ�ȭ
{
	srand(time(NULL));

	b_pause=0;
	ID_reset=0;
	score1=score2=0;
	laser_frame1=0;
	laser_frame2=0;
	SetSDLRect(r_Laser1,0,0,14,17);
	SetSDLRect(r_Laser2,0,0,14,17);

	ReachScore1=0,ReachScore2=0;
	//----------�̹��� ���� �ʱ�ȭ----------------------------------
	SDL_Surface* temp = SDL_LoadBMP("image/background.bmp"); //��� �׸� load
	bg = SDL_DisplayFormat(temp);
	//�� �̹��� �ε�
	temp=SDL_LoadBMP("image/block_sheet.bmp");
	s_block=SDL_DisplayFormat(temp);
	
	//-------------------�� ����-----------------------
	
	MapCreate(game->map);  // 0: �⺻,  1: ������,  2: ��Ʈ,  3: �����
			
	//-----------------Bar ����----------------------------
	player1= new CBar(CPoint2(LEFTLINE+5,550));
	player2= new CBar(CPoint2(RIGHTLINE+100,550));

	//-----------------��ֹ� ����-------------------------
	N_play = new CBar(CPoint2(LEFTLINE, 400));  // ��ֹ� �� ����

	//�̹����ε�
	temp = SDL_LoadBMP("image/bar_1p.bmp");		//bar �̹��� �ε�
	s_bar1 = SDL_DisplayFormat(temp);
	temp = SDL_LoadBMP("image/bar_2p.bmp");
	s_bar2 = SDL_DisplayFormat(temp);
	player1->Init(s_bar1);						//�ε��� �̹����� ǥ���� �����͸� ������ bar ��ü�� �Ѱ��ش�.
	player2->Init(s_bar2);	

	temp = SDL_LoadBMP("image/tuttle2.bmp");  // ��ֹ� �̹��� �ε�
	s_Nbar = SDL_DisplayFormat(temp);
	N_play->Init(s_Nbar);

	//----------------Ball ����----------------------------
	temp = SDL_LoadBMP("image/ball_red.bmp");		//ball �̹��� �ε�
	s_ball1=SDL_DisplayFormat(temp);
	Ball1.push_back(CBall(CPoint2(LEFTLINE+55,540),s_ball1));	//ball �����̳ʿ� 1���� �־��ش�. ó������ ��1���� �����ϹǷ�
	
	temp = SDL_LoadBMP("image/ball_blue.bmp");		//ball �̹��� �ε�
	s_ball2=SDL_DisplayFormat(temp);
	Ball2.push_back(CBall(CPoint2(RIGHTLINE+155,540),s_ball2));
	
	//�� �ı� �ִϸ��̼� ����
	temp = SDL_LoadBMP("image/block_broken.bmp");
	s_block_ani=SDL_DisplayFormat(temp);
	SDL_SetColorKey( s_block_ani, SDL_SRCCOLORKEY, SDL_MapRGB( s_block_ani->format, 133, 176, 182) );

	//���ھ� ��Ʈ �̹��� �ε�
	temp = SDL_LoadBMP("image/number_sheet.bmp");
	s_score= SDL_DisplayFormat(temp);
	SDL_SetColorKey( s_score, SDL_SRCCOLORKEY, SDL_MapRGB( s_score->format, 0, 0, 0) );

	//������ ��Ʈ �̹��� �ε�
	temp = SDL_LoadBMP("image/item_sheet.bmp");
	item_sheet=SDL_DisplayFormat(temp);
	SDL_SetColorKey(item_sheet , SDL_SRCCOLORKEY, SDL_MapRGB( item_sheet->format, 0, 255, 255) );
	//������ ��Ʈ �̹��� �ε�
	temp = SDL_LoadBMP("image/laser_sheet.bmp");
	s_Laser=SDL_DisplayFormat(temp);
	SDL_SetColorKey(s_Laser , SDL_SRCCOLORKEY, SDL_MapRGB( s_Laser->format, 255, 0, 0) );
	//���� �ε�
	m_Bgm = Mix_LoadMUS("music/main.mid");
	if(m_Bgm==NULL)
	{
		cout<<"���� �ε� ����"<<endl;
	}
	

	//���� ���
	Mix_PlayMusic( m_Bgm, -1 );
	//ȿ���� �ε�
	cow=Mix_LoadWAV("music/cow.wav");
	dog=Mix_LoadWAV("music/dog.wav");
	sheep=Mix_LoadWAV("music/sheep.wav");
	lion=Mix_LoadWAV("music/lion.wav");
	dak=Mix_LoadWAV("music/dak.wav");
	pig=Mix_LoadWAV("music/cat.wav");

	SDL_FreeSurface(temp);	
}

void CPlayState::Cleanup()  //�Ҹ�
{ 
  SDL_FreeSurface(bg);
  SDL_FreeSurface(s_block);
  SDL_FreeSurface(s_bar1);
  SDL_FreeSurface(s_bar2);
  SDL_FreeSurface(s_ball1);
  SDL_FreeSurface(s_ball2);
  SDL_FreeSurface(s_block_ani);
  SDL_FreeSurface(item_sheet);
  SDL_FreeSurface(s_Laser);


  Mix_HaltMusic();  
  Mix_FreeMusic(m_Bgm);
	
  Mix_FreeChunk(cow);
  Mix_FreeChunk(dog);
  Mix_FreeChunk(lion);
  Mix_FreeChunk(dak);
  Mix_FreeChunk(pig);
  Mix_FreeChunk(sheep);

  Mix_FreeChunk(ball_high);
  Mix_FreeChunk(ball_law);
  Mix_FreeChunk(ball_spup);
  Mix_FreeChunk(ball_spdown);   
  Mix_FreeChunk(ball_stop);       
  Mix_FreeChunk(ball_hide);
  Mix_FreeChunk(key_revers);
  Mix_FreeChunk(ball_saperate);
  Mix_FreeChunk(ball_die);
  Mix_FreeChunk(ball_point);
  Mix_FreeChunk(key_raiser);
  Mix_FreeChunk(ball_bar);

  //�����̳� ����
  Block.clear();			
  Ball1.clear();
  Ball2.clear();

  G_item.clear();
  item1.clear();
  item2.clear();

  delete player1;
  delete player2;
  delete N_play;      // ��ֹ� ����
}

void CPlayState::Pause()
{
	int random=rand()%3;
	SDL_Surface * tmp_screen = SDL_GetVideoSurface();
	SDL_Surface *tmp;
	SDL_Rect	r_pos;
	SetSDLRect(r_pos,200,150,400,300);
	switch(random)
	{
	case 0:
		tmp=SDL_LoadBMP("image/game_tip1.bmp");
		break;
	case 1:
		tmp=SDL_LoadBMP("image/game_tip2.bmp");
		break;
	case 2:
		tmp=SDL_LoadBMP("image/game_tip3.bmp");
		break;
	}
	SDL_Surface *tips = SDL_DisplayFormat(tmp);
	SDL_SetAlpha(tips, SDL_RLEACCEL | SDL_SRCALPHA, 60);

	SDL_BlitSurface(tmp,NULL,tmp_screen,&r_pos);
	SDL_Flip(tmp_screen);
	SDL_Delay(2000);		//2�� ����
	SDL_FreeSurface(tmp);
	SDL_FreeSurface(tips);
	b_pause=-1;
}

void CPlayState::Resume()
{
}

void CPlayState::HandleEvents(CGameEngine* game)
{
	SDL_Event event;
	CPoint2 pos;

	if (SDL_PollEvent(&event)) 
	{
		switch (event.type) 
		{
			case SDL_QUIT:
				game->Quit();
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					 case SDLK_ESCAPE:							
							game->Quit();
							break;	
					 case SDLK_LEFT:
						 player2->SetLeft(true);
						 break;
					 case SDLK_RIGHT:
						 player2->SetRight(true);
						 break;
					 case SDLK_RETURN:						 
						 Ball2.back().SetState(1);			//���� ���¸� ���� ���������� �ٲ۴�.						 
						 break;
					case ']':	
						 break;
					 case 'a':
						 player1->SetLeft(true);
						 break;
					 case 'd':
						 player1->SetRight(true);
						 break;
					 case SDLK_SPACE:
						 Ball1.back().SetState(1);
						 break;
					 case 'g':	//1p �ڽſ��� ���
						 if(!item1.empty())
						 {
							 item1.back()->UseItem();
							 item1.pop_back();
						 }
						 break;
					 case 'h':
						 if(!item1.empty())
						 {
							 item1.back()->SetOwn(0);
							 item1.back()->UseItem();
							 item1.pop_back();
						 }
						 break;
					 case SDLK_DELETE: //2p �ڽſ��� ���
						 if(!item2.empty())
						 {
							 item2.back()->UseItem();
							 item2.pop_back();
						 }
						 break;
					 case SDLK_END:
						 if(!item2.empty())
						 {
							 item2.back()->SetOwn(1);
							 item2.back()->UseItem();
							 item2.pop_back();
						 }
						 break;
					 case 's': //������ �׽�Ʈ
						
						break;
				}			
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{						
					 case SDLK_LEFT:
						 player2->SetLeft(false);
						 break;
					 case SDLK_RIGHT:
						 player2->SetRight(false);
						 break;
					 case 'a':
						 player1->SetLeft(false);
						 break;
					 case 'd':
						 player1->SetRight(false);
						 break;
				}		
				break;			
		 }
	}
}

void CPlayState::Update(CGameEngine* game)
{
	if(Block.empty())	//���� ����� ���
	{
		game->map=ReachScore1>ReachScore2 ? 0:1;
		game->ChangeState( &gameOver );
		return ;
	}
	if(LaserOn1)			//1p �������� ���� ���
	{	
		r_Laser1.x=laser_pos1.x;
		laser_pos1.y-=20;
		r_Laser1.y=laser_pos1.y;
		cout<<r_Laser1.y<<endl;
		sheet_Laser1.x=laser_frame1*14;
		sheet_Laser1.y=0;
		sheet_Laser1.w=14;
		sheet_Laser1.h=170;
		if(laser_frame1 < 9)
			laser_frame1++;
	}
	
	if(LaserOn2)
	{	
		r_Laser2.x=laser_pos2.x;
		laser_pos2.y-=20;
		r_Laser2.y=laser_pos2.y;
		sheet_Laser2.x=laser_frame2*14;
		sheet_Laser2.y=0;
		sheet_Laser2.w=14;
		sheet_Laser2.h=170;
		if(laser_frame2 < 9)
			laser_frame2++;
	}
	//������ ���� 
	if(laser_pos1.y < -200)
		LaserOn1=false;
	if(laser_pos2.y < -200)
		LaserOn2=false;

    // ��ֹ��� �̵�
	if(N_play->GetRect().x == LEFTLINE) 
	{
		N_play->SetLeft(false);
		N_play->SetRight(true);
	}
	else if(N_play->GetRect().x == LEFTLINE+450)
	{
		N_play->SetLeft(true);
		N_play->SetRight(false);
	}
	N_play->Move(1);

	player1->Move(0);					//1player �̵�
	player2->Move(0);					//2player �̵�
		
	 /////1p   2p �켱 ���� ����
	 if(score1 > score2 )
	 {
		 player1->SetRect(CPoint2(player1->GetRect().x,550));
		 player2->SetRect(CPoint2(player2->GetRect().x,530));
	 }
	 else if(score1 !=score2)
	 {
		 player1->SetRect(CPoint2(player1->GetRect().x,530));
		 player2->SetRect(CPoint2(player2->GetRect().x,550));
	 }
	 ///
	
	if(Ball1.back().GetState()==false)
		Ball1.back().SetBarPos(player1->GetRect());		//1p�� ��ġ�� �����ͼ� ������ �ѱ�

	if(Ball2.back().GetState()==false)
		Ball2.back().SetBarPos(player2->GetRect());
	
	for_each(Ball1.begin(),Ball1.end(),mem_fun_ref(&CBall::Move));		//1p �� �̵�
    for_each(Ball2.begin(),Ball2.end(),mem_fun_ref(&CBall::Move));		//2p �� �̵�
	
	CheckColision();	//�浹 üũ �Լ�

	//�������� �̵�
	if(!G_item.empty())
		for_each(G_item.begin(),G_item.end(),mem_fun(&Citem::Update));

	//�� �ı� �ִϸ��̼��� ���� ������ ������Ʈ
	for_each(Block.begin(),Block.end(),mem_fun_ref(&CBlock::update));
	
	//�� �ƿ��Ǿ��� ��� �� ���� ���°�
	deque<CBall>::iterator pos;	

	pos=find_if(Ball1.begin(),Ball1.end(),Ball_destroy);
	if(pos!=Ball1.end())
	{
		Ball1.erase(pos);	
		ReachScore1-=100;
		if(ReachScore1 < 0) ReachScore1=0; 
		
		if(Ball1.empty())
			Ball1.push_back(CBall(CPoint2(LEFTLINE+55,540),s_ball1));
	}
	pos=find_if(Ball2.begin(),Ball2.end(),Ball_destroy);
	if(pos!=Ball2.end())
	{
		Ball2.erase(pos);
		ReachScore2-=100;
		if(ReachScore2 < 0) ReachScore2=0; 

		if(Ball2.empty())
			Ball2.push_back(CBall(CPoint2(RIGHTLINE+155,540),s_ball2));
	}
	/////////////////////////////////////////////
	//�ٴ����� ������ �������� ������ ��Ͽ��� ����
	if(!G_item.empty())
	{
		vector<Citem*>::iterator item_itor;
		item_itor=find_if(G_item.begin(),G_item.end(),check_item);
		if(item_itor!=G_item.end())
		{
			G_item.erase(item_itor);
		}
	}
	/////////////////////////////////////////////

	ID_reset++;			//ID ���� ī���� ����.
	if(ID_reset==60)	//0.5�ʰ� �Ǹ� ID�� ��� ���µ�.
	{
		for_each(Ball1.begin(),Ball1.end(),mem_fun_ref(&CBall::ID_Reset));
		for_each(Ball2.begin(),Ball2.end(),mem_fun_ref(&CBall::ID_Reset));
		ID_reset=0;
	}	
	 UpdateScore();												//���� ������Ʈ
	 //�������� ��� �浹 üũ
	 
	 if(LaserOn1)		//�������� ������� ���� �浹 üũ
	 {
		 deque<CBlock>::iterator iter_block;
		 iter_block = find_if(Block.begin(),Block.end(),Check_Bar(r_Laser1));
		 if(iter_block!=Block.end())
		 {			
			 AddScore(iter_block,ReachScore1);		//���� ����.
			 Block.erase(iter_block);
		 }
	 }
	 if(LaserOn2)		//2p �������� ��� ���� ���� �浹 üũ
	 {
		 deque<CBlock>::iterator iter_block;
		 iter_block = find_if(Block.begin(),Block.end(),Check_Bar(r_Laser2));
		 if(iter_block!=Block.end())
		 {			
			 AddScore(iter_block,ReachScore2);		//���� ����.
			 Block.erase(iter_block);
		 }
	}	
}

void CPlayState::Draw(CGameEngine* game)
{
  SDL_BlitSurface(bg, NULL, game->screen, NULL);					  //��� �׸���
  for_each(Block.begin(),Block.end(),BlockDraw(s_block,s_block_ani));//�� �׸���
  for_each(Ball1.begin(),Ball1.end(),mem_fun_ref(&CBall::Draw));	 //for_each �˰������� Ball1�� ��� ���� �׸���
  for_each(Ball2.begin(),Ball2.end(),mem_fun_ref(&CBall::Draw));
  
  if(!G_item.empty())
	for_each(G_item.begin(),G_item.end(),mem_fun(&Citem::Draw));		//������ �����Ȱ� �׸���
  
  player1->Draw(game);												//1p �׸���
  player2->Draw(game);												//2p �׸���

  if(Nbar_On)                            
	  N_play->Draw(game);                                           //��ֹ� �׸���

  ScoreDraw(game);													//���ھ� ���� �׸��� 
  DrawItemBox();													//������ �ڽ� �׸���
  if(b_pause==0) b_pause++;
  if(b_pause > 0)
	 Pause();  

  if(LaserOn1)
  {
	  SDL_BlitSurface(s_Laser, &sheet_Laser1, game->screen, &r_Laser1);
  }
  if(LaserOn2)
  {
	  SDL_BlitSurface(s_Laser, &sheet_Laser2, game->screen, &r_Laser2);
  }
}

//�浹 ���� �˰���
void CPlayState::CheckColision()
{
	//���� ���� �浹.
	int item_drop_rate=0;
	deque<CBlock>::iterator iter_pos;
	deque<CBlock>::iterator iter_tmp;	//�ӽ� �ݺ���
	deque<CBlock>::iterator iter_pos2;
	deque<CBall>::iterator  iter_ball;
	iter_pos=find_first_of(Block.begin(),Block.end(),Ball1.begin(),Ball1.end(),check_collision);		//�浹 üũ �˰���
	if(iter_pos!=Block.end())
	{				
		item_drop_rate=rand()%6;		//���� �μ��� �� �������� ���� Ȯ�� 1/6
		if(item_drop_rate==0)
		{
			int tmp=rand()%12;	
			if(tmp==0)
				G_item.push_back(new Nbar_block(iter_pos->GetRect(),item_sheet,2,10,&Nbar_On, &tmp_bar));
			else if(tmp==1)
				G_item.push_back(new AddBall(iter_pos->GetRect(),item_sheet,1,8,&Ball1,&Ball2,2)); //�������� ������ �ڽ��� �߰�.
			else if(tmp==2)
				G_item.push_back(new AddBall(iter_pos->GetRect(),item_sheet,1,9,&Ball1,&Ball2,4));
			else if(tmp==3)
				G_item.push_back(new RawBall(iter_pos->GetRect(),item_sheet,1,0,&Ball1,&Ball2,0));
			else if(tmp==4)
				G_item.push_back(new HighBall(iter_pos->GetRect(),item_sheet,1,3,&Ball1,&Ball2,0));
			else if(tmp==5)
				G_item.push_back(new SPupBall(iter_pos->GetRect(),item_sheet,1,5,&Ball1,&Ball2,0));
			else if(tmp==6)
				G_item.push_back(new SPdownBall(iter_pos->GetRect(),item_sheet,1,6,&Ball1,&Ball2,0));
			else if(tmp==7)
				G_item.push_back(new SPstopBall(iter_pos->GetRect(),item_sheet,1,7,&Ball1,&Ball2,0));
			else if(tmp==8)
				G_item.push_back(new SPhideBall(iter_pos->GetRect(),item_sheet,1,4,&Ball1,&Ball2,0));
			else if(tmp==9)
				G_item.push_back(new ControlBar(iter_pos->GetRect(),item_sheet,1,2,player1,player2));
			else if(tmp==10)
				G_item.push_back(new Laser(iter_pos->GetRect(),item_sheet,1,1,&LaserOn1,&laser_pos1,player1)); 
			else if(tmp==11)
				G_item.push_back(new Nbar_block(iter_pos->GetRect(),item_sheet,2,10,&Nbar_On, &tmp_bar));
		}
		AddScore(iter_pos,ReachScore1);		//���� ����.
	}
	iter_pos=find_first_of(Block.begin(),Block.end(),Ball2.begin(),Ball2.end(),check_collision);		//�浹 üũ �˰���
	if(iter_pos!=Block.end())
	{		
		item_drop_rate=rand()%6;
		if(item_drop_rate==1)
		{
			int tmp=rand()%12;	//�ӽ÷� �����Ѱ���
			if(tmp==0)
				G_item.push_back(new Bone(iter_pos->GetRect(),item_sheet,2,11,&Ball1,&Ball2,&ReachScore1,&ReachScore2)); //�������� ������ �ڽ��� �߰�.
			else if(tmp==1)
				G_item.push_back(new AddBall(iter_pos->GetRect(),item_sheet,0,8,&Ball1,&Ball2,2));
			else if(tmp==2)
				G_item.push_back(new AddBall(iter_pos->GetRect(),item_sheet,0,9,&Ball1,&Ball2,4));
			else if(tmp==3)
				G_item.push_back(new RawBall(iter_pos->GetRect(),item_sheet,0,0,&Ball1,&Ball2,0));
			else if(tmp==4)
				G_item.push_back(new HighBall(iter_pos->GetRect(),item_sheet,0,3,&Ball1,&Ball2,0));
			else if(tmp==5)
				G_item.push_back(new SPupBall(iter_pos->GetRect(),item_sheet,0,5,&Ball1,&Ball2,0));
			else if(tmp==6)
				G_item.push_back(new SPdownBall(iter_pos->GetRect(),item_sheet,0,6,&Ball1,&Ball2,0));
			else if(tmp==7)
				G_item.push_back(new SPstopBall(iter_pos->GetRect(),item_sheet,0,7,&Ball1,&Ball2,0));
			else if(tmp==8)
				G_item.push_back(new SPhideBall(iter_pos->GetRect(),item_sheet,0,4,&Ball1,&Ball2,0));
			else if(tmp==9)
				G_item.push_back(new ControlBar(iter_pos->GetRect(),item_sheet,0,2,player1,player2));
			else if(tmp==10)
				G_item.push_back(new Laser(iter_pos->GetRect(),item_sheet,0,1,&LaserOn2,&laser_pos2,player2));
			else if(tmp==11)
				G_item.push_back(new Nbar_block(iter_pos->GetRect(),item_sheet,2,10,&Nbar_On, &tmp_bar));
		}
		AddScore(iter_pos,ReachScore2);		//���� ����.
	}

	//�ı� ó���� �� �����̳ʿ��� ����
	iter_pos=find_if(Block.begin(),Block.end(),check_break_block);	
	if(iter_pos!=Block.end())
	{
		Block.erase(iter_pos);
	}
	//------------------------------------
	//���� bar�� �浹�� üũ�ϱ� ���� for-each�˰���
	for_each(Ball1.begin(),Ball1.end(),Check_Bar(player1->GetRect()));
	for_each(Ball2.begin(),Ball2.end(),Check_Bar(player2->GetRect()));
	
	//���� �������� �浹�� üũ �ϱ� ���� �˰���	
	find_first_of(Ball1.begin(),Ball1.end(),Ball2.begin(),Ball2.end(),ball_collision<CBall,CBall>);

	if(!G_item.empty())	//������ �����̳ʿ� �������� ���� ��츸 �浹 Ȯ�� (������ ���̱� ����)
	{
		//�����۰� bar�� �浹 �ϴ°� üũ ������ �Ծ����� ��
		vector<Citem*>::iterator item_pos;
		item_pos=find_if(G_item.begin(),G_item.end(),Check_Bar(player1->GetRect()));		//�÷��̾� 1�� �浹�� ������ �ֳ� üũ
		if(item_pos!=G_item.end())
		{			
			if( (*item_pos)->GetOwn() == 1 || (*item_pos)->GetOwn()==2)	//�浹�� �������� ���� �϶��� �� �Ծ����� �ϱ�
			{
				if((*item_pos)->GetOwn()==2)		//���� ����
				{
					(*item_pos)->SetOwn(0);
					(*item_pos)->UseItem();
				}
				else if(item1.size() < 3)		//�������� 3���� �Ծ����� �Ѵ�.
					item1.push_back(*item_pos);
				G_item.erase(item_pos);
			}
		}
		item_pos=find_if(G_item.begin(),G_item.end(),Check_Bar(player2->GetRect()));		//�÷��̾� 2�� �浹�� ������ �ֳ� üũ
		if(item_pos!=G_item.end())
		{
			
			if( (*item_pos)->GetOwn() ==0 || (*item_pos)->GetOwn()==2)
			{
				if((*item_pos)->GetOwn()==2)		//���� ����
				{
					(*item_pos)->SetOwn(1);
					(*item_pos)->UseItem();
				}
				else if(item2.size() < 3)
					item2.push_back(*item_pos);
				G_item.erase(item_pos);
			}
		}
	}
	
	if(Nbar_On)
	{
		static bool temp1=true, temp2=true;      // ���� ��ֹ� �߰����� ���� �� ����� ������
												 // ���� ����

		iter_ball=find_if(Ball1.begin(),Ball1.end(),obstacle(N_play->GetRect()));
		if(iter_ball!=Ball1.end())
		{
			if(temp1)
			{
				Ball1.back().InverseVec((CPoint2(1.0f,-1.0f)));
				temp1=false;
			}	
		}
		else
			temp1=true;


		iter_ball=find_if(Ball2.begin(),Ball2.end(),obstacle(N_play->GetRect()));
		if(iter_ball!=Ball2.end())
		{
			if(temp2)
			{
				Ball2.back().InverseVec((CPoint2(1.0f,-1.0f)));
				temp2=false;
			}		
		}
		else
			temp2=true;
		

		tmp_bar--;

		if(tmp_bar == 0)
			Nbar_On = false;
	}
}

void CPlayState::AddScore(deque<CBlock>::iterator pos, int &score)
{
	//���� ����
		switch(pos->GetKind()) //0 : �� 1:�� 2:���� 3.�� 4.��
		{
			case 0:
				score+=20;
				Mix_PlayChannel(-1, dak, 0); 
				break;
			case 1:
				Mix_PlayChannel(-1, dog, 0); 
				score+=30;
				break;
			case 2:
				Mix_PlayChannel(-1, pig, 0); 
				score+=40;
				break;
			case 3:
				Mix_PlayChannel(-1, cow, 0); 
				score+=50;
				break;
			case 4:
				Mix_PlayChannel(-1, sheep, 0); 
				score+=60;
				break;
			case 5:
				score+=200;
				Mix_PlayChannel(-1, lion, 0); 
				break;
		}		
}
void CPlayState::UpdateScore()
{
	if(ReachScore1 > score1)
	{
		score1+=2;
	}
	else if(ReachScore1 < score1)
	{
		score1-=2;
	}
	if(ReachScore2 > score2)
	{
		score2+=2;
	}
	else if(ReachScore2 < score2)
	{
		score2-=2;
	}
}

void CPlayState::ScoreDraw(CGameEngine* game)
{
	SDL_Rect s_point1,s_point2;
	SDL_Rect pos1,pos2;

	int Score1[4],Score2[4];
	int tmp;

	if(score1 < 0)
		score1=0;
	if(score2 < 0)
		score2=0;

	Score1[0]=score1/1000;
	Score2[0]=score2/1000;

	tmp=score1%1000;
	Score1[1]=tmp/100;
	tmp=tmp%100;
	Score1[2]=tmp/10;
	Score1[3]=tmp%10;

	tmp=score2%1000;
	Score2[1]=tmp/100;
	tmp=tmp%100;
	Score2[2]=tmp/10;
	Score2[3]=tmp%10;

	for(int i=0;i<4;i++)
	{
		s_point1.x=Score1[i]*26;
		s_point1.y=0;
		s_point1.w=26;
		s_point1.h=30;
		pos1.x=13+i*31;
		pos1.y=483;
		pos1.w=26;
		pos1.h=30;
		SDL_BlitSurface(s_score, &s_point1, game->screen, &pos1);
		s_point2.x=Score2[i]*26;
		s_point2.y=0;
		s_point2.w=26;
		s_point2.h=30;
		pos2.x=670+i*31;
		pos2.y=483;
		pos2.w=26;
		pos2.h=30;
		SDL_BlitSurface(s_score, &s_point2, game->screen, &pos2);
	}	
}
//������ �ڽ� �׸���
inline void CPlayState::DrawItemBox()
{
	if(!item1.empty())
	{
		int i=item1.size();
		for(int j=0;j<i;j++)
		{
			item1[j]->SetRect(52,345-j*55);		//1p ������ �ڽ��� �׸� ��ġ ����
		}		
		
		for_each(item1.begin(),item1.end(),mem_fun(&Citem::Draw));
	}
	if(!item2.empty())
	{
		int i=item2.size();
		for(int j=0;j<i;j++)
		{
			item2[j]->SetRect(714,345-j*55);		//2p ������ �ڽ��� �׸� ��ġ ����
		}
		for_each(item2.begin(),item2.end(),mem_fun(&Citem::Draw));
	}
	
}



void CPlayState::MapCreate(int A_Stage)
{
	int Pos_X=LEFTLINE,Pos_Y=0;          //��� ����� X���� Y��

	int BC_i = 0, BC_j = 0, BC_k = 0;    //����� ���� ������ �����ϴ� ���ڰ�
	int temp_box = 0;                    
	bool Block_Create = true, Block_MCreate = false;    // ����� ���θ� ����


	if(A_Stage == 0) // �⺻���� �׸� ������ �� ����
 	{
		int Pos_X=LEFTLINE,Pos_Y=0;
		for(int i=0;i<10;i++)
		{
			for(int j=0;j<17;j++)
			{			
				Block.push_front(CBlock(CPoint2(Pos_X,Pos_Y)));
				Pos_X+=30;		//�� ũ�� 30�� ����
			}
			Pos_X=LEFTLINE;
			Pos_Y+=30;
		}
	}
	else if(A_Stage == 1) // ������ ������ ����
	{
		BC_k = -7;
		for(int i=0;i<10;i++)
		{
			BC_j = i*2 - 1;

			for(int j=0;j<17;j++)
			{
				if((j + BC_i) == 9 && i != 0)
					Block_Create = false;

				if((9 - (BC_k + 1)) == (j - (i - 3)) && i >= 4)
				{
					Block_Create = true;
					Block_MCreate = true;
					temp_box = BC_k;
				}

				if(Block_MCreate)
				{
					if(temp_box <= 0)
					{
						Block_Create = false;
						Block_MCreate = false;

						BC_j -= BC_k;
					}
					temp_box--;
				}

				if(Block_Create == false)
				{
					BC_j--;
				
					if(BC_j <= 0)
						Block_Create = true;					

				}
				else
					Block.push_front(CBlock(CPoint2(Pos_X,Pos_Y)));
			
				Pos_X+=30;		//�� ũ�� 30�� ����
			
			}
			Pos_X=LEFTLINE;
			Pos_Y+=30;

			BC_k += 2;
			BC_i++;
		}
		for(int i=10;i<12;i++)
		{
			for(int j=0; j<17; j++)
			{
				Block.push_front(CBlock(CPoint2(Pos_X,Pos_Y)));
				Pos_X+=30;
			}	

			Pos_X=LEFTLINE;
			Pos_Y+=60;
		}
	}
	else if(A_Stage == 2)   // ��Ʈ���� ����
	{
		Pos_Y = 0;		
		for(int i=0; i<3; i++)   // ��Ʈ�� �����κ��� ����
		{		 
			BC_k = (4 - i);
			BC_j = 0;
			
			for(int j=0; j<17; j++)
			{
				if( j == BC_k || j == (17 - BC_j))
				{
					temp_box = ((i*2) + 2);
					Block_Create = true;
				}

				if(temp_box == 0)
				{
					BC_j = j;
					Block_Create = false;
				}			

				if(Block_Create)
					Block.push_front(CBlock(CPoint2(Pos_X,Pos_Y)));

				temp_box--;
				Pos_X+=30;		
			}
			Pos_X=LEFTLINE;
			Pos_Y+=30;
		}

		for(int i=0; i<3; i++)  // ��Ʈ�� �߰��κ��� ����
		{
			Pos_X=(LEFTLINE + 30);
			for(int j=0; j<15; j++)
			{
				Block.push_front(CBlock(CPoint2(Pos_X,Pos_Y)));
				Pos_X+=30;		
			}
			Pos_Y+=30;
		}

		for(int i=0; i<7; i++)  // ��Ʈ�� �ϴܺκ��� ����
		{
			Pos_X=(LEFTLINE + 30);
			for(int j=0; j<15; j++)
			{
				if( j == (i+1) )
					Block_Create = true;
				else if( j == (14-i))
					Block_Create = false;

				if(Block_Create)
					Block.push_front(CBlock(CPoint2(Pos_X,Pos_Y)));
				Pos_X+=30;
			}
			Pos_Y+=30;
		}
	}
	else if(A_Stage == 3)   // ������ ����
	{
		Pos_Y = 0;
		Block_Create = false;
		for(int i=0; i<13; i++)
		{
			Pos_X=(LEFTLINE + 30);

			if(i == 0)
				BC_i = 7;
			else if(i == 4)
				BC_i = 0;
			else if(i == 9)
				BC_i = 4;	

			for(int j=0; j<15; j++)
			{
				if( j == BC_i)
					Block_Create = true;				
				else if(j == (15 - BC_i))
					Block_Create = false;

				if(Block_Create)
					Block.push_front(CBlock(CPoint2(Pos_X,Pos_Y)));
		
				Pos_X+=30;
			}

			Block_Create = false;

			if(i < 4)
				BC_i--;
			else if(i >= 4 && i < 6)
				BC_i++;
			else if(i >= 6 && i < 9)
				BC_i--;
			else if(i >= 9)
				BC_i++;

			Pos_Y+=30;
		}
	}
	//���ڿ� ����

	Block.front().SetSheet(150);
	Block.front().SetKind(5);
	
	Block.back().SetSheet(150);
	Block.back().SetKind(5);
}

