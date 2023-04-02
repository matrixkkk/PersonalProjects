/*
	클래스 명	: CClientThread
	클래스 용도 : CGameClient 객체네에서 사용되는 스레드를 생성하기 위한 클래스로
				  서버와의 통신을 위한 스레드로 사용된다.

*/

#ifndef CCLIENTTHREAD_H_
#define CCLIENTTHREAD_H_

#include "NetCommon.h"

#define CLIENT_BUFSIZE 256

class CGameClient;

class CClientThread : public NiThreadProcedure
{
private:

	CGameClient *m_pNetSystem;
	NiThread*	m_pkThread;		//스레드 객체

	bool	m_bRunning;
public:
	CClientThread();
	~CClientThread();

	bool	 Create(CGameClient *pNetSystem);	//생성
	int		 Suspend();	//스레드 정지
	int		 Resume();  //재시작
	void	 EndThread();
	

	unsigned int ThreadProcedure(void* pvArg);
};


#endif