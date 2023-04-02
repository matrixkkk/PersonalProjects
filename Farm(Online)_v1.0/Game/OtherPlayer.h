#ifndef OTHERPLAYER_H_
#define OTHERPLAYER_H_

#include "common.h"
#include "block.h"
#include "gameengine.h"
#include "scoreboard.h"


//�ٸ� �÷��̾� ���� ����ü
struct OtherPlayerGameInform
{
	SDL_Rect		bar;				//bar�� �׸� ��ġ
	SDL_Rect		ballPos[4];			//ball�� �׸� ��ġ
	
	ITEMINFO		itemInfo;			//������ ����
	
	deque<CBlock> otherBlocks;				//����

	int				brokenBlock[10];		//�ֱ� ���� ��ϵ�
	int				score;				//����
	int				reachScore;			//���� ����
	int				playerNum;			//�÷��̾� �ѹ�
	UINT			numOfBall;			//���� ��
	
	OtherPlayerGameInform()
	{
		bar.x		 = LEFTLINE+180; bar.y = BOTTOMLINE-20; bar.w = 100; bar.h = 20;
		ballPos[0].x = LEFTLINE+220; ballPos[0].y = BOTTOMLINE-35; ballPos[0].w = 20, ballPos[0].h = 20;
		score = 0;
		reachScore = 0;
		numOfBall = 0;		
		memset(brokenBlock,-1,sizeof(brokenBlock));
	}
};

//�ٸ� �÷��̾��� ���� ������ ó���� ���� Ŭ����
class COtherPlayer
{
	vector<OtherPlayerGameInform>	otherPlayer;		//�ٸ� �÷��̾� ������
	SDL_Surface						*m_pScreen;			//��ũ�� ������
	SDL_Surface						**surfaceArray;		//�̹��� ǥ�� ������ ����Ʈ
	
	int		numOfOtherPlayer;						//�ٸ� �÷��̾���� ��
	int		currPlayerNum;								//���� ������ ��ȣ
	
	ITEMINFO	targetedItem;							//���� ������ ���ݴ��� �������� ����
	
	//��Ʈ ����
	TTF_Font *font;
	SDL_Surface *ttfBlended[6];
	SDL_Rect idRect[6];				//���̵� �׸� ��ġ.
	SDL_Rect *idRectArray[6];
	
public:
	
	vector<ScoreBoard>				board;					//���ھ� ����
	
	void	init(SDL_Surface ** images,SDL_Surface *screen,int players,deque<CBlock> blocks,Argument *arg);	//Ŭ���� �ʱ�ȭ
	void	Draw				();				//�ٸ� �÷��̾� �� �׸���
	void	ChangePort			(int idx,SDL_Rect *rect);		// ��ȯ�ؼ� �׸���
	void	SetPlayersInform	(CGameEngine *pGame);	//�ٸ� �÷��̾� ������ ����
	void	CleanUp();			//�޸� �Ұ�
	void	NonBrokenBlockCreate();						

	int		GetCurrPlayerNum()		{ return currPlayerNum; }

	ITEMINFO* GetTargetItemInfo()	{ return &targetedItem; }

};


#endif
