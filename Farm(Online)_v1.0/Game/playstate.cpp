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
	SDL_Rect crossRect;		//교차된 부분의 rect를 구하기 위한 변수
	CPoint2 pos,p,vec;
	
	int degree;

	crossRect=block.GetRect();
	pos.x=(float)crossRect.x+15;
	pos.y=(float)crossRect.y+15;
	p=ball.GetPos();
	
	if(block.GetState()==false || block.Getbreak()==true)	//이미 파괴된 블럭일 경우 그냥 false;
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
	
	vec=ball.GetVec();		//볼의 방향 벡터를 가져온다.
	
	if(degree > -135 && degree <= -45)			//위에 맞음
	{
		ball.InverseVec(CPoint2(1.0f,-1.0f));
	}
	else if(degree > -45 && degree <= 45) //우측에 맞음
	{
		ball.InverseVec(CPoint2(-1.0f,1.0f));
	}
	else if(degree > 45 && degree <= 135)		//아래 맞음
	{
		ball.InverseVec(CPoint2(1.0f,-1.0f));
	}
	else if(degree > 135 || degree <= -135)	//왼쪽에 맞음
	{
		ball.InverseVec(CPoint2(-1.0f,1.0f));
	}
			
	return true;
}
///////////////////
//볼과 볼의 충돌을 처리하는 조건자.
template<typename T1,typename T2>
bool ball_collision( T1 &a, T2 &b)
{
	CPoint2 Nomal,center1,center2;
	if(a.GetState()==0 || b.GetState()==0)
		return false;
	if(collision(a.GetRect(),b.GetRect())==true && a.GetID()!=b.GetID())	//볼 두개가 충돌 한경우.
	{
		int ids;

		center1=a.GetPos();	//볼1의 중심을 얻어오기
		center2=b.GetPos(); //볼2의 중심을 얻어오기

		center1.x=center1.x-a.GetVec().x*a.GetSpeed();
		center1.y=center1.y-a.GetVec().y*a.GetSpeed();
		center2.x=center2.x-a.GetVec().x*a.GetSpeed();
		center2.y=center2.y-a.GetVec().y*a.GetSpeed();
		
		Nomal.x=center2.x-center1.x;
		Nomal.y=center2.y-center1.y;
		a.SetVector(Reflect_vector(Nomal,a.GetVec()));		//반사벡터를구해서 볼 벡터에 셋해준다.
		
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
/////파괴된 블럭 찾아서 지우는 조건자
bool check_break_block(CBlock &block)
{
	return !block.GetState();
}
/////바닥으로 떨어진 아이템 삭제
bool check_item(Citem *item)
{
	if(item->GetRect().y > BOTTOMLINE-20)
		return true;
	else
		return false;

}
////////////////////
//볼 삭제 조건자
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


void CPlayState::Init(CGameEngine* game) //초기화
{
	srand(unsigned int(time(NULL)));
	
	//play=true;
	m_IDReset=0;
	m_score=0;
	m_LaserFrame=0;
	m_duration = 0;
	m_addBlockLine = 0;

	SetSDLRect(m_rLaser,0,0,14,17);					//레이저 초기 위치

	//아이템 드랍 확률 셋팅
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

	//채팅을 위한 에디트 박스 생성
	SDL_Rect chatEditPos;
	SetSDLRect(chatEditPos,500,718,500,26);
	chatEditBox.CreateEditBox(chatEditPos,READ_WRITE);	//에디트 박스 생성

	//----------이미지 관련 초기화----------------------------------
	//이미지 버퍼 생성
		
	SDL_Surface* temp = SDL_LoadBMP("../../Game/image/layout.bmp"); //배경 그림 load
	m_sBackGround = SDL_DisplayFormat(temp);

	//블럭 이미지 로딩
	temp=SDL_LoadBMP("../../Game/image/block_sheet.bmp");
	m_sBlock=SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sBlock, SDL_SRCCOLORKEY, SDL_MapRGB( m_sBlock->format, 0, 0, 0) );
	
	//키보드 재반복 허용
	SDL_EnableKeyRepeat(10,SDL_DEFAULT_REPEAT_INTERVAL);
	//-------------------블럭 셋팅-----------------------
	
	CreateMap(this->pArgs->mapIndex);		
	//-----------------Bar 셋팅----------------------------
	m_pCBPlayer= new CBar(CPoint2(LEFTLINE+180,BOTTOMLINE-20));

	//-----------------장애물 셋팅-------------------------
	turtle.Init(game->screen);				//거북이 초기화


	//이미지로딩
	temp = SDL_LoadBMP("../../Game/image/bar_1p.bmp");		//bar 이미지 로딩
	m_sBar = SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sBar, SDL_SRCCOLORKEY, SDL_MapRGB( m_sBar->format, 255, 0, 0) );

	m_pCBPlayer->Init(m_sBar);						//로딩한 이미지의 표면의 포인터를 각각의 bar 객체에 넘겨준다.

	//축소된 bar 로딩
	temp = SDL_LoadBMP("../../Game/image/bar2_1.bmp");
	imageBuffer[4] = SDL_DisplayFormat(temp);
	SDL_SetColorKey( imageBuffer[4], SDL_SRCCOLORKEY, SDL_MapRGB( imageBuffer[4]->format, 255, 0, 0) );
	
	//----------------Ball 셋팅----------------------------
	temp = SDL_LoadBMP("../../Game/image/ball.bmp");		//ball 이미지 로딩
	m_sBall=SDL_DisplayFormat(temp);
	m_deqBall.push_back(CBall(CPoint2(LEFTLINE+55,BOTTOMLINE-25),m_sBall,&m_minCollisionLine));	//ball 컨테이너에 1개씩 넣어준다. 처음에는 공1개로 시작하므로

	//블럭 파괴 애니메이션 셋팅
	temp = SDL_LoadBMP("../../Game/image/block_broken.bmp");
	m_sBlockAnim=SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sBlockAnim, SDL_SRCCOLORKEY, SDL_MapRGB( m_sBlockAnim->format, 133, 176, 182) );

	//스코어 시트 이미지 로딩
	temp = SDL_LoadBMP("../../Game/image/number_sheet.bmp");
	m_sScore= SDL_DisplayFormat(temp);
	SDL_SetColorKey( m_sScore, SDL_SRCCOLORKEY, SDL_MapRGB( m_sScore->format, 0, 0, 0) );

	//아이템 시트 이미지 로딩
	temp = SDL_LoadBMP("../../Game/image/item_sheet.bmp");
	m_sItemSheet=SDL_DisplayFormat(temp);
	SDL_SetColorKey(m_sItemSheet , SDL_SRCCOLORKEY, SDL_MapRGB( m_sItemSheet->format, 0, 250, 250) );

	//레이저 시트 이미지 로딩
	temp = SDL_LoadBMP("../../Game/image/laser_sheet.bmp");
	m_sLaser=SDL_DisplayFormat(temp);
	SDL_SetColorKey(m_sLaser , SDL_SRCCOLORKEY, SDL_MapRGB( m_sLaser->format, 255, 0, 0) );

	//음악 로딩
	m_mBgm[0] = Mix_LoadMUS("../../Game/music/BGM.mp3");
	m_mBgm[1] = Mix_LoadMUS("../../Game/music/BGM2.mp3");
	if(m_mBgm[0]==NULL || m_mBgm[1]==NULL)
	{
		cout<<"음악 로딩 실패"<<endl;
	}
	

	//음악 재생(정신없어서 임시 주석)
	int randomMusic = rand()%2;
	Mix_PlayMusic( m_mBgm[randomMusic], -1 );

	//효과음 로드
	m_wCow		=Mix_LoadWAV("../../Game/music/cow.wav");
	m_wDog		=Mix_LoadWAV("../../Game/music/dog.wav");
	m_wSheep	=Mix_LoadWAV("../../Game/music/sheep.wav");
	m_wLion		=Mix_LoadWAV("../../Game/music/lion.wav");
	m_wChiken	=Mix_LoadWAV("../../Game/music/chiken.wav");
	m_wPig		=Mix_LoadWAV("../../Game/music/pig.wav");
	m_wBump		=Mix_LoadWAV("../../Game/music/ball.wav");

    //아이템 효과음 추가 해야함. 
	SDL_FreeSurface(temp);	

	//이미지 포인터들  모음
	imageBuffer[0] = m_sBlock;
	imageBuffer[1] = m_sBlockAnim;
	imageBuffer[2] = m_sBar;
	imageBuffer[3] = m_sBall;

	//다른 플레이어 처리 객체 초기화
	m_otherPlayer.init(imageBuffer,game->screen,pArgs->numOfUser,m_deqBlock,pArgs);	
	
	//수집 데이터 초기화
	memset(colData.breakenBlock,-1,sizeof(colData.breakenBlock));
	memset(&colData.itemInfo,-1,sizeof(colData.itemInfo));

	//타이머 초기화
	m_timer.StartTick();	
}

