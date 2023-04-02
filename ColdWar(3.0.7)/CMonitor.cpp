#include "stdafx.h"
#include "CMonitor.h"


CMonitor::CMonitor()
{
	InitializeCriticalSection( &m_csSyncObject );
}

CMonitor::~CMonitor()
{
	DeleteCriticalSection( &m_csSyncObject );
}

#if(_WIN32_WINNT >= 0x0400 )
BOOL CMonitor::TryEnter()
{
	return TrhEnterCriticalSection(&m_csSyncObject);
}
#endif

void CMonitor::Enter()
{
	::EnterCriticalSection(&m_csSyncObject);
}

void CMonitor::Leave()
{
	::LeaveCriticalSection(&m_csSyncObject);
}

CMonitor::Owner::Owner(CMonitor &crit)
: m_csSyncObject(crit)
{
	m_csSyncObject.Enter();
}

CMonitor::Owner::~Owner()
{
	m_csSyncObject.Leave();
}