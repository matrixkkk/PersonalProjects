/*
	클래스 이름 : ProcessAnimation
	클래스 용도 : 넘겨진 애니메이션의 종류와 역할에 따라 기존에 수행되던 애니메이션이
				  대체될지 아니면 그냥 기존으로 흐를지 아니면 블렌드될지 정해준다.
*/

#ifndef PROCESSANIMATION_H_
#define PROCESSANIMATION_H_

#include "main.h"

class ProcessAnimation
{
public:
	enum TYPE
	{
		NONINTERRUPT =0,	//방해 받지 않음
		INTERRUPT	 =1,	//방해 받음
		BLEND		 =2		//섞임 

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