void CPlayState::Cleanup()  //소멸
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


  //컨테이너 비우기
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

//데이터 수집 함수
MYDATA* CPlayState::Collect()
{
	//볼의 정보를 저장
	for(UINT i=0;i<m_deqBall.size();i++)
	{
		colData.ballPos[i] = m_deqBall[i].GetRect();	//볼의 위치 가져와서 저장.
	}
	//player bar의 위치를 저장
	colData.barRect = m_pCBPlayer->GetRect();

	//부셔진 블록 인덱스를 저장
	memset(colData.breakenBlock,-1,sizeof(colData.breakenBlock));		//초기화 -1 은 빈곳을 의미
	
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
	//키보드 입력 처리
	Uint8 *keystate = SDL_GetKeyState(NULL);

	if ( keystate[SDLK_ESCAPE] )	//게임 종료
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
	//활성화 인 경우.
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
	else if( keystate[SDLK_SPACE] )		//볼 발사
		m_deqBall.back().SetState(1);
	
	if( keystate['a'] )			//아이템 자신에게 사용
	{
		 if(!m_vecAbtainItem.empty())
		 {
			 m_vecAbtainItem.front()->UseItem();
			 m_vecAbtainItem.pop_front();
		 }		
		 keystate['a'] = false;
	}	
	else if( keystate['1'])				//0번에게 아이템 사용
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //자신의 번호를 눌렀을 경우 처리
			 if(m_otherPlayer.GetCurrPlayerNum() == 0)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//아이템 번호
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 0;			//대상
			 }
		 }
		 keystate['1'] = false;
	}
	else if( keystate['2'])				//1번에게 아이템 사용
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //자신의 번호를 눌렀을 경우 처리
			 if(m_otherPlayer.GetCurrPlayerNum() == 1)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//아이템 번호
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 1;			//대상
			 }
		 }
		 keystate['2'] = false;
	}
	else if( keystate['3'])				//1번에게 아이템 사용
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //자신의 번호를 눌렀을 경우 처리
			 if(m_otherPlayer.GetCurrPlayerNum() == 2)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//아이템 번호
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 2;			//대상
			 }
		 }
		 keystate['3'] = false;
	}
	else if( keystate['4'])				//1번에게 아이템 사용
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //자신의 번호를 눌렀을 경우 처리
			 if(m_otherPlayer.GetCurrPlayerNum() == 3)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//아이템 번호
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 3;			//대상
			 }
		 }
		 keystate['4'] = false;
	}
	else if( keystate['5'])				//1번에게 아이템 사용
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //자신의 번호를 눌렀을 경우 처리
			 if(m_otherPlayer.GetCurrPlayerNum() == 4)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//아이템 번호
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 4;			//대상
			 }
		 }
		 keystate['5'] = false;
	}
	else if( keystate['6'])				//1번에게 아이템 사용
	{
		if(!m_vecAbtainItem.empty())
		 {
			 //자신의 번호를 눌렀을 경우 처리
			 if(m_otherPlayer.GetCurrPlayerNum() == 5)
			 {
				 m_vecAbtainItem.front()->UseItem();
				 m_vecAbtainItem.pop_front();
			 }
			 else
			 {
				 colData.itemInfo.itemNumber = m_vecAbtainItem.front()->GetItemNum();	//아이템 번호
				 m_vecAbtainItem.pop_front();
				 colData.itemInfo.targetPlayer = 5;			//대상
			 }
		 }
		 keystate['6'] = false;
	}
	
	/*//테스트용
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
	
	//30초 타이머 
	if(m_duration > 20000.0f)
	{
		m_duration = 0;
		m_timer.StartTick();	//타이머 재시작
		//안부셔지는 블록 생성.
		NonBrokenBlockCreate();
	}

	//라인 검사
	m_minCollisionLine = m_addBlockLine*30+148;
     //게임 종료 조건 (일시적인 주석 처리 
	/*if(m_vecAbtainItem.empty())	//블럭이 비었을 경우
	{
		game->map=ReachScore1>ReachScore2 ? 0:1;
		game->ChangeState( &gameOver );
		return ;
	}*/

	//레이저 이동
	if(m_bLaserOn)			//1p 레이저가 사용된 경우
	{	
		m_rLaser.x=(Sint16)m_cpLaserPos.x;
		m_cpLaserPos.y-=20;
		m_rLaser.y=(Sint16)m_cpLaserPos.y;
		
		m_rSheetLaser.x=m_LaserFrame*14;
		m_rSheetLaser.y=0;
		m_rSheetLaser.w=14;
		m_rSheetLaser.h=170;
		if(m_LaserFrame < 9) //레이저 프레임 증가. 
			m_LaserFrame++;
	}	

	//레이저 끄기 
	if(m_cpLaserPos.y < TOPLINE)
		m_bLaserOn=false;


    // 장애물의 이동
	turtle.Move();	

	
			
	if(m_deqBall.back().GetState()==false)
		m_deqBall.back().SetBarPos(m_pCBPlayer->GetRect());		//1p의 위치를 가져와서 볼에게 넘김
	
	for_each(m_deqBall.begin(),m_deqBall.end(),mem_fun_ref(&CBall::Move));		//1p 볼 이동
   	
	CheckColision();	//충돌 체크 함수


	//아이템의 이동	 
	if(!m_vecPublicItem.empty())
		for_each(m_vecPublicItem.begin(),m_vecPublicItem.end(),mem_fun(&Citem::Update));

	//블럭 파괴 애니메이션을 위한 프레임 업데이트
	for_each(m_deqBlock.begin(),m_deqBlock.end(),mem_fun_ref(&CBlock::update));
	
	//볼 아웃되었을 경우 볼 없애 지는거
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
			m_deqBall.back().SetCollisionLine(&m_minCollisionLine);	//top라인 추가.
		}
	}
	
	/////////////////////////////////////////////
	//바닥으로 떨어진 아이템을 아이템 목록에서 삭제
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

	m_IDReset++;			//ID 리셋 카운터 증가.
	if(m_IDReset==30)	//0.5초가 되면 ID가 모두 리셋됨.
	{
		for_each(m_deqBall.begin(),m_deqBall.end(),mem_fun_ref(&CBall::ID_Reset));
		m_IDReset=0;
	}	

	 UpdateScore();												//점수 업데이트

	 //////////////////////////////////
	 //레이저와 블록 충돌 체크
	 ///////////////////////////////////

	 if(m_bLaserOn)		//레이저가 사용중일 때만 충돌 체크
	 {
		 deque<CBlock>::iterator iter_block;
		 iter_block = find_if(m_deqBlock.begin(),m_deqBlock.end(),Check_Bar(m_rLaser,m_wBump));
		 if(iter_block!=m_deqBlock.end())
		 {		
			  //부셔진 블록 목록 저장
			if(m_brokenBlockIdx.size() < 10)		//10개 까지만 저장
				m_brokenBlockIdx.push_back(iter_block->GetBlockNum());	//블록의 부셔진 idx를 가져와서 컨테이너에 저장
			else														//10개 이상일 경우 front를 제거하고 push_back
			{
				m_brokenBlockIdx.pop_front();							//제일 오래된 블록 인덱스 제거
				m_brokenBlockIdx.push_back(iter_block->GetBlockNum());	//인덱스 저장
			}
			AddScore(iter_block,m_ReachScore);		//점수 증가.
			m_deqBlock.erase(iter_block);
		 }
	 }

	 //다른 플레이어 게임 처리
	 m_otherPlayer.SetPlayersInform(game);
	 TargetItemProcess();
}

