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
//extern CGameOver gameOver;

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
	pos.x=(float)crossRect.x+15;
	pos.y=(float)crossRect.y+15;
	p=ball.GetPos();
	
	if(block.GetState()==false || block.Getbreak()==true)	//�̹� �ı��� ���� ��� �׳� false;
	{
		return false;
	}
	if(block.GetKind()==6)
		return false;

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
	
	
	if(block.GetKind()!=6)
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
		ball.InverseVec(CPoint2(-1.0f,1.0f));
	}
	else if(degree > 45 && degree <= 135)		//�Ʒ� ����
	{
		ball.InverseVec(CPoint2(1.0f,-1.0f));
	}
	else if(degree > 135 || degree <= -135)	//���ʿ� ����
	{
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
	if(a.GetState()==0 || b.GetState()==0)
		return false;
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
	if(item->GetRect().y > BOTTOMLINE-20)
		return true;
	else
		return false;

}
////////////////////
//�� ���� ������
bool Ball_destroy(CBall &ball)
{
	int y;
	y=(int)ball.GetPos().y;
	if(y > BOTTOMLINE+200)
	{		
		return true;
	}

	return false;
}


void CPlayState::Init(CGameEngine* game) //�ʱ�ȭ
{
	srand(unsigned int(time(NULL)));
	
	//play=true;
	m_IDReset=0;
	m_score=0;
	m_LaserFrame=0;
	m_duration = 0;
	m_addBlockLine = 0;

	SetSDLRect(m_rLaser,0,0,14,17);					//������ �ʱ� ��ġ

	//������ ��� Ȯ�� ����
	char item[100] = {
		0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,
		2,2,2,2,2,
		3,3,3,3,3,3,3,3,3,3,
		4,4,4,4,4,4,4,4,4,4,
		5,5,5,5,5,5,5,5,5,5,
		6,6,6,6,6,
		7,7,7,7,7,7,7,
		8,8,8,8,8,8,8,8,
		9,9,9,9,9,9,9,
		10,10,10,
		11,11,11,11,11,11,11,11,11,11,
		12,12,12,12,12,
		13,13,13,13,13,
	};
	memcpy(itemDropTable,item,sizeof(item));

	m_ReachScore=0;

	//ä���� ���� ����Ʈ �ڽ� ����
	SDL_Rect chatEditPos;
	SetSDLRect(chatEditPos,500,718,500,26);
	chatEditBox.CreateEditBox(chatEditPos,READ_WRITE);	//����Ʈ �ڽ� ����

	//----------�̹��� ���� �ʱ�ȭ----------------------------------
	//�̹��� ���� ����
		
	SDL_Surface* temp = SDL_LoadBMP("../../Game/image/layout.bmp"); //��� �׸� load
	m_sBackGround = SDL_DisplayFormat(temp);

	//�� �̹��� �ε�
	temp=SDL_LoadBMP("../../Game/image/block_sheet.bmp");
	m_sBlock=SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sBlock, SDL_SRCCOLORKEY, SDL_MapRGB( m_sBlock->format, 0, 0, 0) );
	
	//Ű���� ��ݺ� ���
	SDL_EnableKeyRepeat(10,SDL_DEFAULT_REPEAT_INTERVAL);
	//-------------------�� ����-----------------------
	
	CreateMap(this->pArgs->mapIndex);		
	//-----------------Bar ����----------------------------
	m_pCBPlayer= new CBar(CPoint2(LEFTLINE+180,BOTTOMLINE-20));

	//-----------------��ֹ� ����-------------------------
	turtle.Init(game->screen);				//�ź��� �ʱ�ȭ


	//�̹����ε�
	temp = SDL_LoadBMP("../../Game/image/bar_1p.bmp");		//bar �̹��� �ε�
	m_sBar = SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sBar, SDL_SRCCOLORKEY, SDL_MapRGB( m_sBar->format, 255, 0, 0) );

	m_pCBPlayer->Init(m_sBar);						//�ε��� �̹����� ǥ���� �����͸� ������ bar ��ü�� �Ѱ��ش�.

	//��ҵ� bar �ε�
	temp = SDL_LoadBMP("../../Game/image/bar2_1.bmp");
	imageBuffer[4] = SDL_DisplayFormat(temp);
	SDL_SetColorKey( imageBuffer[4], SDL_SRCCOLORKEY, SDL_MapRGB( imageBuffer[4]->format, 255, 0, 0) );
	
	//----------------Ball ����----------------------------
	temp = SDL_LoadBMP("../../Game/image/ball.bmp");		//ball �̹��� �ε�
	m_sBall=SDL_DisplayFormat(temp);
	m_deqBall.push_back(CBall(CPoint2(LEFTLINE+55,BOTTOMLINE-25),m_sBall,&m_minCollisionLine));	//ball �����̳ʿ� 1���� �־��ش�. ó������ ��1���� �����ϹǷ�

	//�� �ı� �ִϸ��̼� ����
	temp = SDL_LoadBMP("../../Game/image/block_broken.bmp");
	m_sBlockAnim=SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sBlockAnim, SDL_SRCCOLORKEY, SDL_MapRGB( m_sBlockAnim->format, 133, 176, 182) );

	//���ھ� ��Ʈ �̹��� �ε�
	temp = SDL_LoadBMP("../../Game/image/number_sheet.bmp");
	m_sScore= SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sScore, SDL_SRCCOLORKEY, SDL_MapRGB( m_sScore->format, 0, 0, 0) );

	//������ ��Ʈ �̹��� �ε�
	temp = SDL_LoadBMP("../../Game/image/item_sheet.bmp");
	m_sItemSheet=SDL_DisplayFormat(temp);
	SDL_SetColorKey(m_sItemSheet , SDL_SRCCOLORKEY, SDL_MapRGB( m_sItemSheet->format, 0, 250, 250) );

	//������ ��Ʈ �̹��� �ε�
	temp = SDL_LoadBMP("../../Game/image/laser_sheet.bmp");
	m_sLaser=SDL_DisplayFormat(temp);
	SDL_SetColorKey(m_sLaser , SDL_SRCCOLORKEY, SDL_MapRGB( m_sLaser->format, 255, 0, 0) );

	//���� �ε�
	m_mBgm[0] = Mix_LoadMUS("../../Game/music/BGM.mp3");
	m_mBgm[1] = Mix_LoadMUS("../../Game/music/BGM2.mp3");
	if(m_mBgm[0]==NULL || m_mBgm[1]==NULL)
	{
		cout<<"���� �ε� ����"<<endl;
	}
	

	//���� ���(���ž�� �ӽ� �ּ�)
	int randomMusic = rand()%2;
	Mix_PlayMusic( m_mBgm[randomMusic], -1 );

	//ȿ���� �ε�
	m_wCow		=Mix_LoadWAV("../../Game/music/cow.wav");
	m_wDog		=Mix_LoadWAV("../../Game/music/dog.wav");
	m_wSheep	=Mix_LoadWAV("../../Game/music/sheep.wav");
	m_wLion		=Mix_LoadWAV("../../Game/music/lion.wav");
	m_wChiken	=Mix_LoadWAV("../../Game/music/chiken.wav");
	m_wPig		=Mix_LoadWAV("../../Game/music/pig.wav");
	m_wBump		=Mix_LoadWAV("../../Game/music/ball.wav");

    //������ ȿ���� �߰� �ؾ���. 
	SDL_FreeSurface(temp);	

	//�̹��� �����͵�  ����
	imageBuffer[0] = m_sBlock;
	imageBuffer[1] = m_sBlockAnim;
	imageBuffer[2] = m_sBar;
	imageBuffer[3] = m_sBall;

	//�ٸ� �÷��̾� ó�� ��ü �ʱ�ȭ
	m_otherPlayer.init(imageBuffer,game->screen,pArgs->numOfUser,m_deqBlock,pArgs);	
	
	//���� ������ �ʱ�ȭ
	memset(colData.breakenBlock,-1,sizeof(colData.breakenBlock));
	memset(&colData.itemInfo,-1,sizeof(colData.itemInfo));

	//Ÿ�̸� �ʱ�ȭ
	m_timer.StartTick();	
}

