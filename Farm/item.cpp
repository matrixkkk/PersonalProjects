#include "item.h"
#include "playstate.h"


Citem::Citem(SDL_Rect item_pos, SDL_Surface *sheet, int _own,int _kind)
{
	r_item=item_pos;
	item_sheet=sheet;
	own=_own;
	kind=_kind;						//랜덤으로 아이템 생성.
	screen = SDL_GetVideoSurface();		//주 화면의 메모리 포인터 가져오기

	if(own==2)
	{
		sheet_rect.x=kind*30;		//아이템 종류에 따른 시트에서 그림 가져오기
		sheet_rect.y=30;		
		sheet_rect.w=30;
		sheet_rect.h=30;
	}
	else
	{				
		sheet_rect.x=kind*30;		//아이템 종류에 따른 시트에서 그림 가져오기
		sheet_rect.y=own*30;		
		sheet_rect.w=30;
		sheet_rect.h=30;
	}

	ball_high=Mix_LoadWAV("music/고각2.wav");	
	ball_law=Mix_LoadWAV("music/저각.wav");
	ball_spup=Mix_LoadWAV("music/스피드업.wav");
	ball_spdown=Mix_LoadWAV("music/스피드업.wav");   // 스피드 다운음악이없네
	ball_stop=Mix_LoadWAV("music/하이드wav");        // 스탑 음악이 없네
	ball_hide=Mix_LoadWAV("music/하이드.wav");
	key_revers=Mix_LoadWAV("music/키보드.wav");
	ball_saperate=Mix_LoadWAV("music/캡슐.wav");
	ball_die=Mix_LoadWAV("music/공떨구면.wav");
	ball_point=Mix_LoadWAV("music/캡슐1.wav");
	key_raiser=Mix_LoadWAV("music/레이져.wav");

	ball_bar=Mix_LoadWAV("music/sheep.wav");

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

AddBall::AddBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;		//볼 갯수
		
}

void AddBall::UseItem()
{
	if(own==1)
	{
		int k=(*balls1).size()*idx-(*balls1).size();
		for(int i=0;i<k;i++)
		{
			if((*balls1).size() > 4)
				break;
			(*balls1).back().SetState(1);
			(*balls1).push_back(CBall((*balls1).back().GetPos(),(*balls1).back().GetSurface()));
			(*balls1).back().SetState(1);		
		}
	}
	else if(own==0)
	{
		int k=(*balls2).size()*idx-(*balls2).size();
		for(int i=0;i<k;i++)
		{
			if((*balls2).size() > 4)
				break;
			(*balls2).back().SetState(1);
			(*balls2).push_back(CBall((*balls2).back().GetPos(),(*balls2).back().GetSurface()));
			(*balls2).back().SetState(1);			
		}
	}

	Mix_PlayChannel(-1, ball_saperate, 0);
}

///----------------------------------
Bone::Bone(SDL_Rect item_pos, SDL_Surface *sheet, int owns, int _kind, std::deque<CBall> *ball1,deque<CBall> *ball2,int *pscore1,int *pscore2) : Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	scores1=pscore1;
	scores2=pscore2;
}
void Bone::UseItem()
{
	CBall tmp;
	
	if(own==0)		//1p가 먹었을 때
	{		
		*scores1-=100;
		tmp=(*balls1).back();
		(*balls1).clear();
		(*balls1).push_back(tmp);
	}
	else if(own==1) //2p가 먹었을 때
	{		
		*scores2-=100;
		tmp=(*balls2).back();
		(*balls2).clear();
		(*balls2).push_back(tmp);
	}

	Mix_PlayChannel(-1, ball_die, 0);
}
//--------점수 증가----------
PlusPoint::PlusPoint(SDL_Rect item_pos, SDL_Surface *sheet, int owns, int _kind, int *score1, int *score2, int point): Citem(item_pos,sheet,owns,_kind)
{
	this->scores1=score1;
	this->scores2=score2;
	this->point=point;
}

void PlusPoint::UseItem()
{
	if(own==0)		//1p가 먹었을 때
	{		
		*scores1+=point;
	}
	else if(own==1) //2p가 먹었을 때
	{		
		*scores2+=point;
	}

	Mix_PlayChannel(-1, ball_point, 0);
}
////-----------레이저-----------
Laser::Laser(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,bool *laser,CPoint2 *pos,CBar *bar) : Citem(item_pos,sheet,owns,_kind)
{
	this->bar=bar;
	lasers=laser;
	this->pos=pos;
}
void Laser::UseItem()
{
	*lasers=true;
	pos->x=bar->GetRect().x+43;
	pos->y=bar->GetRect().y;

	Mix_PlayChannel(-1, key_raiser, 0);
}
Laser::~Laser()
{
	*lasers=false;		//소멸할때 변수를 안그리게 함.
}
//---------------------------