void CPlayState::Draw(CGameEngine* game)
{	  
  SDL_BlitSurface(m_sBackGround, NULL, game->screen, NULL);			 //배경 그리기
  //SDL_SoftStretch(m_sBackGround,NULL,game->screen,&des);			 //축소 그리기.
  for_each(m_deqBlock.begin(),m_deqBlock.end(),BlockDraw(m_sBlock,m_sBlockAnim));//블럭 그리기
  for_each(m_deqBall.begin(),m_deqBall.end(),mem_fun_ref(&CBall::Draw));	 //for_each 알고리즘으로 m_deqBall의 모든 원소 그리기
  
  //아이템 생성된거 그리기
  if(!m_vecPublicItem.empty())
	for_each(m_vecPublicItem.begin(),m_vecPublicItem.end(),mem_fun(&Citem::Draw));		
  
  //바 그리기
  m_pCBPlayer->Draw(game);												//1p Bar Draw
  
  //장애물 그리기
  turtle.Draw();

  //다른 플레이어 화면 그리기
  m_otherPlayer.Draw();

  //아이템 박스 그리기
  DrawItemBox();

  //레이저 Draw
  if(m_bLaserOn)
  {
	  SDL_BlitSurface(m_sLaser, &m_rSheetLaser, game->screen, &m_rLaser);
  }  

  //chat입력 창 커서 그리기
  chatEditBox.DrawEditBox(game->screen);
}

