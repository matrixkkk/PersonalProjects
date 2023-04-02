/*
	Ŭ���� �� : GameSystemManager
	Ŭ���� �뵵 : ������ �帧�� ����, ���� - ���� - ��, ������ ������ ���÷����Ѵ�.

*/

#ifndef GAMESYSTEMMANAGER_H_
#define GAMESYSTEMMANAGER_H_

#include "main.h"
#include "CPrimaryWeapon.h"
#include "CUserCharacter.h"
#include "CRadar.h"

//������ �߰� ������
const float fHeadAddDamage  = 6.0f;
const float fChestAddDamage = 3.0f;


//���� �� ü��
struct Healths
{
	short	head;		//�Ӹ�
	short	chest;		//����
	short	leftArm;	//����
	short	rightArm;	//������
	short	leftLeg;	//�޹�
	short   rightLeg;	//������

	Healths()
	{
		head = chest = leftArm = rightArm = leftLeg = rightLeg = 100;
	}
	void Reset()
	{
		head = chest = leftArm = rightArm = leftLeg = rightLeg = 100;
	}
};

//���ھ��
struct SCORE
{
	BYTE	kill;		//���� ��
	BYTE	death;		//���� ��

	SCORE()
	{
		ZeroMemory(this,sizeof(*this));
	}
};

typedef std::map<DWORD,SCORE>	SCOREMAP;

//������ �ٵ� ����
struct HealthBody
{
	NiRect<unsigned char>	m_kHead;
	NiRect<unsigned char>	m_kChest;
	NiRect<unsigned char>	m_kLeftArm;
	NiRect<unsigned char>	m_kRightArm;
	NiRect<unsigned char>	m_kLeftLeg;
	NiRect<unsigned char>	m_kRightLeg;

	HealthBody()
	{
		m_kHead		= NiRect<unsigned char>(19,35,2,16);
		m_kChest	= NiRect<unsigned char>(16,39,16,54);
		m_kLeftArm	= NiRect<unsigned char>(7,16,16,54);
		m_kRightArm	= NiRect<unsigned char>(39,48,16,54);
		m_kLeftLeg	= NiRect<unsigned char>(12,27,54,92);
		m_kRightLeg	= NiRect<unsigned char>(27,44,54,92);
	}
};


class GameSystemManager : public NiMemObject
{
public:
	GameSystemManager();
	~GameSystemManager();

	static GameSystemManager* GetInstance();

	bool	Init(DWORD	dwArmy,float fTotalTime);
	bool	CreateFont();
	bool	CreateScreenTexture();
	bool	CreateRadar();
	void	DrawUI();
	void	Update(float fTime);				//������Ʈ

	//==========================================
	//======���� �÷��� �ð� ���� �Լ�==========
	void	StartGame();		//���� �ð� ����
	void	EndGame();			//����
	void	ResetPlay();		//����
	void	MeasureTime(float fTime);		//���������� ���
	void	SetGameTime(float fCurrTime);	//Ŭ�󿡼��� ���
	//==========================================

	//======= ü�� ���� �Լ� ===================
	void	RecoveryHealth();								//ü�� ��� ȸ��
	DWORD	ProcessCondition(FireInfo* pFireInfo);			//�������� �������� ĳ���� ���� ����
	void	CalculateDamageByPart(BYTE part,short sDamage); //������ ������ ���
	void	DamageByInjury(float fTime);					//�λ����� ���� Tick������
	Healths* GetHealths();									//ü�� ����

	//====== ���ھ� ���� �Լ� ==================
	void	AddKill(DWORD id);
	void	AddDeath(DWORD id);
	SCOREMAP& GetScoreBoard(){ return m_kScoreboard; }
	void	ProcessResultScore();

	//====== ������ ����Ʈ �Լ� ================
	bool	CreateDamageEffect();
	void	DisplayDamageEffect();


private:
	void	DrawFont(NiRenderer*	pkRenderer);	//��Ʈ ��ο�
	void	HealthBodyUpdate();						//�ｺ �ٵ� ������Ʈ
	void	ConditionUpdate();						//��ü ����� ������Ʈ
	void	ChangeTexture(NiPixelData* kPixel,NiRect<unsigned char> kBody,unsigned short kHealth);	//�ش� ������ �ؽ�ó ��ȭ

	//====== static this�����ͺ��� ======
	static GameSystemManager*		ms_gameSystemManager;

	//====== ��ũ�� �ؽ��� ��� =======
	NiScreenTextureArray	m_kScreenTextureArray;	//��ũ�� �ؽ��� �׷�	

	//====== ü�� ǥ�ñ� ���� ===========================
	NiSourceTexturePtr		m_spOrigin;
	NiSourceTexturePtr		m_spBodyTex;

	//====== ��Ʈ ���� ���� ==============================
	NiFontPtr				m_spFont;				//��Ʈ
	NiString2DPtr			m_spText;
	
	//��Ʈ ��ġ
	NiPoint2				m_kPosTime;
	NiPoint2				m_kPosBulletData;	//���� �߼� ��Ʈ ��ġ --�߰�

	//========= Ŭ���� ��� ==============
	CRadar*					m_pkRadar;
	NiScreenElementsArray	m_kRadarPoint;			//���̴����� ���� ��ġ�� �׸� ��������
	
	//========= ����ü ��� ==============
	//ü�� ���� ����
	HealthBody				m_kHealthBody;		//ü�� �ٵ� ����
	Healths					m_kHealth;			//ĳ������ ü�� ����

	//========= �����̳� ��� ============
	SCOREMAP				m_kScoreboard;		//ų�� ���ھ� ��
	
	
	unsigned int			m_uiTotalCondition;	//��ü ü�� ���� �ۼ�Ʈ
	unsigned int			m_uiArmor;			//�Ƹ�

	
	//========= ���� �ð� ���� ���� ===========
	float					m_fTotalTime;		//���� �÷��� �ð�
	float					m_fCurrTime;		//���� �ð�
	float					m_fPlayAccumTime;	//�����÷��̵��� ����� ���� �ð�
	float					m_fTimer;			//�ð� �޽����� ������ Ÿ�̸�
	bool					m_bIsServer;		//true:���� ������ ���� false: ���� ������ Ŭ��
	
	//========= ������ ����Ʈ ���� ============
	NiScreenElements*		m_pkDamageEffect;	//������ ����Ʈ
	bool					m_bDamageEffectOn;	
	
	float					m_fLastDamage;
};


#endif
