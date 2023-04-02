#pragma once

#include <winsock2.h>


class CMonitor : public NiMemObject
{

public:
	
	class Owner
	{
	public:

		Owner( CMonitor &crit );
		~Owner();

	private:

		CMonitor &m_csSyncObject;

		Owner( const Owner &rhs );
		Owner &operator=(const Owner &rhs );
	};

	CMonitor();
	~CMonitor();

#if(_WIN32_WINNT >= 0x0400 )
	BOOL TryEnter();
#endif

	void Enter();
	void Leave();

private:

	CRITICAL_SECTION m_csSyncObject;

	CMonitor( const CMonitor &rhs );
	CMonitor &operator=( const CMonitor &rhs );
};


