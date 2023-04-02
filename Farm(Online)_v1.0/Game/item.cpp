#include "item.h"
#include "playstate.h"


Citem::Citem(SDL_Rect item_pos, SDL_Surface *sheet)
{
	r_item=item_pos;
	item_sheet=sheet;

	screen = SDL_GetVideoSurface();		//주 화면의 메모리 포인터 가져오기

	sheet_rect.x=0;		//아이템 종류에 따른 시트에서 그림 가져오기
	sheet_rect.y=0;		
	sheet_rect.w=30;
	sheet_rect.h=30;
}
Citem::~Citem()
{
	if(soundEffect!=NULL)
		Mix_FreeChunk(soundEffect);
}
void Citem::Draw()
{
	SDL_BlitSurface(item_sheet, &sheet_rect, screen, &r_item);
}
void Citem::Update()
{
	r_item.y+=2;	//샘플 5정도의 속도로 내려온다.
}
//-------------------------------------------------------------------

AddBall::AddBall(SDL_Rect item_pos, SDL_Surface *sheet,deque<CBall> *ball, int idx,int *line): Citem(item_pos,sheet)
{
	balls=ball;

	this->idx=idx;		//볼 갯수

	if(idx == 2)
	{
		sheet_rect.x = 0; itemNum = ADDBALL2;
	}
	else
	{
		sheet_rect.x = 30; itemNum = ADDBALL4;
	}
	store = true;
	topLine = line;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/x24.wav");
}

void AddBall::UseItem()
{
	
	int k=(int) ((*balls).size()*idx-(*balls).size());
	for(int i=0;i<k;i++)
	{
		if((*balls).size() > 4)
			break;
		//(*balls).back().SetState(1);
		(*balls).push_back(CBall((*balls).back().GetPos(),(*balls).back().GetSurface()));
		(*balls).back().SetState(1);	
		(*balls).back().SetCollisionLine(topLine);		
	}	

	Mix_PlayChannel(-1, soundEffect, 0);
}

///----------------------------------
Bone::Bone(SDL_Rect item_pos, SDL_Surface *sheet,std::deque<CBall> *ball,int *pscore) : Citem(item_pos,sheet)
{
	balls=ball;
	
	scores=pscore;
	
	sheet_rect.x = 450;

	store = false;
	itemNum = BONE;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/born.wav");
}
void Bone::UseItem()
{
	CBall tmp;
			
	*scores-=100;
	tmp=(*balls).back();
	(*balls).clear();
	(*balls).push_back(tmp);
		

	Mix_PlayChannel(-1, soundEffect, 0);
}
//--------점수 증가----------
PlusPoint::PlusPoint(SDL_Rect item_pos, SDL_Surface *sheet, int *score, int point): Citem(item_pos,sheet)
{
	this->scores=score;	
	this->point=point;

	if(point == 100)		//100점 추가 일 때
		sheet_rect.x = 420;
	else
		sheet_rect.x = 390;
	itemNum = PLUS100;

	store = false;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/capsule100.wav");
}

void PlusPoint::UseItem()
{	
	*scores+=point;
	
	Mix_PlayChannel(-1, soundEffect, 0);
}
////-----------레이저-----------
Laser::Laser(SDL_Rect item_pos,SDL_Surface *sheet,bool *laser,CPoint2 *pos,CBar *bar) : Citem(item_pos,sheet)
{
	this->bar=bar;
	lasers=laser;
	this->pos=pos;

	sheet_rect.x = 120;

	itemNum = LASER;

	store = true;

	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/laser.wav");
}
void Laser::UseItem()
{
	*lasers=true;
	pos->x=(float)bar->GetRect().x+43;
	pos->y=bar->GetRect().y;

	Mix_PlayChannel(-1, soundEffect, 0);
}
Laser::~Laser()
{
	*lasers=false;		//소멸할때 변수를 안그리게 함.
}
//---------------------------




//---볼 각도------------------------------
RawBall::RawBall(SDL_Rect item_pos, SDL_Surface *sheet, deque<CBall> *ball): Citem(item_pos,sheet)
{
	balls=ball;	

	sheet_rect.x = 90;

	store = true;
	itemNum = LOWBALL;
	
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/low_angle.wav");
}

