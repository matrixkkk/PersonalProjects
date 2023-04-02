#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

#include "common.h"

/* ���ھ� ����
 �����ǿ� ���� ��� ó���� �ϴ� Ŭ����

 */

class ScoreBoard
{
	int			playNum;		//�÷��̾� �ѹ�
	int			score;			//������ ����
	int			reachScore;		//���� ����
	int			rank;			//���

	//char		divideScore[4];	//������ �ڸ������� �и�

	SDL_Surface *scoreSheet;		//���� ��Ʈ
	SDL_Surface *playerNumSheet;	//�÷��̾� ��ȣ ��Ʈ
 
	SDL_Rect	scorePos;			//������ �׸� ��ġ
	SDL_Rect	ScoresheetPos;		//��Ʈ���� ��ġ

	SDL_Rect	playerNumPos;		//�÷��̾� ��ȣ�� �׸� ��ġ
	SDL_Rect	playerNumSheetPos;	//��Ʈ���� �÷��̾� ��ȣ ��ġ
	
public:
	ScoreBoard(int PlayerNum);			//�÷��̾� �ѹ��� �Ѱ��ְ� �ʱ�ȭ
		
	void Draw(SDL_Surface* pScreen);	//���ھ� ���� �׸���
	void ScoreUpdate();					//���� ������Ʈ
//	void PositionResetByRank();			//��޿� ���� ��ġ ������
	void SetReachScore(int ReachScore);					//���� ���ϰų� ����
	void CleanUp();

};


#endif