void CPlayState::Cleanup()  //�Ҹ�
{ 	
  SDL_FreeSurface(m_sBackGround);
  SDL_FreeSurface(m_sBlock);
  SDL_FreeSurface(m_sBar);
  SDL_FreeSurface(m_sBall);
  SDL_FreeSurface(m_sBlockAnim);
  SDL_FreeSurface(m_sItemSheet);
  SDL_FreeSurface(m_sLaser);
  SDL_FreeSurface(s_Obstarcle);
  SDL_FreeSurface(m_sScore);
  SDL_FreeSurface(imageBuffer[4]);
  
  Mix_HaltMusic();  
  Mix_FreeMusic(m_mBgm[0]);
  Mix_FreeMusic(m_mBgm[1]);

  Mix_FreeChunk(m_wCow);
  Mix_FreeChunk(m_wDog);
  Mix_FreeChunk(m_wLion);
  Mix_FreeChunk(m_wChiken);
  Mix_FreeChunk(m_wPig);
  Mix_FreeChunk(m_wSheep);
  Mix_FreeChunk(m_wBump);


  //�����̳� ����
  m_deqBlock.clear();			
  m_deqBall.clear();
 
  m_vecPublicItem.clear();
  m_vecAbtainItem.clear();
  
  delete m_pCBPlayer;  
 
  turtle.CleanUp();
  m_otherPlayer.CleanUp();

}

