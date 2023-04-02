#include "OtherPlayer.h"


//�ٸ� �÷��̾� ��� �� �μ��� ��� ã�� �Լ���ü

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
	
	winPort = SRECT(18,467,133,674);	//������ ��Ʈ ����

	m_pScreen = screen;
	surfaceArray = images;			//�̹��� �迭�� ����

	//��Ʈ �ε�
	font = TTF_OpenFont("../../Game/font/HYNAMB.ttf",20);
	Uint16	transUnicode[50];
	SDL_Color color = { 255,255,255 };
	
	char **pIdArray = arg->playIDs;
	
	for(int i=0;i<arg->numOfUser;i++)
	{
		han2unicode(pIdArray[i],transUnicode);
		ttfBlended[i] = TTF_RenderUNICODE_Blended(font,transUnicode,color);
	}

	//arument������ �ٸ� �÷��̾� ����
	currPlayerNum = arg->currPlayerNum;
	numOfOtherPlayer = arg->numOfUser;

	
	//���̵� �׸� ��ġ �ʱ�ȭ
	SetSDLRect(idRect[0],130,80,0,0);
	SetSDLRect(idRect[1],688,58,0,0);
	SetSDLRect(idRect[2],860,58,0,0);
	SetSDLRect(idRect[3],515,310,0,0);
	SetSDLRect(idRect[4],688,310,0,0);
	SetSDLRect(idRect[5],860,310,0,0);


	//��ġ �缳��
	for(int i=0,j=1;i<numOfOtherPlayer;i++)
	{
		if(i==currPlayerNum)
		{
			idRectArray[i]=&idRect[0];
			continue;
		}
		idRectArray[i]=&idRect[j++];
	}

	
	//�÷��̾� ������ŭ �����̳� �޸� �Ҵ�
	for(int i=0;i<numOfOtherPlayer-1;i++)
	{
		otherPlayer.push_back(OtherPlayerGameInform());		//�÷��̾� ����
	}
	//���ھ� ���� �����̳� �޸� �Ҵ�
	for(int i=0;i<numOfOtherPlayer;i++)
	{
		board.push_back(ScoreBoard(i));				//���ھ� ����
	}
	
	//���õ� �� ������ �ʱ�ȭ
	for(unsigned int i=0;i<otherPlayer.size();i++)
	{
		
		//���� ��ġ ����
		otherPlayer[i].otherBlocks.assign(blocks.begin(),blocks.end());		
	}	
	
	//	��ȯ�ؼ� �׸��� �� �����Դϴ�.(������ ��ȯ���� ����)
	for(int i=0;i<numOfOtherPlayer-1;i++)
	{
		for(UINT j=0;j<blocks.size();j++)
		{
			//���� ����ؼ� �׸��� ���� true�� ����
			otherPlayer[i].otherBlocks[j].SetScale(true);
			ChangePort(i,otherPlayer[i].otherBlocks[j].GetPointRect());
		}		
	}
	
}
//��ǥ ��ȯ�ϱ�
void COtherPlayer::ChangePort(int idx,SDL_Rect *rect)
{
	SRECT viewPort,winPort;
	
	winPort = SRECT(18,467,133,674);	//������ ��Ʈ ����

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
	//��� �׸���
	for(unsigned int i=0 ;i<otherPlayer.size() ;i++)
	{	
		for_each(otherPlayer[i].otherBlocks.begin(),otherPlayer[i].otherBlocks.end(),BlockDraw(surfaceArray[0],surfaceArray[1]));
	}
	//bar �׸���
	for(UINT i=0;i<otherPlayer.size();i++)
	{
		SDL_Rect tmpRect = otherPlayer[i].bar;
		ChangePort(i,&tmpRect);		//��ǥ ��ȯ
		SDL_BlitSurface(surfaceArray[4],NULL,m_pScreen,&tmpRect);
	}
	//ball �׸���
	for(UINT i=0;i<otherPlayer.size() ; i++)
	{
		int ballCount = otherPlayer[i].numOfBall;
		for(int j=0;j<ballCount;j++)
		{
			//0�� �ƴҶ��� �׸���. 
			if(otherPlayer[i].ballPos[j].x!=0 && otherPlayer[i].ballPos[j].y < BOTTOMLINE)
			{
				SDL_Rect tmpRect = otherPlayer[i].ballPos[j];
				ChangePort(i,&tmpRect);	//��ǥ ��ȯ
				SDL_SoftStretch(surfaceArray[3],NULL,m_pScreen,&tmpRect);
			}			
		}
	}
	//���ھ� ���� �׸���
	for(UINT i=0;i<board.size();i++)
	{
		board[i].Draw(m_pScreen);
	}
	//�ӽ� ��Ʈ ��ο�
	
	for(int i=0;i<numOfOtherPlayer;i++)
	{
		SDL_BlitSurface(ttfBlended[i],NULL,m_pScreen,idRectArray[i]);
	}	
	
}

