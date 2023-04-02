/*
	Ŭ���� �� : CWorkerThread 
	Ŭ���� �뵵 : ����� �۾��� ��û�� ���Ͽ� ���� ����� ó���� ���� ������
	�ۼ� : ������
*/


#ifndef CWORKERTHREAD_H_
#define CWORKERTHREAD_H_

#include "CNetSystemServer.h"


class CWorkerThread : public NiThreadProcedure
{
private:
	CNetSystemServer *m_pNetSystem;
	NiThread*	m_pkThread;		//������ ��ü

	bool	m_bRunnging;
public:	
	CWorkerThread();
	~CWorkerThread();

	bool	 Create(CNetSystemServer *pNetSystem);	//����
	int		 Suspend();	//������ ����
	int		 Resume();  //�����
	void	 EndThread();

	//ó�� ���ν���
	unsigned int ThreadProcedure(void* pvArg);
};

#endif