void CPlayState::Pause()
{
	
}

void CPlayState::Resume()
{

}

//������ ���� �Լ�
MYDATA* CPlayState::Collect()
{
	//���� ������ ����
	for(UINT i=0;i<m_deqBall.size();i++)
	{
		colData.ballPos[i] = m_deqBall[i].GetRect();	//���� ��ġ �����ͼ� ����.
	}
	//player bar�� ��ġ�� ����
	colData.barRect = m_pCBPlayer->GetRect();

	//�μ��� ��� �ε����� ����
	memset(colData.breakenBlock,-1,sizeof(colData.breakenBlock));		//�ʱ�ȭ -1 �� ����� �ǹ�
	
	for(UINT i=0;i<m_brokenBlockIdx.size();i++)
	{
		colData.breakenBlock[i] = m_brokenBlockIdx[i];
	}

	colData.score = m_ReachScore;
	colData.numOfBall = (UINT)m_deqBall.size();
	
	return &colData;
}

void CPlayState::HandleEvents(CGameEngine* game)
{
	SDL_Event events;
	CPoint2 pos;

	SDL_PollEvent(&events);
	//Ű���� �Է� ó��
	Uint8 *keystate = SDL_GetKeyState(NULL);

	if ( keystate[SDLK_ESCAPE] )	//���� ����
	{		
		game->Quit();
	}

	if( keystate[SDLK_RETURN])
	{
		if(chatEditBox.GetEnable())
		{
			if(chatEditBox.textBuffer.empty())
			{
				chatEditBox.EnableSet(false);
				keystate[SDLK_RETURN] = false;
			}
		}
		else 
		{
			chatEditBox.EnableSet(true);
			keystate[SDLK_RETURN] = false;
		}
	}
	//Ȱ��ȭ �� ���.
	if(chatEditBox.GetEnable())
	{		
		Uint8 ch;
		for(Uint8 i=0;i<255;i++)
		{
			if(keystate[i])
			{
				ch=i;
				chatEditBox.ReadEdit(ch);
				keystate[i] = false;
			}
		}
		return;
	} 
	
	if( keystate[SDLK_LEFT] )
		m_pCBPlayer->MoveLeft();
	else if( keystate[SDLK_RIGHT] )
		m_pCBPlayer->MoveRight();
	else if( keystate[SDLK_SPACE] )		//�� �߻�
		m_deqBall.back().SetState(1);
	
	if( keystate['a'] )			//������ �ڽſ��� ���
	{
		 if(!m_vecAbtainItem.empty())
		 {
			 m_vecAbtainItem.front()->UseItem();
			 m_vecAbtainItem.pop_front();
		 }		
		 keystate['a'] = false;
	}	
	else if( keystate['1'])				//0������ ������ ���
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //�ڽ��� ��ȣ�� ������ ��� ó��
			 if(m_otherPlayer.GetCurrPlayerNum() == 0)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//������ ��ȣ
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 0;			//���
			 }
		 }
		 keystate['1'] = false;
	}
	else if( keystate['2'])				//1������ ������ ���
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //�ڽ��� ��ȣ�� ������ ��� ó��
			 if(m_otherPlayer.GetCurrPlayerNum() == 1)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//������ ��ȣ
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 1;			//���
			 }
		 }
		 keystate['2'] = false;
	}
	else if( keystate['3'])				//1������ ������ ���
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //�ڽ��� ��ȣ�� ������ ��� ó��
			 if(m_otherPlayer.GetCurrPlayerNum() == 2)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//������ ��ȣ
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 2;			//���
			 }
		 }
		 keystate['3'] = false;
	}
	else if( keystate['4'])				//1������ ������ ���
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //�ڽ��� ��ȣ�� ������ ��� ó��
			 if(m_otherPlayer.GetCurrPlayerNum() == 3)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//������ ��ȣ
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 3;			//���
			 }
		 }
		 keystate['4'] = false;
	}
	else if( keystate['5'])				//1������ ������ ���
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //�ڽ��� ��ȣ�� ������ ��� ó��
			 if(m_otherPlayer.GetCurrPlayerNum() == 4)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//������ ��ȣ
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 4;			//���
			 }
		 }
		 keystate['5'] = false;
	}
	else if( keystate['6'])				//1������ ������ ���
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //�ڽ��� ��ȣ�� ������ ��� ó��
			 if(m_otherPlayer.GetCurrPlayerNum() == 5)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//������ ��ȣ
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 5;			//���
			 }
		 }
		 keystate['6'] = false;
	}
	
	/*//�׽�Ʈ��
	if( keystate['s'])
	{
		Citem *tmpItem;
		tmpItem = new AddBall(SDL_Rect(),NULL,&m_deqBall,4,&m_minCollisionLine);
		tmpItem->UseItem();
		delete tmpItem;
		keystate['s'] = false;
	}
	if( keystate['d'])
	{
		Citem *tmpItem;
		tmpItem = new SPstopBall(SDL_Rect(),NULL,&m_deqBall);
		tmpItem->UseItem();
		delete tmpItem;
		keystate['d'] = false;
	}*/
}

