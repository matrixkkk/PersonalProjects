#include "OtherPlayer.h"


//다른 플레이어 블록 중 부셔진 블록 찾는 함수객체

class BrokenOtherBlockCheck
{
private:
	int blockIndexs;
public:
	BrokenOtherBlockCheck(int num)
	{
		blockIndexs = num;
	}
	bool operator()(CBlock &blocks)
	{
		
		if(blockIndexs == blocks.GetBlockNum())
		{
			return true;
		}		
		return false;
	}
};


void han2unicode(char *src, Uint16 *dest);


void COtherPlayer::init(SDL_Surface ** images,SDL_Surface *screen,int players,deque<CBlock> blocks,Argument *arg)
{
	SRECT viewPort,winPort;
	
	winPort = SRECT(18,467,133,674);	//윈도우 포트 설정

	m_pScreen = screen;
	surfaceArray = images;			//이미지 배열들 대입

	//폰트 로드
	font = TTF_OpenFont("../../Game/font/HYNAMB.ttf",20);
	Uint16	transUnicode[50];
	SDL_Color color = { 255,255,255 };
	
	char **pIdArray = arg->playIDs;
	
	for(int i=0;i<arg->numOfUser;i++)
	{
		han2unicode(pIdArray[i],transUnicode);
		ttfBlended[i] = TTF_RenderUNICODE_Blended(font,transUnicode,color);
	}

	//arument값으로 다른 플레이어 설정
	currPlayerNum = arg->currPlayerNum;
	numOfOtherPlayer = arg->numOfUser;

	
	//아이디를 그릴 위치 초기화
	SetSDLRect(idRect[0],130,80,0,0);
	SetSDLRect(idRect[1],688,58,0,0);
	SetSDLRect(idRect[2],860,58,0,0);
	SetSDLRect(idRect[3],515,310,0,0);
	SetSDLRect(idRect[4],688,310,0,0);
	SetSDLRect(idRect[5],860,310,0,0);


	//위치 재설정
	for(int i=0,j=1;i<numOfOtherPlayer;i++)
	{
		if(i==currPlayerNum)
		{
			idRectArray[i]=&idRect[0];
			continue;
		}
		idRectArray[i]=&idRect[j++];
	}

	
	//플레이어 개수만큼 컨테이너 메모리 할당
	for(int i=0;i<numOfOtherPlayer-1;i++)
	{
		otherPlayer.push_back(OtherPlayerGameInform());		//플레이어 정보
	}
	//스코어 보드 컨테이너 메모리 할당
	for(int i=0;i<numOfOtherPlayer;i++)
	{
		board.push_back(ScoreBoard(i));				//스코어 보드
	}
	
	//선택된 맵 정보로 초기화
	for(unsigned int i=0;i<otherPlayer.size();i++)
	{
		
		//블럭의 위치 복사
		otherPlayer[i].otherBlocks.assign(blocks.begin(),blocks.end());		
	}	
	
	//	변환해서 그리기 할 차례입니다.(블럭에서 변환에서 저장)
	for(int i=0;i<numOfOtherPlayer-1;i++)
	{
		for(UINT j=0;j<blocks.size();j++)
		{
			//블럭을 축소해서 그리기 위해 true값 넣음
			otherPlayer[i].otherBlocks[j].SetScale(true);
			ChangePort(i,otherPlayer[i].otherBlocks[j].GetPointRect());
		}		
	}
	
}
//좌표 변환하기
void COtherPlayer::ChangePort(int idx,SDL_Rect *rect)
{
	SRECT viewPort,winPort;
	
	winPort = SRECT(18,467,133,674);	//윈도우 포트 설정

	switch(idx)
	{
		case 0:
			viewPort = SRECT(669,831,96,288);
			break;
		case 1:
			viewPort = SRECT(843,1005,96,288);
			break;
		case 2:
			viewPort = SRECT(495,657,347,539);
			break;
		case 3:
			viewPort = SRECT(669,831,347,539);
			break;
		case 4:
			viewPort = SRECT(843,1005,347,539);
			break;
	}
	win_viewTransFrom(winPort,viewPort,*rect,rect);
}

void COtherPlayer::Draw()
{
	//블록 그리기
	for(unsigned int i=0 ;i<otherPlayer.size() ;i++)
	{	
		for_each(otherPlayer[i].otherBlocks.begin(),otherPlayer[i].otherBlocks.end(),BlockDraw(surfaceArray[0],surfaceArray[1]));
	}
	//bar 그리기
	for(UINT i=0;i<otherPlayer.size();i++)
	{
		SDL_Rect tmpRect = otherPlayer[i].bar;
		ChangePort(i,&tmpRect);		//좌표 변환
		SDL_BlitSurface(surfaceArray[4],NULL,m_pScreen,&tmpRect);
	}
	//ball 그리기
	for(UINT i=0;i<otherPlayer.size() ; i++)
	{
		int ballCount = otherPlayer[i].numOfBall;
		for(int j=0;j<ballCount;j++)
		{
			//0이 아닐때만 그린다. 
			if(otherPlayer[i].ballPos[j].x!=0 && otherPlayer[i].ballPos[j].y < BOTTOMLINE)
			{
				SDL_Rect tmpRect = otherPlayer[i].ballPos[j];
				ChangePort(i,&tmpRect);	//좌표 변환
				SDL_SoftStretch(surfaceArray[3],NULL,m_pScreen,&tmpRect);
			}			
		}
	}
	//스코어 보드 그리기
	for(UINT i=0;i<board.size();i++)
	{
		board[i].Draw(m_pScreen);
	}
	//임시 폰트 드로우
	
	for(int i=0;i<numOfOtherPlayer;i++)
	{
		SDL_BlitSurface(ttfBlended[i],NULL,m_pScreen,idRectArray[i]);
	}	
	
}

