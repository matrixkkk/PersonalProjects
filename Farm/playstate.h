
#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <SDL/SDL.h>
//#include <SDL/SDL_ttf.h>
#include "gamestate.h"
#include "common.h"
#include "block.h"
#include "bar.h"
#include "ball.h"
#include "item.h"

class CPlayState : public CGameState
{
	SDL_Surface		*bg;			//���� ��� ȭ���� �ε��� ǥ��	
	SDL_Surface     *s_block;		//���� �ε��� ǥ��
	SDL_Surface		*s_bar1;		//bar �̹����� �ε��� ǥ��
	SDL_Surface		*s_bar2;
	SDL_Surface		*s_ball1;		//ball �̹����� �ε��� ǥ��.
	SDL_Surface		*s_ball2;		//ball �̹����� �ε��� ǥ��.
	SDL_Surface		*s_block_ani;	//�� �ı� �ִϸ��̼��� �ε��� ǥ��
	SDL_Surface		*s_score;		//���ھ� ��Ʈ
	SDL_Surface		*item_sheet;	//������ ��Ʈ
	SDL_Surface		*s_Laser;		//������ ��Ʈ

	SDL_Surface		*s_Nbar;		//��ֹ��� �̹��� �ε��� ǥ��
	
	Mix_Music       *m_Bgm;			//���� �ε�.
	
	Mix_Chunk		*cow;
	Mix_Chunk		*dak;
	Mix_Chunk		*lion;
	Mix_Chunk		*sheep;
	Mix_Chunk		*pig;
	Mix_Chunk		*dog;

	Mix_Chunk		*ball_high;
	Mix_Chunk		*ball_law;
	Mix_Chunk		*ball_spup;
	Mix_Chunk		*ball_spdown;
	Mix_Chunk		*ball_stop;
	Mix_Chunk		*ball_hide;
	Mix_Chunk		*key_revers;
	Mix_Chunk		*ball_die;
	Mix_Chunk		*ball_saperate;
	Mix_Chunk       *key_raiser;
	Mix_Chunk       *ball_point;
	Mix_Chunk		*ball_bar;

	///-------������ ������ ���� ����------
	SDL_Rect		r_Laser1,r_Laser2;			//�������� �׸� rect
	SDL_Rect		sheet_Laser1,sheet_Laser2;	// ��Ʈ ���� ������ ��ġ
	CPoint2			laser_pos1,laser_pos2;
	int				laser_frame1,laser_frame2;	//������ �ִϸ��̼� ������

	CBar			*player1,*player2;
	CBar			*N_play;            // ��ֹ�


	int				ID_reset;		//���� ID�� ���½�Ű�� ī����	
	int				score1,score2;	//���� ����
	int				ReachScore1,ReachScore2;
	int				GenerateItem;	//�����Ǵ� �������� ����.
	int				map;			//���� ����
	int				b_pause;		//tip�� �����ֱ� ���� �Ͻ� ���� �ϴ� ����

	bool			LaserOn1,LaserOn2;		//�������� ��� ���� ����
	
	bool			Nbar_On;                //��ֹ��� ��� �ϱ� ���� ����
	int             tmp_bar, tmp_ubar;      //��ֹ� ��� �ð�, ��Ʈ�Ѽ�ũ ���� �ð�

public:
	deque<CBlock>	Block;			//���� ���� �����̳�
	deque<CBall>	Ball1;			//1p �� Ŭ������ ���� �����̳� , ���� ���ڴ� �����ϱ� ������ �����̳ʷ� �����.
	deque<CBall>	Ball2;			//2p �� Ŭ������ ���� �����̳�
	vector<Citem*>  item1;			//1p �������� ���� �����̳�
	vector<Citem*>  item2;			//2p �������� ���� �����̳�
	vector<Citem*>  G_item;			//������ �������� ���� �ϴ� �����̳�
	
	CPlayState() { }
	void Init(CGameEngine* game);
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(CGameEngine* game);
	void Update(CGameEngine* game);
	void Draw(CGameEngine* game);
	void ScoreDraw(CGameEngine* game);
	void UpdateScore();

	void MapCreate(int A_Stage);  // �� ����
	inline void CheckColision();	//�浹 üũ
	inline void AddScore(deque<CBlock>::iterator pos,int &score);		//���� ����.
	inline void DrawItemBox();		//������ �ڽ� �׸���

};

#endif