void CPlayState::Update(CGameEngine* game)
{
	m_timer.GetTick();
	
	double tmpTime = m_timer.GetElapedTime();
	m_duration += tmpTime;
	
	//30�� Ÿ�̸� 
	if(m_duration > 20000.0f)
	{
		m_duration = 0;
		m_timer.StartTick();	//Ÿ�̸� �����
		//�Ⱥμ����� ��� ����.
		NonBrokenBlockCreate();
	}

	//���� �˻�
	m_minCollisionLine = m_addBlockLine*30+148;
     //���� ���� ���� (�Ͻ����� �ּ� ó�� 
	/*if(m_vecAbtainItem.empty())	//���� ����� ���
	{
		game->map=ReachScore1>ReachScore2 ? 0:1;
		game->ChangeState( &gameOver );
		return ;
	}*/

	//������ �̵�
	if(m_bLaserOn)			//1p �������� ���� ���
	{	
		m_rLaser.x=(Sint16)m_cpLaserPos.x;
		m_cpLaserPos.y-=20;
		m_rLaser.y=(Sint16)m_cpLaserPos.y;
		
		m_rSheetLaser.x=m_LaserFrame*14;
		m_rSheetLaser.y=0;
		m_rSheetLaser.w=14;
		m_rSheetLaser.h=170;
		if(m_LaserFrame < 9) //������ ������ ����. 
			m_LaserFrame++;
	}	

	//������ ���� 
	if(m_cpLaserPos.y < TOPLINE)
		m_bLaserOn=false;


    // ��ֹ��� �̵�
	turtle.Move();	

	
			
	if(m_deqBall.back().GetState()==false)
		m_deqBall.back().SetBarPos(m_pCBPlayer->GetRect());		//1p�� ��ġ�� �����ͼ� ������ �ѱ�
	
	for_each(m_deqBall.begin(),m_deqBall.end(),mem_fun_ref(&CBall::Move));		//1p �� �̵�
   	
	CheckColision();	//�浹 üũ �Լ�


	//�������� �̵�	 
	if(!m_vecPublicItem.empty())
		for_each(m_vecPublicItem.begin(),m_vecPublicItem.end(),mem_fun(&Citem::Update));

	//�� �ı� �ִϸ��̼��� ���� ������ ������Ʈ
	for_each(m_deqBlock.begin(),m_deqBlock.end(),mem_fun_ref(&CBlock::update));
	
	//�� �ƿ��Ǿ��� ��� �� ���� ���°�
	deque<CBall>::iterator pos;	

    pos=find_if(m_deqBall.begin(),m_deqBall.end(),Ball_destroy);
	if(pos!=m_deqBall.end())
	{
		m_deqBall.erase(pos);	
		m_ReachScore-=100;   
		if(m_ReachScore < 0) m_ReachScore=0; 
		
		if(m_deqBall.empty())
		{
			SDL_Rect tmpRect = m_pCBPlayer->GetRect();
			m_deqBall.push_back(CBall(CPoint2((float)tmpRect.x+50,(float)tmpRect.y-5),m_sBall));
			m_deqBall.back().SetCollisionLine(&m_minCollisionLine);	//top���� �߰�.
		}
	}
	
	/////////////////////////////////////////////
	//�ٴ����� ������ �������� ������ ��Ͽ��� ����
	if(!m_vecPublicItem.empty())
	{
		vector<Citem*>::iterator item_itor;
		item_itor=find_if(m_vecPublicItem.begin(),m_vecPublicItem.end(),check_item);
		if(item_itor!=m_vecPublicItem.end())
		{
			m_vecPublicItem.erase(item_itor);
		}
	}
	/////////////////////////////////////////////

	m_IDReset++;			//ID ���� ī���� ����.
	if(m_IDReset==30)	//0.5�ʰ� �Ǹ� ID�� ��� ���µ�.
	{
		for_each(m_deqBall.begin(),m_deqBall.end(),mem_fun_ref(&CBall::ID_Reset));
		m_IDReset=0;
	}	

	 UpdateScore();												//���� ������Ʈ

	 //////////////////////////////////
	 //�������� ��� �浹 üũ
	 ///////////////////////////////////

	 if(m_bLaserOn)		//�������� ������� ���� �浹 üũ
	 {
		 deque<CBlock>::iterator iter_block;
		 iter_block = find_if(m_deqBlock.begin(),m_deqBlock.end(),Check_Bar(m_rLaser,m_wBump));
		 if(iter_block!=m_deqBlock.end())
		 {		
			  //�μ��� ��� ��� ����
			if(m_brokenBlockIdx.size() < 10)		//10�� ������ ����
				m_brokenBlockIdx.push_back(iter_block->GetBlockNum());	//����� �μ��� idx�� �����ͼ� �����̳ʿ� ����
			else														//10�� �̻��� ��� front�� �����ϰ� push_back
			{
				m_brokenBlockIdx.pop_front();							//���� ������ ��� �ε��� ����
				m_brokenBlockIdx.push_back(iter_block->GetBlockNum());	//�ε��� ����
			}
			AddScore(iter_block,m_ReachScore);		//���� ����.
			m_deqBlock.erase(iter_block);
		 }
	 }

	 //�ٸ� �÷��̾� ���� ó��
	 m_otherPlayer.SetPlayersInform(game);
	 TargetItemProcess();
}

