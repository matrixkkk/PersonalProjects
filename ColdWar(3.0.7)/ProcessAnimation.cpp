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
	//���� �������� �ִϸ��̼ǰ� ���� �ϰԵ� �ִϸ��̼��� ���� ���
	//�ִϸ��̼��� �״�� �����Ѵ�.
	if(eventCode == -1)
	{
		//���� �ִϸ��̼��� ���ٸ�
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
