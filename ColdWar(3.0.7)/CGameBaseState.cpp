#include "CGameBaseState.h"

CGameBaseState::CGameBaseState() : CBaseState(), m_bSuspended(false)
{	
}

void CGameBaseState::OnResume( const char *szState )
{
	m_bSuspended = false;
}

void CGameBaseState::OnOverride( const char *szState )
{
	m_bSuspended = true;
}

void CGameBaseState::OnUpdate( float fElapsedTime )
{

}

void CGameBaseState::OnEnter(const char* szPrevious)
{

}