void CPlayState::Draw(CGameEngine* game)
{	  
  SDL_BlitSurface(m_sBackGround, NULL, game->screen, NULL);			 //��� �׸���
  //SDL_SoftStretch(m_sBackGround,NULL,game->screen,&des);			 //��� �׸���.
  for_each(m_deqBlock.begin(),m_deqBlock.end(),BlockDraw(m_sBlock,m_sBlockAnim));//�� �׸���
  for_each(m_deqBall.begin(),m_deqBall.end(),mem_fun_ref(&CBall::Draw));	 //for_each �˰������� m_deqBall�� ��� ���� �׸���
  
  //������ �����Ȱ� �׸���
  if(!m_vecPublicItem.empty())
	for_each(m_vecPublicItem.begin(),m_vecPublicItem.end(),mem_fun(&Citem::Draw));		
  
  //�� �׸���
  m_pCBPlayer->Draw(game);												//1p Bar Draw
  
  //��ֹ� �׸���
  turtle.Draw();

  //�ٸ� �÷��̾� ȭ�� �׸���
  m_otherPlayer.Draw();

  //������ �ڽ� �׸���
  DrawItemBox();

  //������ Draw
  if(m_bLaserOn)
  {
	  SDL_BlitSurface(m_sLaser, &m_rSheetLaser, game->screen, &m_rLaser);
  }  

  //chat�Է� â Ŀ�� �׸���
  chatEditBox.DrawEditBox(game->screen);
}

