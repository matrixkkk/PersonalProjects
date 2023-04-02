/*
	Ŭ���� �� : CCharacter Ŭ����
	Ŭ���� �뵵:ĳ������ �⺻���� �̵�,��ȯ�� ó���ϴ� Ŭ���� �Դϴ�.
				ĳ���Ϳ��� �� ������ �ִµ� �ϳ��� Ű���� �Է°� �浹�� ó������ �ʴ� 
				�ٸ� �÷��̾� ĳ�����̰� �ϳ��� ���� ĳ���ͷ� �Է°� �浹�� ó�� �մϴ�.

*/
#ifndef	CCHARACTER_H_
#define CCHARACTER_H_

#include "CBiped.h"
#include "CMuzelSplash.h"

class CCharacter : public CBiped
{
public:
	CCharacter();
	virtual ~CCharacter();

	bool	Init(NiActorManager* pkActor,const NiTransform& kSpawn);
	void	Reset();
	
	virtual void Update(float fTime);
	virtual void Draw(NiCamera* pkCamera);		//�׸��� �׸���
	virtual void ProcessFireResult( FireResult* pResult);
	
	NiNode*		GetABVRoot();
	NiNode*		GetSpineBone();
	
	DWORD		GetStatus(){ return m_dwStatus;}		//ĳ������ ���� ��������	

	void		UpdateMuzel( float fAccumTime );
	void		DrawMuzel();
	void		SetStatus(unsigned int uiStatus);
	void		DeactiveState(unsigned int uiStatus);

protected:

	CMuzelSplash* m_pkMuzle;		//�ѱ� �Ҳ� Ŭ����

	virtual void InterpretInput(float fTime);			// �ִϸ��̼� ó��
	virtual void UpdateBipedRootTransform(float fTime);	// ��Ʈ ĳ���� ��ȯ
		
	float		m_fSpeed;				//ĳ������ ���ǵ�
	float		m_fHeadHorizon;			//ĳ���� �Ӹ��� ����ȸ��
	float		m_fHeadVertical;		//ĳ���� �Ӹ��� ���� ȸ��
	
	float		m_fLastUpdate;			//���������� ������Ʈ�� �ð�
	NiPoint2	m_kMoveDir;				//ĳ���� �̵� ���� 
	NiPoint3	m_Gravity;				//�߷�

	NiMatrix3	m_kBaseSpineRotate;	

	//spawn ��ġ
	NiPoint3	m_kSpawnTranslate;		//ĳ������ ���� �̵���ȯ
	NiMatrix3	m_kSpawnRotate;			//ĳ������ ���� ȸ��

	NiNodePtr	m_spABVRoot;			//�浹�ڽ� ��Ʈ
	NiNodePtr	m_spSpineBone;			//���� ȸ���� ���� ���� ���� ���

	DWORD		m_dwStatus;			//ĳ������ ����	

};

#endif