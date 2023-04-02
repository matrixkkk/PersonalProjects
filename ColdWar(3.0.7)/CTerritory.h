#ifndef CTERRITORY_H
#define CTERRITORY_H

#include "main.h"
#include "CUserCharacter.h"
#include <map>
#include <vector>


class CTerritory : public NiMemObject
{
	enum{ USA_TERR, EU_TERR, NEUTRAL };
	
	static CTerritory		*m_pTerritory;		
	NiNodePtr				m_spRoot;
	CUserCharacter			*m_pCharacter;

	NiTPointerMap< BYTE, NiNodePtr >	m_mapTerritorys;
	NiTPointerMap< BYTE, NiNodePtr >	m_mapCollision;
	std::map< BYTE, NiPoint3 >			m_mapTerritoryPos;

	BYTE	m_nStateTerritory[nsPlayerInfo::TERRITORY_MAX];	// 점령지 점령되어진 상태	

	NiScreenElementsPtr	m_spTerritoryStateBar;
	NiScreenElementsPtr m_pKTerritoryState[3];

	NiTexturingPropertyPtr m_spUSA;
	NiTexturingPropertyPtr m_spEU;

	float m_fStartX, m_fStartY;
	float m_fXSpacing;
	float m_fBarWidth, m_fBarHeight;

public:

	CTerritory();
	~CTerritory();

	
	static void CreateTerritory();
	static CTerritory* GetInstance();
	static void Destroy();	

	bool Initialize();
	virtual void Update( float fElapsedTime );
	virtual void Render( NiCamera* pkCamera ); 

	void UpdateTerritory( BYTE nCamp, BYTE nTerritory );
	void AddTerritory( int nCamp, char *pName );	
	NiNode* ClonTerritory( BYTE nCamp );
	NiNode* GetObject();

	void RobeTerritory( BYTE nPoseCamp0, BYTE nRobeTerritory );
	void PossessionTerritory( BYTE nCamp, BYTE nTerritory );

	void RenderScreenItem();

};

#endif