//---볼 각도------------------------------
RawBall::RawBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;	
}

HighBall::HighBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;	
}

void RawBall::UseItem()
{
	if(own == 1)
	{
	    for (int i=0; i<(*balls1).size(); ++i) {
			(*balls1)[i].SetItemVector(0);
		}
	}
	else if(own == 0)
	{
		for (int i=0; i<(*balls2).size(); ++i) {
			(*balls2)[i].SetItemVector(0);
		}
	}

	Mix_PlayChannel(-1, ball_law, 0);	
}

void HighBall::UseItem()
{
	if(own == 1)
	{
		for (int i=0; i<(*balls1).size(); ++i) {
			(*balls1)[i].SetItemVector(1);
		}
	}
	else if(own == 0)
	{
		for (int i=0; i<(*balls2).size(); ++i) {
			(*balls2)[i].SetItemVector(1);
		}
	}

	Mix_PlayChannel(-1, ball_high, 0);
}

///---볼 속도-------------------------------------
SPupBall::SPupBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;
}

SPdownBall::SPdownBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;	
}

void SPupBall::UseItem()
{
	 if(own == 1)
	 {
		if((*balls1).back().GetSpeed() < 7)
		{
			for (int i=0; i<(*balls1).size(); ++i) {
				(*balls1)[i].GetSpeed() += 2;
			}
		}
	 }
	 else if(own == 0)
	 {
		if((*balls2).back().GetSpeed() < 7)
		{
			for (int i=0; i<(*balls2).size(); ++i) {
				(*balls2)[i].GetSpeed() += 2;
			}
		}
	 }

	 Mix_PlayChannel(-1, ball_spup, 0);
}
void SPdownBall::UseItem()
{
	 if(own == 1)
	 {
		if((*balls1).back().GetSpeed() > 3)
		{
			for (int i=0; i<(*balls1).size(); ++i) {
				(*balls1)[i].GetSpeed() -= 2;
			}
		}
	 }
	 else if(own == 0)
	 {
		if((*balls2).back().GetSpeed() > 3)
		{
			for (int i=0; i<(*balls2).size(); ++i) {
				(*balls2)[i].GetSpeed() -= 2;
			}
		}
	 }

	 Mix_PlayChannel(-1, ball_spdown, 0);
}

///---볼 상태(멈춤, 안보임)-----------------------

SPstopBall::SPstopBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;	
}

SPhideBall::SPhideBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;
}

void SPstopBall::UseItem()
{
	 if(own == 1)
	 {
		for (int i=0; i<(*balls1).size(); ++i) {
			(*balls1)[i].GetState() = 3;
		}
	 }
	 else if(own == 0)
	 {
		for (int i=0; i<(*balls2).size(); ++i) {
			(*balls2)[i].GetState() = 3;
		}
	 }

	 Mix_PlayChannel(-1, ball_stop, 0);
}

void SPhideBall::UseItem()
{
	 if(own == 1)
	 {
		for (int i=0; i<(*balls1).size(); ++i) {
			(*balls1)[i].GetView() = 0;
		}
	 }
	 else if(own == 0)
	 {
		for (int i=0; i<(*balls2).size(); ++i) {
			(*balls2)[i].GetView() = 0;
		}
	 }

	 Mix_PlayChannel(-1, ball_hide, 0);
}


///---컨트롤 쇼크 ----------------------------------------

ControlBar::ControlBar(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, CBar *player1, CBar *player2): Citem(item_pos,sheet,owns,_kind)
{
	players1=player1;
	players2=player2;
}

void ControlBar::UseItem()
{
	 if(own == 1)
		 players1->ControlSK() = 0;
	 else if(own == 0)
		 players2->ControlSK() = 0;

	 Mix_PlayChannel(-1, key_revers, 0);
}


///---장애물 함수 -------------------------------------------
Nbar_block::Nbar_block(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,bool *nbar, int *tmp_bar) : Citem(item_pos,sheet,owns,_kind) 
{
	nbars=nbar;
	tmp_bars=tmp_bar;
}
void Nbar_block::UseItem()
{
	*nbars=true;
	*tmp_bars=400;

	Mix_PlayChannel(-1, ball_bar, 0);
}
