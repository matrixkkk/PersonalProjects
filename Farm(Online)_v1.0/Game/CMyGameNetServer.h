#ifndef CMYGAMENETSERVER_H_
#define CMYGAMENETSERVER_H_

#include "NetSystem.h"
#include "common.h"
#include "gameengine.h"

//���� Ŭ����
class CMyGameNetServer : public CNetSystem
{
public:
	//���� �߻� �޼ҵ�
	bool Initstance(SOCKADDR_IN servAddr,int numOfClient);	//�ʱ�ȭ.
	void Running(CGameEngine *pGame);					//��Ʈ��ũ ����
	void CleanUp();					//
	bool SendMessage();				//�޼��� ������
	bool RecvMessage();
	void publicDataProcess(MYDATA *mData); //���� ������ ó��

	PackedData* GetPublicData();	//���� ������ ��ȯ

	//�������� ���� �Լ�
	void Packing();			//������ packing

	void err_display(char *msg);

	CRITICAL_SECTION cs;		//�Ӱ迵��

private:
	vector<MYDATA>	gameData;		//Ŭ���̾�Ʈ���� ���� �����͸� ������ �����Ӵϵ�
	MYDATA			storageData;	//�ӽ÷� ������ ������
	PackedData		sendData;		//Ŭ���̾�Ʈ���� ���� ���� ������

	ClientAddr		sClientAddr;			//Ver.MadCow Ŭ���̾�Ʈ���� �ּҷ�
	UINT			numOfClient;	//Ŭ���̾�Ʈ���� ��
	//SOCKADDR_IN		clientAddr[6];	//Ŭ���̾�Ʈ���� �ּҷ�
};


#endif