//충돌 감지 알고리즘
void CPlayState::CheckColision()
{
	int itemDropRate=0;				//아이템 드랍률.

	deque<CBlock>::iterator iter_pos;
	deque<CBall>::iterator  iter_ball;
	
	//블럭과 볼의 충돌
	iter_pos=find_first_of(m_deqBlock.begin(),m_deqBlock.end(),m_deqBall.begin(),m_deqBall.end(),check_collision);		//충돌 체크 알고리즘
	if(iter_pos!=m_deqBlock.end())
	{			
		itemDropRate=rand()%4;		//블럭을 부셨을 때 아이템이 나올 확률 1/4
		if(itemDropRate==0)
		{
			int tableIndex = rand()% 100;
			int itemType=itemDropTable[tableIndex];	
			if(itemType==0)
				m_vecPublicItem.push_back(new Bone(iter_pos->GetRect(),m_sItemSheet,&m_deqBall,&m_ReachScore));
			else if(itemType==1)
				m_vecPublicItem.push_back(new AddBall(iter_pos->GetRect(),m_sItemSheet,&m_deqBall,2,&m_minCollisionLine)); //아이템을 아이템 박스에 추가.
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
		AddScore(iter_pos,m_ReachScore);		//점수 증가.
		
		//부셔진 블록 목록 저장
		if(m_brokenBlockIdx.size() < 10)		//10개 까지만 저장
			m_brokenBlockIdx.push_back(iter_pos->GetBlockNum());	//블록의 부셔진 idx를 가져와서 컨테이너에 저장
		else														//10개 이상일 경우 front를 제거하고 push_back
		{
			m_brokenBlockIdx.pop_front();							//제일 오래된 블록 인덱스 제거
			m_brokenBlockIdx.push_back(iter_pos->GetBlockNum());	//인덱스 저장
		}
	}	

	//파괴 처리된 블럭 컨테이너에서 삭제
	iter_pos=find_if(m_deqBlock.begin(),m_deqBlock.end(),check_break_block);	
	if(iter_pos!=m_deqBlock.end())
	{
		m_deqBlock.erase(iter_pos);
	}
	//------------------------------------
	//볼과 bar의 충돌을 체크하기 위한 for-each알고리즘
	for_each(m_deqBall.begin(),m_deqBall.end(),Check_Bar(m_pCBPlayer->GetRect(),m_wBump));
		
	//아이템 획득에 관한 처리
	if(!m_vecPublicItem.empty())	//아이템 컨테이너에 아이템이 있을 경우만 충돌 확인 (연산을 줄이기 위해)
	{
		//아이템과 bar가 충돌 하는거 체크 아이템 먹어지기 ㅎ
		vector<Citem*>::iterator item_pos;
		item_pos=find_if(m_vecPublicItem.begin(),m_vecPublicItem.end(),Check_Bar(m_pCBPlayer->GetRect(),m_wBump));		//플레이어 1과 충돌된 아이템 있나 체크
		if(item_pos!=m_vecPublicItem.end())
		{				
			if(!(*item_pos)->GetStore())		//저장이 불가능한 아이템
			{				
				(*item_pos)->UseItem();
			}
			else if(m_vecAbtainItem.size() < 5)		//아이템은 5개만 먹어지게 한다.
				m_vecAbtainItem.push_back(*item_pos);
			m_vecPublicItem.erase(item_pos);
		}		
	}
	
	//장애물과 볼 충돌 처리 (해야함)
	if(turtle.GetLive())
	{
		for(UINT i=0;i< m_deqBall.size();i++)
		{
			CPoint2 ObjectCenter,ballCenter;	//장애물의 중심,볼의 중심
			SDL_Rect turtleRect = turtle.GetRect();
			SDL_Rect ballRect   = m_deqBall[i].GetRect();
			

			//충돌한 경우
			if(collision(turtleRect,ballRect))		//장애물과 볼이 충돌한 경우
			{
				vector<int>::iterator itor;
				//거북이와 충돌했던 볼은 일정기간동안 다시 총돌 안되게함
				itor = find(turtle.ball_id.begin(),turtle.ball_id.end(),m_deqBall[i].GetID());
				if(itor != turtle.ball_id.end())
				{
					//충돌 한 적 있다면
					continue;
				}

				ObjectCenter.x = (float)turtleRect.x+50;	//장애물의 중심을 얻어 옴 x
				ObjectCenter.y = (float)turtleRect.y+30;	//장애물의 중심을 얻어 옴 y
				ballCenter = m_deqBall[i].GetPos();			//볼의 중심
				
				double tmp1=ballCenter.x - ObjectCenter.x;
				double tmp2=ballCenter.y - ObjectCenter.y;
				
				int degree=atan2(tmp2,tmp1)* 180 / 3.14159265;
							
				if(degree > -135 && degree <= -45)			//위에 맞음
				{
					m_deqBall[i].InverseVec(CPoint2(1.0f,-1.0f));
				}
				else if(degree > -45 && degree <= 45) //우측에 맞음
				{
					m_deqBall[i].InverseVec(CPoint2(-1.0f,1.0f));
				}
				else if(degree > 45 && degree <= 135)		//아래 맞음
				{
					m_deqBall[i].InverseVec(CPoint2(1.0f,-1.0f));
				}
				else if(degree > 135 || degree <= -135)	//왼쪽에 맞음
				{
					m_deqBall[i].InverseVec(CPoint2(-1.0f,1.0f));
				}
				turtle.ball_id.push_back(m_deqBall[i].GetID());
			}
		}
	}
}

//점수를 증가시키는 함수.
void CPlayState::AddScore(deque<CBlock>::iterator pos, int &score)
{
	//점수 증가
		switch(pos->GetKind()) //0 : 닭 1:개 2:돼지 3.소 4.양
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
//점수 업데이트
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


//공격당한 아이템 처리 프로세스
inline void CPlayState::TargetItemProcess()
{
	ITEMINFO *attackedItem = m_otherPlayer.GetTargetItemInfo();
	
			
	Citem * useItem=NULL;
	
	//클라이언트에서 서버에 아이템을 보낸 후에 해당 변수를 초기화.
	switch(attackedItem->itemNumber)
	{
		case LOWBALL:
			useItem = new RawBall(SDL_Rect(),NULL,&m_deqBall);
			useItem->UseItem();		//아이템의 사용
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

	memset(attackedItem,-1,sizeof(*attackedItem));//아이템 임시 버퍼 초기화
}

//아이템 박스 그리기
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
			m_vecAbtainItem[j]->SetRect(coordX,718);		//1p 아이템 박스에 그릴 위치 설정
		}		
		
		for_each(m_vecAbtainItem.begin(),m_vecAbtainItem.end(),mem_fun(&Citem::Draw));
	}		
}

