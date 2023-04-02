#include "item.h"
#include "playstate.h"


Citem::Citem(SDL_Rect item_pos, SDL_Surface *sheet, int _own,int _kind)
{
	r_item=item_pos;
	item_sheet=sheet;
	own=_own;
	kind=_kind;						//�������� ������ ����.
	screen = SDL_GetVideoSurface();		//�� ȭ���� �޸� ������ ��������

	if(own==2)
	{
		sheet_rect.x=kind*30;		//������ ������ ���� ��Ʈ���� �׸� ��������
		sheet_rect.y=30;		
		sheet_rect.w=30;
		sheet_rect.h=30;
	}
	else
	{				
		sheet_rect.x=kind*30;		//������ ������ ���� ��Ʈ���� �׸� ��������
		sheet_rect.y=own*30;		
		sheet_rect.w=30;
		sheet_rect.h=30;
	}

	ball_high=Mix_LoadWAV("music/��2.wav");	
	ball_law=Mix_LoadWAV("music/����.wav");
	ball_spup=Mix_LoadWAV("music/���ǵ��.wav");
	ball_spdown=Mix_LoadWAV("music/���ǵ��.wav");   // ���ǵ� �ٿ������̾���
	ball_stop=Mix_LoadWAV("music/���̵�wav");        // ��ž ������ ����
	ball_hide=Mix_LoadWAV("music/���̵�.wav");
	key_revers=Mix_LoadWAV("music/Ű����.wav");
	ball_saperate=Mix_LoadWAV("music/ĸ��.wav");
	ball_die=Mix_LoadWAV("music/��������.wav");
	ball_point=Mix_LoadWAV("music/ĸ��1.wav");
	key_raiser=Mix_LoadWAV("music/������.wav");

	ball_bar=Mix_LoadWAV("music/sheep.wav");

}
void Citem::Draw()
{
	SDL_BlitSurface(item_sheet, &sheet_rect, screen, &r_item);
}
void Citem::Update()
{
	r_item.y+=2;	//���� 5������ �ӵ��� �����´�.
}
//-------------------------------------------------------------------

AddBall::AddBall(SDL_Rect item_pos, SDL_Surface *sheet, int owns,int _kind, deque<CBall> *ball1,deque<CBall> *ball2, int idx): Citem(item_pos,sheet,owns,_kind)
{
	balls1=ball1;
	balls2=ball2;
	this->idx=idx;		//�� ����
		
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
	
	if(own==0)		//1p�� �Ծ��� ��
	{		
		*scores1-=100;
		tmp=(*balls1).back();
		(*balls1).clear();
		(*balls1).push_back(tmp);
	}
	else if(own==1) //2p�� �Ծ��� ��
	{		
		*scores2-=100;
		tmp=(*balls2).back();
		(*balls2).clear();
		(*balls2).push_back(tmp);
	}

	Mix_PlayChannel(-1, ball_die, 0);
}
//--------���� ����----------
PlusPoint::PlusPoint(SDL_Rect item_pos, SDL_Surface *sheet, int owns, int _kind, int *score1, int *score2, int point): Citem(item_pos,sheet,owns,_kind)
{
	this->scores1=score1;
	this->scores2=score2;
	this->point=point;
}

void PlusPoint::UseItem()
{
	if(own==0)		//1p�� �Ծ��� ��
	{		
		*scores1+=point;
	}
	else if(own==1) //2p�� �Ծ��� ��
	{		
		*scores2+=point;
	}

	Mix_PlayChannel(-1, ball_point, 0);
}
////-----------������-----------
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
	*lasers=false;		//�Ҹ��Ҷ� ������ �ȱ׸��� ��.
}
//---------------------------




//---�� ����------------------------------
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

///---�� �ӵ�-------------------------------------
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

///---�� ����(����, �Ⱥ���)-----------------------

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


///---��Ʈ�� ��ũ ----------------------------------------

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


///---��ֹ� �Լ� -------------------------------------------
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
