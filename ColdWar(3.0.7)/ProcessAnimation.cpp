#include "ProcessAnimation.h"

void ProcessAnimation::Init(NiActorManager *pkActor)
{
	m_pkActorManager = pkActor;
	eventCode = -1;
	m_type = INTERRUPT;
	m_bBlend = false;
}

void ProcessAnimation::SetAnimation(NiActorManager::EventCode eCode, ProcessAnimation::TYPE type, float fTime,bool bBlend)
{	
	//현재 진행중인 애니메이션과 새로 하게될 애니메이션이 같을 경우
	//애니메이션을 그대로 진행한다.
	if(eventCode == -1)
	{
		//현재 애니메이션이 없다면
		eventCode = eCode;
		m_type = type;
		m_pkActorManager->SetTargetAnimation(eCode);
		return;
	}

	if(eventCode == eCode)
	{
		float fEndTime = m_pkActorManager->GetNextEventTime(NiActorManager::TEXT_KEY_EVENT,
			m_pkActorManager->GetTargetAnimation(),"end");

		if(fTime > fEndTime)
		{
			m_type = type;
			eventCode = eCode;
		}
		return;
	}

	if(m_type == NONINTERRUPT)
	{
		float fEndTime = m_pkActorManager->GetNextEventTime(NiActorManager::TEXT_KEY_EVENT,
			m_pkActorManager->GetTargetAnimation(),"end");
				
		if(fTime > fEndTime)
		{
			eventCode = eCode;
			m_type = type;					
		}
	}
	else if(m_type == INTERRUPT)
	{
 		eventCode = eCode;
		m_type = type;		
	}		
}

void ProcessAnimation::UpdateAnimation()
{	
	m_pkActorManager->SetTargetAnimation(eventCode);
}

NiActorManager::EventCode ProcessAnimation::GetAnimation()
{
	return eventCode;
}
