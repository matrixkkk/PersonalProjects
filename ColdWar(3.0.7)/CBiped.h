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

	bool Init();		//초기화

	virtual void Update(float fTime);
	virtual void Reset();

	virtual void Draw(NiCamera* pkCamera) = 0;

	void SetTargetAnimation(NiActorManager::EventCode eCode);	//타겟 애니메이션 설정

	const bool IsIdling() const;
	void SetIdling(bool bIdle);

	NiNode*			GetNIFRoot() const;			//NIF 넘겨준다.
	NiNode*			GetCharRoot() const;		//캐릭터 루트 노드 리턴
	NiActorManager* GetActorManager();	//actor매니저 리턴
	NiPoint3		GetCharacterPos();

protected:	

	virtual void InterpretInput(float fTime) = 0;			// 애니메이션 처리
	virtual void UpdateBipedRootTransform(float fTime) = 0;	// 루트 캐릭터 변환
	
	NiActorManager*		m_spActorManager;					//액터 매니져
	NiNodePtr			m_spCharRoot;						//캐릭터 루트 노드
	NiAVObjectPtr		m_spSkinObject;

	float				m_fHeightOffset;					//높이 오프셋
	float				m_fHeight;							//캐릭터 높이

	bool				m_bIsIdling;
	
	NiActorManager::EventCode m_eTargetCode;				//타겟 애니메이션 코드
	
};

#endif
