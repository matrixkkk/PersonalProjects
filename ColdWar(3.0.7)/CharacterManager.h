/*
	클래스명 : CharacrerManager
	클래스 용도 : 캐릭터 매니저! 캐릭터에 대한 생성 및 관리하는 클래스 입니다

*/

#ifndef CHARACTERMANAGER_H_
#define CHARACTERMANAGER_H_

#include "CUserCharacter.h"

#include <vector>

namespace BranchOfMilitary
{
	enum
	{
		Gerilla		= 0,

		EU_Soldier	= 1,		//보병
		EU_Sniper	= 2,		//저격수
		EU_Medic	= 3,		//의무병
		EU_ASP		= 4,		//탄약병
		
		US_Soldier	= 5,
		US_Sniper	= 6,		//저격수
		US_Medic	= 7,		//의무병
		US_ASP		= 8,		//탄약병
	};
}

class CharacterManager : public NiMemObject
{
public:
	CharacterManager();
	~CharacterManager();

	bool Initialize(NiNode* pkNode);		//캐릭터 매니저 초기화	
	void Update(float fTime, bool bNetMode = false );
	void DrawSoldier(NiCamera* pkCamera);
	void AddCharacter(unsigned int uiClass,const NiTransform& kSpawn, bool bSelf = false);	 // 캐릭터 생성
	void AllResetIntersectionRecord();

	//Get함수 목록
	CCharacter*					GetCharacterIndex(int index);			//Character 객체 리턴
	CUserCharacter*				GetUserCharacter();						//유저 캐릭터 리턴
	
	std::vector<CCharacter*>&	GetColliderVector();
private:	
	CUserCharacter*				m_pkUserCharacter;		//유저 캐릭터

	CRITICAL_SECTION				m_csCharacters;
	std::vector<NiActorManager*>	m_pkVecLoadedCharacter;
	std::vector<CCharacter*>		m_pkVecCharacter;			//유저를 제외한 캐릭터를 담아두는 컨테이너
	std::vector<NiNode*>			m_pkStartPoints;			//시작점의 노드들.. 랜덤 생성을 위해 사용
};

#include "CharacterManager.inl"
#endif

