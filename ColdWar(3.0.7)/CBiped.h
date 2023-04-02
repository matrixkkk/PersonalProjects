#ifndef CBIPED_H_
#define CBIPED_H_

#include <NiAnimation.h>
#include "main.h"
#include "CameraManager.h"
#include "Animation_Enum.h"


class CBiped : public NiRefObject
{
public:
	CBiped();
	virtual ~CBiped() = 0;

	bool Init();		//�ʱ�ȭ

	virtual void Update(float fTime);
	virtual void Reset();

	virtual void Draw(NiCamera* pkCamera) = 0;

	void SetTargetAnimation(NiActorManager::EventCode eCode);	//Ÿ�� �ִϸ��̼� ����

	const bool IsIdling() const;
	void SetIdling(bool bIdle);

	NiNode*			GetNIFRoot() const;			//NIF �Ѱ��ش�.
	NiNode*			GetCharRoot() const;		//ĳ���� ��Ʈ ��� ����
	NiActorManager* GetActorManager();	//actor�Ŵ��� ����
	NiPoint3		GetCharacterPos();

protected:	

	virtual void InterpretInput(float fTime) = 0;			// �ִϸ��̼� ó��
	virtual void UpdateBipedRootTransform(float fTime) = 0;	// ��Ʈ ĳ���� ��ȯ
	
	NiActorManager*		m_spActorManager;					//���� �Ŵ���
	NiNodePtr			m_spCharRoot;						//ĳ���� ��Ʈ ���
	NiAVObjectPtr		m_spSkinObject;

	float				m_fHeightOffset;					//���� ������
	float				m_fHeight;							//ĳ���� ����

	bool				m_bIsIdling;
	
	NiActorManager::EventCode m_eTargetCode;				//Ÿ�� �ִϸ��̼� �ڵ�
	
};

#endif
