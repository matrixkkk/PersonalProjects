/*
	Ŭ������ : CharacrerManager
	Ŭ���� �뵵 : ĳ���� �Ŵ���! ĳ���Ϳ� ���� ���� �� �����ϴ� Ŭ���� �Դϴ�

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

		EU_Soldier	= 1,		//����
		EU_Sniper	= 2,		//���ݼ�
		EU_Medic	= 3,		//�ǹ���
		EU_ASP		= 4,		//ź�ິ
		
		US_Soldier	= 5,
		US_Sniper	= 6,		//���ݼ�
		US_Medic	= 7,		//�ǹ���
		US_ASP		= 8,		//ź�ິ
	};
}

class CharacterManager : public NiMemObject
{
public:
	CharacterManager();
	~CharacterManager();

	bool Initialize(NiNode* pkNode);		//ĳ���� �Ŵ��� �ʱ�ȭ	
	void Update(float fTime, bool bNetMode = false );
	void DrawSoldier(NiCamera* pkCamera);
	void AddCharacter(unsigned int uiClass,const NiTransform& kSpawn, bool bSelf = false);	 // ĳ���� ����
	void AllResetIntersectionRecord();

	//Get�Լ� ���
	CCharacter*					GetCharacterIndex(int index);			//Character ��ü ����
	CUserCharacter*				GetUserCharacter();						//���� ĳ���� ����
	
	std::vector<CCharacter*>&	GetColliderVector();
private:	
	CUserCharacter*				m_pkUserCharacter;		//���� ĳ����

	CRITICAL_SECTION				m_csCharacters;
	std::vector<NiActorManager*>	m_pkVecLoadedCharacter;
	std::vector<CCharacter*>		m_pkVecCharacter;			//������ ������ ĳ���͸� ��Ƶδ� �����̳�
	std::vector<NiNode*>			m_pkStartPoints;			//�������� ����.. ���� ������ ���� ���
};

#include "CharacterManager.inl"
#endif

