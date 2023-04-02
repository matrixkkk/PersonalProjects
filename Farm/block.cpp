#include "block.h"


//블럭을 만드는 생성자
CBlock::CBlock(const CPoint2 &pos)
{	
	rect.x=pos.x;
	rect.y=pos.y;
	rect.w=rect.h=30;			//블럭의 사이즈 30x30
	
	this->kind=rand()%5;		//랜덤 블럭 종류
	this->state=true;			//모두 그리는 상태.
	this->frame=0;
	this->frame_delay=0;
	this->block_break=false;

	//시트 rect 설정
	sheet_rect.x=0;
	sheet_rect.y=0;
	sheet_rect.w=30;
	sheet_rect.h=30;

	switch(kind)	//블럭 종류에 따른 시트상의 rect 설정
	{
		case 0:
			sheet_rect.x=0;
			break;
		case 1:
			sheet_rect.x=30;
			break;
		case 2:
			sheet_rect.x=60;
			break;
		case 3:
			sheet_rect.x=90;
			break;
		case 4:
			sheet_rect.x=120;
			break;
		case 5:
			sheet_rect.x=150;
			break;
	}
}

//시트의 x 위치 바꿈
void CBlock::SetSheet(int sheet)
{
	sheet_rect.x=sheet;
}

void CBlock::update()
{
	if(block_break)		//블럭이 파괴된 상태.
	{
		//애니메이션 업데이트
		animation_sheet.x=frame*30;
		animation_sheet.y=0;
		animation_sheet.w=30;
		animation_sheet.h=30;
		if(frame_delay>=4)
		{
			frame++;
			frame_delay=0;
		}
		frame_delay++;
		if(frame==4)
		{
			frame=0;
			state=false;
		}
	}
}