//�浹 ���� �˰���
void CPlayState::CheckColision()
{
	int itemDropRate=0;				//������ �����.

	deque<CBlock>::iterator iter_pos;
	deque<CBall>::iterator  iter_ball;
	
	//���� ���� �浹
	iter_pos=find_first_of(m_deqBlock.begin(),m_deqBlock.end(),m_deqBall.begin(),m_deqBall.end(),check_collision);		//�浹 üũ �˰���
	if(iter_pos!=m_deqBlock.end())
	{			
		itemDropRate=rand()%4;		//���� �μ��� �� �������� ���� Ȯ�� 1/4
		if(itemDropRate==0)
		{
			int tableIndex = rand()% 100;
			int itemType=itemDropTable[tableIndex];	
			if(itemType==0)
				m_vecPublicItem.push_back(new Bone(iter_pos->GetRect(),m_sItemSheet,&m_deqBall,&m_ReachScore));
			else if(itemType==1)
				m_vecPublicItem.push_back(new AddBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall,2,&m_minCollisionLine)); //�������� ������ �ڽ��� �߰�.
			else if(itemType==2)
				m_vecPublicItem.push_back(new AddBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall,4,&m_minCollisionLine));
			else if(itemType==3)
				m_vecPublicItem.push_back(new RawBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall));
			else if(itemType==4)
				m_vecPublicItem.push_back(new HighBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall));
			else if(itemType==5)
				m_vecPublicItem.push_back(new SPupBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall));
			else if(itemType==6)
				m_vecPublicItem.push_back(new SPdownBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall));
			else if(itemType==7)
				m_vecPublicItem.push_back(new SPstopBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall));
			else if(itemType==8)
				m_vecPublicItem.push_back(new SPhideBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall));
			else if(itemType==9)
				m_vecPublicItem.push_back(new ControlBar(iter_pos->GetRect(),m_sItemSheet,m_pCBPlayer));
			else if(itemType==10)
				m_vecPublicItem.push_back(new Laser(iter_pos->GetRect(),m_sItemSheet,&m_bLaserOn,&m_cpLaserPos,m_pCBPlayer)); 
			else if(itemType==11)
				m_vecPublicItem.push_back(new Nbar_block(iter_pos->GetRect(),m_sItemSheet,&turtle));
			else if(itemType==12)
				m_vecPublicItem.push_back(new PlusPoint(iter_pos->GetRect(), m_sItemSheet, &m_ReachScore,100));
			else if(itemType==13)
				m_vecPublicItem.push_back(new PlusPoint(iter_pos->GetRect(), m_sItemSheet, &m_ReachScore, 200));
			
		}
		AddScore(iter_pos,m_ReachScore);		//���� ����.
		
		//�μ��� ��� ��� ����
		if(m_brokenBlockIdx.size() < 10)		//10�� ������ ����
			m_brokenBlockIdx.push_back(iter_pos->GetBlockNum());	//����� �μ��� idx�� �����ͼ� �����̳ʿ� ����
		else														//10�� �̻��� ��� front�� �����ϰ� push_back
		{
			m_brokenBlockIdx.pop_front();							//���� ������ ��� �ε��� ����
			m_brokenBlockIdx.push_back(iter_pos->GetBlockNum());	//�ε��� ����
		}
	}	

	//�ı� ó���� �� �����̳ʿ��� ����
	iter_pos=find_if(m_deqBlock.begin(),m_deqBlock.end(),check_break_block);	
	if(iter_pos!=m_deqBlock.end())
	{
		m_deqBlock.erase(iter_pos);
	}
	//------------------------------------
	//���� bar�� �浹�� üũ�ϱ� ���� for-each�˰���
	for_each(m_deqBall.begin(),m_deqBall.end(),Check_Bar(m_pCBPlayer->GetRect(),m_wBump));
		
	//������ ȹ�濡 ���� ó��
	if(!m_vecPublicItem.empty())	//������ �����̳ʿ� �������� ���� ��츸 �浹 Ȯ�� (������ ���̱� ����)
	{
		//�����۰� bar�� �浹 �ϴ°� üũ ������ �Ծ����� ��
		vector<Citem*>::iterator item_pos;
		item_pos=find_if(m_vecPublicItem.begin(),m_vecPublicItem.end(),Check_Bar(m_pCBPlayer->GetRect(),m_wBump));		//�÷��̾� 1�� �浹�� ������ �ֳ� üũ
		if(item_pos!=m_vecPublicItem.end())
		{				
			if(!(*item_pos)->GetStore())		//������ �Ұ����� ������
			{				
				(*item_pos)->UseItem();
			}
			else if(m_vecAbtainItem.size() < 5)		//�������� 5���� �Ծ����� �Ѵ�.
				m_vecAbtainItem.push_back(*item_pos);
			m_vecPublicItem.erase(item_pos);
		}		
	}
	
	//��ֹ��� �� �浹 ó�� (�ؾ���)
	if(turtle.GetLive())
	{
		for(UINT i=0;i< m_deqBall.size();i++)
		{
			CPoint2 ObjectCenter,ballCenter;	//��ֹ��� �߽�,���� �߽�
			SDL_Rect turtleRect = turtle.GetRect();
			SDL_Rect ballRect   = m_deqBall[i].GetRect();
			

			//�浹�� ���
			if(collision(turtleRect,ballRect))		//��ֹ��� ���� �浹�� ���
			{
				vector<int>::iterator itor;
				//�ź��̿� �浹�ߴ� ���� �����Ⱓ���� �ٽ� �ѵ� �ȵǰ���
				itor = find(turtle.ball_id.begin(),turtle.ball_id.end(),m_deqBall[i].GetID());
				if(itor != turtle.ball_id.end())
				{
					//�浹 �� �� �ִٸ�
					continue;
				}

				ObjectCenter.x = (float)turtleRect.x+50;	//��ֹ��� �߽��� ��� �� x
				ObjectCenter.y = (float)turtleRect.y+30;	//��ֹ��� �߽��� ��� �� y
				ballCenter = m_deqBall[i].GetPos();			//���� �߽�
				
				double tmp1=ballCenter.x - ObjectCenter.x;
				double tmp2=ballCenter.y - ObjectCenter.y;
				
				int degree=atan2(tmp2,tmp1)* 180 / 3.14159265;
							
				if(degree > -135 && degree <= -45)			//���� ����
				{
					m_deqBall[i].InverseVec(CPoint2(1.0f,-1.0f));
				}
				else if(degree > -45 && degree <= 45) //������ ����
				{
					m_deqBall[i].InverseVec(CPoint2(-1.0f,1.0f));
				}
				else if(degree > 45 && degree <= 135)		//�Ʒ� ����
				{
					m_deqBall[i].InverseVec(CPoint2(1.0f,-1.0f));
				}
				else if(degree > 135 || degree <= -135)	//���ʿ� ����
				{
					m_deqBall[i].InverseVec(CPoint2(-1.0f,1.0f));
				}
				turtle.ball_id.push_back(m_deqBall[i].GetID());
			}
		}
	}
}

