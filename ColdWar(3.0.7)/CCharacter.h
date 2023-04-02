/*
	클래스 명 : CCharacter 클래스
	클래스 용도:캐릭터의 기본적인 이동,변환을 처리하는 클래스 입니다.
				캐릭터에는 두 종류가 있는데 하나는 키보드 입력과 충돌을 처리하지 않는 
				다른 플레이어 캐릭터이고 하나는 유저 캐릭터로 입력과 충돌을 처리 합니다.

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
	virtual void Draw(NiCamera* pkCamera);		//그림자 그리기
	virtual void ProcessFireResult( FireResult* pResult);
	
	NiNode*		GetABVRoot();
	NiNode*		GetSpineBone();
	
	DWORD		GetStatus(){ return m_dwStatus;}		//캐릭터의 상태 가져오기	

	void		UpdateMuzel( float fAccumTime );
	void		DrawMuzel();
	void		SetStatus(unsigned int uiStatus);
	void		DeactiveState(unsigned int uiStatus);

protected:

	CMuzelSplash* m_pkMuzle;		//총구 불꽃 클래스

	virtual void InterpretInput(float fTime);			// 애니메이션 처리
	virtual void UpdateBipedRootTransform(float fTime);	// 루트 캐릭터 변환
		
	float		m_fSpeed;				//캐릭터의 스피드
	float		m_fHeadHorizon;			//캐릭터 머리의 수평회전
	float		m_fHeadVertical;		//캐릭터 머리의 수직 회전
	
	float		m_fLastUpdate;			//마지막으로 업데이트한 시간
	NiPoint2	m_kMoveDir;				//캐릭터 이동 방향 
	NiPoint3	m_Gravity;				//중력

	NiMatrix3	m_kBaseSpineRotate;	

	//spawn 위치
	NiPoint3	m_kSpawnTranslate;		//캐릭터의 로컬 이동변환
	NiMatrix3	m_kSpawnRotate;			//캐릭터의 로컬 회전

	NiNodePtr	m_spABVRoot;			//충돌박스 루트
	NiNodePtr	m_spSpineBone;			//몸통 회전을 위해 본에 심을 노드

	DWORD		m_dwStatus;			//캐릭터의 상태	

};

#endif