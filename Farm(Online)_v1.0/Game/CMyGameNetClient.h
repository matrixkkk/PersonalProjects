#ifndef CMYGAMENETCLIENT_H_
#define CMYGAMENETCLIENT_H_

#include "common.h"
#include "NetSystem.h"
#include "gameengine.h"

class CMyGameNetClient : public CNetSystem
{
public:
	//�������̵� �Լ�
	bool Initstance(SOCKADDR_IN servAddr,int numOfClient);				//�ʱ�ȭ.
	void Running(CGameEngine *pGame);					//��Ʈ��ũ ����
	void CleanUp();					//
	bool SendMessage();				//�޼��� ������
	bool RecvMessage();				//�޼��� �ޱ�
	void publicDataProcess(MYDATA *mData); //���� ������ ó��
	
	PackedData* GetPublicData();	//���� ������ ��ȯ

	//Client Ŭ�������� ���� �Լ�
	
	void UnPacking();				//Packed Data ����
	void collectData();				//������ �����͸� ������ ���� ����ü�� ������ ���� �Լ�

	//���� ���
	void err_display(char *msg);

	bool serverclose;				// Ver.MadCow SO_SNDTIMEO,SO_RCVTIMEO �����Լ� �ɼ��� ����Ͽ� ������
									// �ٿ�Ǿ����� �ƴ����� Ȯ���ϴ� boolŸ�� ����

	CRITICAL_SECTION cs;			//�Ӱ迵��

private:	
	MYDATA		   sendMydata;	//������ ������ ���� Ŭ���̾�Ʈ ������
	PackedData	   recvData;	//�������� ���� ���� ������
	ClientAddr		sClientAddr;	// Ver.MadCow ���� ������ �ٿ�� �� ������ ��ü�ϱ� ���Ͽ� �� ������ �ּҸ� ������ �ִ�.

};

#endif
