#ifndef CACCEPTTHREAD_H_
#define CACCEPTTHREAD_H_

#include "CNetSystemServer.h"


class CAcceptThread : public NiThreadProcedure
{
private:

	CNetSystemServer *m_pNetSystem;
	NiThread*	m_pkThread;		//스레드 객체

	bool	m_bRunning;
public:
	CAcceptThread();
	~CAcceptThread();

	bool	 Create(CNetSystemServer *pNetSystem);	//생성
	int		 Suspend();	//스레드 정지
	int		 Resume();  //재시작
	void	 EndThread();
	

	unsigned int ThreadProcedure(void* pvArg);
};


#endif
