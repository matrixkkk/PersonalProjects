#ifndef NETSYSTEM_H_
#define NETSYSTEM_H_

#include "common.h"


class CGameEngine;		//���� ����

//��Ʈ��ũ ���� Ŭ����
class CNetSystem
{
protected:

	WSADATA			m_wsa;		
	SOCKET			m_socket;
	SOCKADDR_IN		m_addr;			//�ּ����� ����ü

	PackedData		publicPackedData;		//������ ���� packed Data (��ȹ�� ����)
	bool			bUsePublicData;
public:
	virtual bool Initstance(SOCKADDR_IN servAddr,int numOfClient)=0;				//�ʱ�ȭ.
	virtual void Running(CGameEngine *pGame)=0;					//��Ʈ��ũ ����
	virtual void CleanUp()=0;					//
	virtual bool SendMessage()=0;				//�޼��� ������
	virtual bool RecvMessage()=0;				//�޼��� �ޱ�
	virtual void publicDataProcess(MYDATA *mData) = 0;		//���� ������ ó��(��ȹ�� ����)
	
	virtual PackedData* GetPublicData()=0;		//���� ������ ��ȯ(��ȹ���߰�)
};

#endif