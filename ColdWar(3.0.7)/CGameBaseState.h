#ifndef CGAMEBASESTATE_H
#define CGAMEBASESTATE_H

#include "CBaseState.h"

class CGameBaseState : public CBaseState
{

public:

	CGameBaseState();

	virtual void OnResume( const char *szState ) ;
	virtual void OnOverride( const char *szState );
	virtual void OnUpdate( float fElapsedTime );
	virtual void OnEnter(const char* szPrevious);

	inline bool IsSuspended() const { return m_bSuspended; }

	virtual void OnProcessInput(){};
	virtual void OnRender(){};

protected:

	bool	m_bSuspended;
};

NiSmartPointer(CBaseState); 


#endif