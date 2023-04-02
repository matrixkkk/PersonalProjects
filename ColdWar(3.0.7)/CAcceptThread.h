#ifndef CACCEPTTHREAD_H_
#define CACCEPTTHREAD_H_

#include "CNetSystemServer.h"


class CAcceptThread : public NiThreadProcedure
{
private:

	CNetSystemServer *m_pNetSystem;
	NiThread*	m_pkThread;		//������ ��ü

	bool	m_bRunning;
public:
	CAcceptThread();
	~CAcceptThread();

	bool	 Create(CNetSystemServer *pNetSystem);	//����
	int		 Suspend();	//������ ����
	int		 Resume();  //�����
	void	 EndThread();
	

	unsigned int ThreadProcedure(void* pvArg);
};


#endif