//���� �����͸� �����ͼ� �ٸ� �÷��̾� ���� ����
void COtherPlayer::SetPlayersInform(CGameEngine *pGame)
{
	//�Ӱ迵��
	CRITICAL_SECTION cs = pGame->cs;

	CNetSystem *pNet = pGame->GetNetClass();
	
	if(pNet==NULL)
		return;

	EnterCriticalSection(&cs);
		
	//���� ���ҽ� ��������
	PackedData *otherData = pNet->GetPublicData();
		
	if(otherData==NULL)
		return ;

	//�÷��̾� ���� ����	
	int dataStorageCount=0;		//�ٸ� �÷��̾� ������ �����̳ʿ� �ִ� ī��Ʈ
	for(UINT i=0;i<=otherPlayer.size();i++)
	{
		if(otherData->emptyPackageIndex[i])	//�����ϴ� �÷��̾� �� ���
		{
			int num = otherData->packedGameData[i].numOfPlayer;
			//���� �����ϰ��� �ϴ� �����Ͱ� ���� �������� ��� ����
			if(num == currPlayerNum)
				continue;
			memcpy(otherPlayer[dataStorageCount].ballPos ,otherData->packedGameData[i].ballPos,sizeof(otherData->packedGameData[i].ballPos));
			memcpy(otherPlayer[dataStorageCount].brokenBlock,otherData->packedGameData[i].breakenBlock,sizeof(otherData->packedGameData[i].breakenBlock));
			otherPlayer[dataStorageCount].bar        = otherData->packedGameData[i].barRect;
			otherPlayer[dataStorageCount].numOfBall  = otherData->packedGameData[i].numOfBall;
			otherPlayer[dataStorageCount].reachScore = otherData->packedGameData[i].score;
			otherPlayer[dataStorageCount].playerNum  = otherData->packedGameData[i].numOfPlayer;
			otherPlayer[dataStorageCount].itemInfo   = otherData->packedGameData[i].itemInfo;
			
			//���� ����
			int otherScore = otherData->packedGameData[i].score;
			board[num].SetReachScore(otherScore);

			dataStorageCount++;
		}
	}
	LeaveCriticalSection(&cs);
	
	
	//���� ��� ��ŭ ��� ����
	for(UINT i=0;i<otherPlayer.size();i++)
	{					
		//�μ��� �� ã��
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

	//���� �÷��̾�� ����� ������ ����
	for(UINT i=0;i<otherPlayer.size();i++)
	{
		//���� ����� ���� �÷��̾� ���
		if(otherPlayer[i].itemInfo.targetPlayer == currPlayerNum)
		{			
			//�ӽ� ���
			printf("%d -> ��ſ��� ����\n",otherPlayer[i].playerNum);
			targetedItem = otherPlayer[i].itemInfo;
		}
	}

	//�ٸ� �÷��̾� ���� ������Ʈ
	for_each(board.begin(),board.end(),mem_fun_ref(&ScoreBoard::ScoreUpdate));
}


void COtherPlayer::CleanUp()
{	
	otherPlayer.clear();
	//���ھ� ���� �ʱ�ȭ
	for_each(board.begin(),board.end(),mem_fun_ref(&ScoreBoard::CleanUp));
	
	//��Ʈ �ʱ�ȭ
	TTF_CloseFont(font);
	for(int i=0;i<6;i++)
	{
		if(ttfBlended[i])
			SDL_FreeSurface(ttfBlended[i]);
	}
}

//�Ⱥμ����� ���� �߰�
void COtherPlayer::NonBrokenBlockCreate()
{
	for(UINT i=0;i<otherPlayer.size();i++)
	{
		int idx = otherPlayer[i].otherBlocks.front().GetBlockNum()+1;		//������ ����� �ѹ��� +1�� ���� �߰��� ����� �ε���
		float pos_x = LEFTLINE,pos_y=133;

		//�⺻ ����� y���� ��ġ�� ����
		for_each(otherPlayer[i].otherBlocks.begin(),otherPlayer[i].otherBlocks.end(),mem_fun_ref(&CBlock::BlockDown));
		//15�� ����
		for(int j=0;j<15;j++)
		{
			otherPlayer[i].otherBlocks.push_front(CBlock(CPoint2(pos_x,pos_y),idx++));
			otherPlayer[i].otherBlocks.front().SetKind(6);	//�� �μ����� ������
			otherPlayer[i].otherBlocks.front().SetSheet(180);
			
			otherPlayer[i].otherBlocks.front().SetScale(true);
			ChangePort(i,otherPlayer[i].otherBlocks.front().GetPointRect());
			pos_x+=30;
		}			
	}
}