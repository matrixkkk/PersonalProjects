/*
	클래스 명 : CWorkerThread 
	클래스 용도 : 입출력 작업이 요청된 소켓에 대한 입출력 처리를 위한 스레드
	작성 : 김현구
*/


#ifndef CWORKERTHREAD_H_
#define CWORKERTHREAD_H_

#include "CNetSystemServer.h"


class CWorkerThread : public NiThreadProcedure
{
private:
	CNetSystemServer *m_pNetSystem;
	NiThread*	m_pkThread;		//스레드 객체

	bool	m_bRunnging;
public:	
	CWorkerThread();
	~CWorkerThread();

	bool	 Create(CNetSystemServer *pNetSystem);	//생성
	int		 Suspend();	//스레드 정지
	int		 Resume();  //재시작
	void	 EndThread();

	//처리 프로시져
	unsigned int ThreadProcedure(void* pvArg);
};

#endif