//공유 데이터를 가져와서 다른 플레이어 정보 갱신
void COtherPlayer::SetPlayersInform(CGameEngine *pGame)
{
	//임계영역
	CRITICAL_SECTION cs = pGame->cs;

	CNetSystem *pNet = pGame->GetNetClass();
	
	if(pNet==NULL)
		return;

	EnterCriticalSection(&cs);
		
	//공유 리소스 가져오기
	PackedData *otherData = pNet->GetPublicData();
		
	if(otherData==NULL)
		return ;

	//플레이어 정보 저장	
	int dataStorageCount=0;		//다른 플레이어 정보를 컨테이너에 넣는 카운트
	for(UINT i=0;i<=otherPlayer.size();i++)
	{
		if(otherData->emptyPackageIndex[i])	//존재하는 플레이어 일 경우
		{
			int num = otherData->packedGameData[i].numOfPlayer;
			//현재 저장하고자 하는 데이터가 본인 데이터일 경우 무시
			if(num == currPlayerNum)
				continue;
			memcpy(otherPlayer[dataStorageCount].ballPos ,otherData->packedGameData[i].ballPos,sizeof(otherData->packedGameData[i].ballPos));
			memcpy(otherPlayer[dataStorageCount].brokenBlock,otherData->packedGameData[i].breakenBlock,sizeof(otherData->packedGameData[i].breakenBlock));
			otherPlayer[dataStorageCount].bar        = otherData->packedGameData[i].barRect;
			otherPlayer[dataStorageCount].numOfBall  = otherData->packedGameData[i].numOfBall;
			otherPlayer[dataStorageCount].reachScore = otherData->packedGameData[i].score;
			otherPlayer[dataStorageCount].playerNum  = otherData->packedGameData[i].numOfPlayer;
			otherPlayer[dataStorageCount].itemInfo   = otherData->packedGameData[i].itemInfo;
			
			//점수 저장
			int otherScore = otherData->packedGameData[i].score;
			board[num].SetReachScore(otherScore);

			dataStorageCount++;
		}
	}
	LeaveCriticalSection(&cs);
	
	
	//깨진 블록 만큼 블록 제거
	for(UINT i=0;i<otherPlayer.size();i++)
	{					
		//부셔진 돌 찾기
		deque<CBlock> *blocks = &otherPlayer[i].otherBlocks;
		deque<CBlock>::iterator itor,pre;
		
		for(int j=0;j<10;j++)
		{	
			itor = find_if(blocks->begin(),blocks->end(),BrokenOtherBlockCheck(otherPlayer[i].brokenBlock[j]));
			if(itor != blocks->end())
			{
				blocks->erase(itor);
			}
		}		
	}	

	//현재 플레이어에게 적용된 아이템 저장
	for(UINT i=0;i<otherPlayer.size();i++)
	{
		//공격 대상이 현재 플레이어 라면
		if(otherPlayer[i].itemInfo.targetPlayer == currPlayerNum)
		{			
			//임시 출력
			printf("%d -> 당신에게 공격\n",otherPlayer[i].playerNum);
			targetedItem = otherPlayer[i].itemInfo;
		}
	}

	//다른 플레이어 점수 업데이트
	for_each(board.begin(),board.end(),mem_fun_ref(&ScoreBoard::ScoreUpdate));
}


void COtherPlayer::CleanUp()
{	
	otherPlayer.clear();
	//스코어 보드 초기화
	for_each(board.begin(),board.end(),mem_fun_ref(&ScoreBoard::CleanUp));
	
	//폰트 초기화
	TTF_CloseFont(font);
	for(int i=0;i<6;i++)
	{
		if(ttfBlended[i])
			SDL_FreeSurface(ttfBlended[i]);
	}
}

//안부셔지는 라인 추가
void COtherPlayer::NonBrokenBlockCreate()
{
	for(UINT i=0;i<otherPlayer.size();i++)
	{
		int idx = otherPlayer[i].otherBlocks.front().GetBlockNum()+1;		//마지막 블록의 넘버에 +1이 새로 추가될 블록의 인덱스
		float pos_x = LEFTLINE,pos_y=133;

		//기본 블록의 y값의 위치를 수정
		for_each(otherPlayer[i].otherBlocks.begin(),otherPlayer[i].otherBlocks.end(),mem_fun_ref(&CBlock::BlockDown));
		//15개 생성
		for(int j=0;j<15;j++)
		{
			otherPlayer[i].otherBlocks.push_front(CBlock(CPoint2(pos_x,pos_y),idx++));
			otherPlayer[i].otherBlocks.front().SetKind(6);	//안 부셔지는 벽돌로
			otherPlayer[i].otherBlocks.front().SetSheet(180);
			
			otherPlayer[i].otherBlocks.front().SetScale(true);
			ChangePort(i,otherPlayer[i].otherBlocks.front().GetPointRect());
			pos_x+=30;
		}			
	}
}