//������ ������Ű�� �Լ�.
void CPlayState::AddScore(deque<CBlock>::iterator pos, int &score)
{
	//���� ����
		switch(pos->GetKind()) //0 : �� 1:�� 2:���� 3.�� 4.��
		{
			case 0:				
				Mix_PlayChannel(-1, m_wChiken, 0); 
				break;
			case 1:
				Mix_PlayChannel(-1, m_wDog, 0); 
				break;
			case 2:
				Mix_PlayChannel(-1, m_wPig, 0); 
				break;
			case 3:
				Mix_PlayChannel(-1, m_wCow, 0); 
				break;
			case 4:
				Mix_PlayChannel(-1, m_wSheep, 0); 
				break;
			case 5:
				Mix_PlayChannel(-1, m_wLion, 0); 
				break;
		}		
		score+=30;
}
//���� ������Ʈ
void CPlayState::UpdateScore()
{
	if(m_ReachScore > m_score)
	{
		m_score+=2;
	}
	else if(m_ReachScore < m_score)
	{
		m_score-=2;
	}	
	int PlayerNum= m_otherPlayer.GetCurrPlayerNum();
	m_otherPlayer.board[PlayerNum].SetReachScore(m_ReachScore);
}


//���ݴ��� ������ ó�� ���μ���
inline void CPlayState::TargetItemProcess()
{
	ITEMINFO *attackedItem = m_otherPlayer.GetTargetItemInfo();
	
			
	Citem * useItem=NULL;
	
	//Ŭ���̾�Ʈ���� ������ �������� ���� �Ŀ� �ش� ������ �ʱ�ȭ.
	switch(attackedItem->itemNumber)
	{
		case LOWBALL:
			useItem = new RawBall(SDL_Rect(),NULL,&m_deqBall);
			useItem->UseItem();		//�������� ���
			break;
		case CONTROL:
			useItem = new ControlBar(SDL_Rect(),NULL,m_pCBPlayer);
			useItem->UseItem();
			break;
		case LASER:
			useItem = new Laser(SDL_Rect(),NULL,&m_bLaserOn,&m_cpLaserPos,m_pCBPlayer);
			useItem->UseItem();
			break;
		case HIGHBALL:
			useItem = new HighBall(SDL_Rect(),NULL,&m_deqBall);
			useItem->UseItem();
			break;
		case HIDEBALL:
			useItem = new SPhideBall(SDL_Rect(),NULL,&m_deqBall);
			useItem->UseItem();
			break;
		case SPEEDUP:
			useItem = new SPupBall(SDL_Rect(),NULL,&m_deqBall);
			useItem->UseItem();
			break;
		case SPEEDDOWN:
			useItem = new SPdownBall(SDL_Rect(),NULL,&m_deqBall);
			useItem->UseItem();
			break;
		case STOPBALL:
			useItem = new SPstopBall(SDL_Rect(),NULL,&m_deqBall);
			useItem->UseItem();
			break;
		case ADDBALL2:
			useItem = new AddBall(SDL_Rect(),NULL,&m_deqBall,2,&m_addBlockLine);
			useItem->UseItem();
			break;
		case ADDBALL4:
			useItem = new AddBall(SDL_Rect(),NULL,&m_deqBall,4,&m_addBlockLine);
			useItem->UseItem();
			break;
		case OBSTARCLE:
			useItem = new Nbar_block(SDL_Rect(),NULL,&turtle);
			useItem->UseItem();
			break;
	}
	if(useItem!=NULL)
		delete useItem;

	memset(attackedItem,-1,sizeof(*attackedItem));//������ �ӽ� ���� �ʱ�ȭ
}

