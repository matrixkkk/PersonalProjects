
#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <SDL/SDL.h>
#include "gamestate.h"
#include "common.h"
#include "block.h"
#include "bar.h"
#include "ball.h"
#include "item.h"
#include "OtherPlayer.h"
#include "turtle.h"
#include "CEditBox.h"


class CPlayState : public CGameState
{
	SDL_Surface		*m_sBackGround;		//���� ��� ȭ���� �ε��� ǥ��	
	SDL_Surface     *m_sBlock;	        //���� �ε��� ǥ��
	SDL_Surface		*m_sBar;	     	//bar �̹����� �ε��� ǥ��
	SDL_Surface		*m_sBall;			//ball �̹����� �ε��� ǥ��.
	SDL_Surface		*m_sBlockAnim;		//�� �ı� �ִϸ��̼��� �ε��� ǥ��
	SDL_Surface		*m_sScore;			//���ھ� ��Ʈ
	SDL_Surface		*m_sItemSheet;		//������ ��Ʈ
	SDL_Surface		*m_sLaser;			//������ ��Ʈ
	SDL_Surface		*s_Obstarcle;		//��ֹ��� �̹��� �ε��� ǥ��
	
	//�̹��� ������ ����Ʈ
	SDL_Surface		*imageBuffer[6];
	Mix_Music       *m_mBgm[2];			//���� �ε�.
	
	//ȿ������ ������ ������ ������ 
	Mix_Chunk		*m_wCow;
	Mix_Chunk		*m_wChiken;
	Mix_Chunk		*m_wLion;
	Mix_Chunk		*m_wSheep;
	Mix_Chunk		*m_wPig;
	Mix_Chunk		*m_wDog;
	Mix_Chunk		*m_wBump;
	

	///-------������ ������ ���� ����------
	SDL_Rect		m_rLaser;			//�������� �׸� rect
	SDL_Rect		m_rSheetLaser;	    // ��Ʈ ���� ������ ��ġ
	CPoint2			m_cpLaserPos;
	int				m_LaserFrame;    	//������ �ִϸ��̼� ������

    //���� ���� Bar ������ 
	CBar			*m_pCBPlayer;          //Player�� ��� 
	
	CTurtle			turtle;					//�ź���

	int				m_IDReset;		//���� ID�� ���½�Ű�� ī����	
	int				m_score;	       //���� ����
	int				m_ReachScore;
	int				m_GenerateItem;	//�����Ǵ� �������� ����.
	int				m_map;			//���� ����
	
	bool			m_bLaserOn;		//�������� ��� ���� ����
	
	bool			m_bObstarcle;                //��ֹ��� ��� �ϱ� ���� ����
	int             m_LifetimeObstarcle,m_LiftimeControlShock ;      //��ֹ� ��� �ð�, ��Ʈ�Ѽ�ũ ���� �ð�
	
	double			m_duration;				     //��� �߰� Ÿ�̸�
	int				m_addBlockLine;				 //�߰��� ��� ���� ��
	int				m_minCollisionLine;			//�ּ� �浹 ����
	
	//(��ȹ�� �߰�) �ٸ� �÷��̾��� ���� ó�� ��ü
	COtherPlayer    m_otherPlayer;
	//int				numOfPlayer;		//�÷��̾��� �� ( ���� ������ ����)
	
	MYDATA			colData;			// �������� ������ ���� ����
	CTimer			m_timer;			//Ÿ�̸� Ŭ����
	char			itemDropTable[100];		//������ �����

	Argument        *pArgs;
	//ä�� �޼��� 
	CEditBox		chatEditBox;
public:
	deque<int>		m_brokenBlockIdx;		//�μ��� ����� �ε���
	deque<CBlock>	m_deqBlock;				//���� ���� �����̳�
	deque<CBall>	m_deqBall;				//1p �� Ŭ������ ���� �����̳� , ���� ���ڴ� �����ϱ� ������ �����̳ʷ� �����.
	deque<Citem*>   m_vecAbtainItem;		//1p �������� ���� �����̳�
	vector<Citem*>  m_vecPublicItem;		//������ �������� ���� �ϴ� �����̳�
	
	CPlayState() { }
	void Init(CGameEngine* game);
	void Cleanup();

	void Pause();
	void Resume();
	
	MYDATA* Collect();			//������ ���� �Լ�

	void HandleEvents(CGameEngine* game);
	void Update(CGameEngine* game);
	void Draw(CGameEngine* game);
	void UpdateScore();
	
	//�Ϲ� �Լ���
	void SetArgument(Argument *arg);
	void NonBrokenBlockCreate();			//�μ������ʴ� �� ����.
	void CreateMap(int map);          // �� ����

	inline void CheckColision();	  //�浹 üũ
	inline void AddScore(deque<CBlock>::iterator pos,int &score);		//���� ����.
	inline void DrawItemBox();		  //������ �ڽ� �׸���
	inline void TargetItemProcess();  //���ݴ��� ������ ó��

};

#endif