HighBall::HighBall(SDL_Rect item_pos, SDL_Surface *sheet,deque<CBall> *ball): Citem(item_pos,sheet)
{
	balls=ball;

	sheet_rect.x = 60;

	store = true;
	itemNum = HIGHBALL;

	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/high_angle.wav");
}

void RawBall::UseItem()
{
    for (UINT i=0; i<(*balls).size(); ++i) 
	{
		(*balls)[i].SetItemVector(0);
	}

	Mix_PlayChannel(-1, soundEffect, 0);	
}

void HighBall::UseItem()
{
	for (UINT i=0; i<(*balls).size(); ++i) 
	{
			(*balls)[i].SetItemVector(1);
	}
	
	Mix_PlayChannel(-1, soundEffect, 0);
}

///---볼 속도-------------------------------------
SPupBall::SPupBall(SDL_Rect item_pos, SDL_Surface *sheet,deque<CBall> *ball): Citem(item_pos,sheet)
{
	balls=ball;

	sheet_rect.x = 150;

	store = true;
	itemNum = SPEEDUP;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/speed_up.wav");
}

SPdownBall::SPdownBall(SDL_Rect item_pos, SDL_Surface *sheet,deque<CBall> *ball): Citem(item_pos,sheet)
{
	balls=ball;
	
	sheet_rect.x = 270;

	store = true;
	itemNum = SPEEDDOWN;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/slow.wav");
}

void SPupBall::UseItem()
{
	 if((*balls).back().GetSpeed() < 10)		//볼의 최대속도 10
	 {
		for (UINT i=0; i<(*balls).size(); ++i) 
		{
			(*balls)[i].GetSpeed() *= 1.3f;
		}
	 }
	 
	 Mix_PlayChannel(-1, soundEffect, 0);
}
void SPdownBall::UseItem()
{
	 if((*balls).back().GetSpeed() > 3)
	 {
		for (UINT i=0; i<(*balls).size(); ++i) 
		{
			(*balls)[i].GetSpeed() -= 1.3f;
		}
	 }
	 
	 Mix_PlayChannel(-1, soundEffect, 0);
}

///---볼 상태(멈춤, 안보임)-----------------------

SPstopBall::SPstopBall(SDL_Rect item_pos, SDL_Surface *sheet, deque<CBall> *ball): Citem(item_pos,sheet)
{
	balls=ball;

	sheet_rect.x = 300;

	store = true;
	itemNum = STOPBALL;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/stop.wav");
}

SPhideBall::SPhideBall(SDL_Rect item_pos, SDL_Surface *sheet, deque<CBall> *ball): Citem(item_pos,sheet)
{
	balls=ball;
	
	sheet_rect.x = 180;

	store = true;
	itemNum = HIDEBALL;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/hide.wav");
}

void SPstopBall::UseItem()
{
	for (UINT i=0; i<(*balls).size(); ++i) 
	{
		(*balls)[i].GetState() = 3;		//볼의 상태를 멈춤 상태로
	}
	
	 Mix_PlayChannel(-1, soundEffect, 0);
}

void SPhideBall::UseItem()
{
	 for (UINT i=0; i<(*balls).size(); ++i)
	 {
		(*balls)[i].GetView() = 0;
	 }
	 
	 Mix_PlayChannel(-1, soundEffect, 0);
}


///---컨트롤 쇼크 ----------------------------------------

ControlBar::ControlBar(SDL_Rect item_pos, SDL_Surface *sheet,CBar *player): Citem(item_pos,sheet)
{
	players=player;
	
	sheet_rect.x = 210;

	store = true;
	itemNum = CONTROL;

	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/control.wav");
}

void ControlBar::UseItem()
{
	//플레이어의 컨트롤 쇼크를 0으로 
	players->UseControlShock();
	 
	 Mix_PlayChannel(-1, soundEffect, 0);
}


///---장애물 함수 -------------------------------------------
Nbar_block::Nbar_block(SDL_Rect item_pos,SDL_Surface *sheet,CTurtle *turtle) : Citem(item_pos,sheet) 
{
	pTurtle = turtle;
	sheet_rect.x = 240;
	store = true;
	itemNum = OBSTARCLE;
	//사운드 이펙트 로드
	soundEffect=Mix_LoadWAV("../../Game/music/tuttle.wav");
}

void Nbar_block::UseItem()
{
	pTurtle->SetLive(true);

	Mix_PlayChannel(-1, soundEffect, 0);
}