//안부셔지는 블록 추가
void CPlayState::NonBrokenBlockCreate()
{
	if(m_addBlockLine > 5)		//추가되는 최대 라인 수 5
		return;	

	int idx = 0;
	float pos_x = LEFTLINE,pos_y=133;

	//기본 블록의 y값의 위치를 수정
	if(!m_deqBlock.empty())
	{
		idx = m_deqBlock.front().GetBlockNum()+1;		//마지막 블록의 넘버에 +1이 새로 추가될 블록의 인덱스
		for_each(m_deqBlock.begin(),m_deqBlock.end(),mem_fun_ref(&CBlock::BlockDown));
	}
	//15개 생성
	for(int i=0;i<15;i++)
	{
		m_deqBlock.push_front(CBlock(CPoint2(pos_x,pos_y),idx));
		m_deqBlock.front().SetKind(6);	//안 부셔지는 벽돌로
		m_deqBlock.front().SetSheet(180);
		pos_x+=30;
	}
	m_addBlockLine++;
	
	/*//볼의 블록 라인 추가.
	for(UINT i=0;i<m_deqBall.size();i++)
	{
		m_deqBall[i].SetCollisionLine(30);
	}*/
	m_otherPlayer.NonBrokenBlockCreate();
}


void CPlayState::CreateMap(int map)
{
	int Pos_X=LEFTLINE,Pos_Y=133;          //출력 장소의 X값과 Y값

	int BC_i = 0, BC_j = 0, BC_k = 0;    //모양을 위한 연산을 실행하는 인자값
	int temp_box = 0;                    
	bool Block_Create = true, Block_MCreate = false;    // 출력의 여부를 결정

	int blockIdx = 0;					//생성되는 블록의 인덱스


	FILE *fp;
	char mapName[5];					//맵 이름
	char buffer[180];					//맵을 읽어 올 버퍼
	
	//기본적인 맵 일단 생성
	for(int i=0;i<12;i++)
	{
		for(int j=0;j<15;j++)
		{			
			m_deqBlock.push_front(CBlock(CPoint2((float)Pos_X,(float)Pos_Y),blockIdx++));
			Pos_X+=30;		//블럭 크기 30씩 증가
		}
		Pos_X=LEFTLINE;
		Pos_Y+=30;
	}

	if(map == 0) // 기본적인 네모 형태의 블럭 셋팅
 	{		
		return;
	}

	fp = fopen("../../Game/maps/maps.txt","r");
	if(fp == NULL)
	{
		MessageBox(NULL,"파일 열기를 실패 했습니다.","에러",MB_OK);
		exit(0);
	}	
	//파일 읽기 루프
	while(1)
	{
		//맵 타이틀 읽기
		fgets(mapName,sizeof(mapName),fp);
		int mapNum = atoi(mapName);
		if(mapNum == map)
		{
			int idx = 0;
			//버퍼 만큼 읽어옴
			for(int i=0;idx<180;i++)
			{
				char ch = fgetc(fp);
				if(ch == 10)		//줄바꿈일 경우 패스~
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
	//읽은 버퍼로 맵 생성
	for(UINT i=0,j=m_deqBlock.size()-1;i<m_deqBlock.size();i++,j--)
	{
		if(buffer[i]=='0')
			m_deqBlock[j].SetState(false);
	}
	fclose(fp);
}

//argument 설정
void CPlayState::SetArgument(Argument *arg)
{
	pArgs = arg;
}

