/*
	Ŭ���� �̸� : ProcessAnimation
	Ŭ���� �뵵 : �Ѱ��� �ִϸ��̼��� ������ ���ҿ� ���� ������ ����Ǵ� �ִϸ��̼���
				  ��ü���� �ƴϸ� �׳� �������� �带�� �ƴϸ� ������� �����ش�.
*/

#ifndef PROCESSANIMATION_H_
#define PROCESSANIMATION_H_

#include "main.h"

class ProcessAnimation
{
public:
	enum TYPE
	{
		NONINTERRUPT =0,	//���� ���� ����
		INTERRUPT	 =1,	//���� ����
		BLEND		 =2		//���� 

	};
	void Init(NiActorManager* pkActor);
	void SetAnimation(NiActorManager::EventCode eCode,TYPE type,float fTime,bool bBlend = false);
	void UpdateAnimation();
	NiActorManager::EventCode GetAnimation();

private:
	NiActorManager*				m_pkActorManager;
	NiActorManager::EventCode	eventCode;
	ProcessAnimation::TYPE      m_type;

	bool	m_bBlend;

	
};

#endif
