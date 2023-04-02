/*
	Ŭ���� ��	: CClientThread
	Ŭ���� �뵵 : CGameClient ��ü�׿��� ���Ǵ� �����带 �����ϱ� ���� Ŭ������
				  �������� ����� ���� ������� ���ȴ�.

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
	NiThread*	m_pkThread;		//������ ��ü

	bool	m_bRunning;
public:
	CClientThread();
	~CClientThread();

	bool	 Create(CGameClient *pNetSystem);	//����
	int		 Suspend();	//������ ����
	int		 Resume();  //�����
	void	 EndThread();
	

	unsigned int ThreadProcedure(void* pvArg);
};


#endif