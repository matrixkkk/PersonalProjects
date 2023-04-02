#ifndef CBASESTATE_H
#define CBASESTATE_H

#include <NiMain.h>

class CBaseState : public NiRefObject
{

public:

	virtual ~CBaseState(){};

	virtual void OnEnter( const char *szState ) = 0;
	virtual void OnLeave( const char *szState ) = 0;
	virtual void OnResume( const char *szState ) = 0;
	virtual void OnOverride( const char *szState ) = 0;
	virtual void OnUpdate( float fElapsedTime ) = 0;
	
	virtual void OnProcessInput(){};
	virtual void OnRender(){};
};

NiSmartPointer(CBaseState); 


#endif