//������ �ڽ� �׸���
inline void CPlayState::DrawItemBox()
{
	if(!m_vecAbtainItem.empty())
	{
		int i=(int)m_vecAbtainItem.size();
		for(int j=0;j<i;j++)
		{
			int coordX;

			switch(j)
			{
			case 0: coordX = 180; break;
			case 1: coordX = 234; break;
			case 2: coordX = 289; break;
			case 3: coordX = 344; break;
			case 4: coordX = 399; break;
			}
			m_vecAbtainItem[j]->SetRect(coordX,718);		//1p ������ �ڽ��� �׸� ��ġ ����
		}		
		
		for_each(m_vecAbtainItem.begin(),m_vecAbtainItem.end(),mem_fun(&Citem::Draw));
	}		
}

//�Ⱥμ����� ��� �߰�
void CPlayState::NonBrokenBlockCreate()
{
	if(m_addBlockLine > 5)		//�߰��Ǵ� �ִ� ���� �� 5
		return;	

	int idx = 0;
	float pos_x = LEFTLINE,pos_y=133;

	//�⺻ ����� y���� ��ġ�� ����
	if(!m_deqBlock.empty())
	{
		idx = m_deqBlock.front().GetBlockNum()+1;		//������ ����� �ѹ��� +1�� ���� �߰��� ����� �ε���
		for_each(m_deqBlock.begin(),m_deqBlock.end(),mem_fun_ref(&CBlock::BlockDown));
	}
	//15�� ����
	for(int i=0;i<15;i++)
	{
		m_deqBlock.push_front(CBlock(CPoint2(pos_x,pos_y),idx));
		m_deqBlock.front().SetKind(6);	//�� �μ����� ������
		m_deqBlock.front().SetSheet(180);
		pos_x+=30;
	}
	m_addBlockLine++;
	
	/*//���� ��� ���� �߰�.
	for(UINT i=0;i<m_deqBall.size();i++)
	{
		m_deqBall[i].SetCollisionLine(30);
	}*/
	m_otherPlayer.NonBrokenBlockCreate();
}


void CPlayState::CreateMap(int map)
{
	int Pos_X=LEFTLINE,Pos_Y=133;          //��� ����� X���� Y��

	int BC_i = 0, BC_j = 0, BC_k = 0;    //����� ���� ������ �����ϴ� ���ڰ�
	int temp_box = 0;                    
	bool Block_Create = true, Block_MCreate = false;    // ����� ���θ� ����

	int blockIdx = 0;					//�����Ǵ� ����� �ε���


	FILE *fp;
	char mapName[5];					//�� �̸�
	char buffer[180];					//���� �о� �� ����
	
	//�⺻���� �� �ϴ� ����
	for(int i=0;i<12;i++)
	{
		for(int j=0;j<15;j++)
		{			
			m_deqBlock.push_front(CBlock(CPoint2((float)Pos_X,(float)Pos_Y),blockIdx++));
			Pos_X+=30;		//�� ũ�� 30�� ����
		}
		Pos_X=LEFTLINE;
		Pos_Y+=30;
	}

	if(map == 0) // �⺻���� �׸� ������ �� ����
 	{		
		return;
	}

	fp = fopen("../../Game/maps/maps.txt","r");
	if(fp == NULL)
	{
		MessageBox(NULL,"���� ���⸦ ���� �߽��ϴ�.","����",MB_OK);
		exit(0);
	}	
	//���� �б� ����
	while(1)
	{
		//�� Ÿ��Ʋ �б�
		fgets(mapName,sizeof(mapName),fp);
		int mapNum = atoi(mapName);
		if(mapNum == map)
		{
			int idx = 0;
			//���� ��ŭ �о��
			for(int i=0;idx<180;i++)
			{
				char ch = fgetc(fp);
				if(ch == 10)		//�ٹٲ��� ��� �н�~
					continue;
				buffer[idx++]=ch;
			}
			break;
		}
		else
		{
			char ch=0;
			while(1)
			{
				if(ch=='@')
				{
					fseek(fp,2,SEEK_CUR);
					break;
				}
				ch = fgetc(fp);
			}
		}		
	}	
	//���� ���۷� �� ����
	for(UINT i=0,j=m_deqBlock.size()-1;i<m_deqBlock.size();i++,j--)
	{
		if(buffer[i]=='0')
			m_deqBlock[j].SetState(false);
	}
	fclose(fp);
}

//argument ����
void CPlayState::SetArgument(Argument *arg)
{
	pArgs = arg;
}

