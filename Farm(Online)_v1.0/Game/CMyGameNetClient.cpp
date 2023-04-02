#include "CMyGameNetClient.h"
#include <stdlib.h>
#include <time.h>

//Ŭ���̾�Ʈ �ʱ�ȭ.

void gotoxy(int x, int y)  {
	COORD Cur = {x,y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);

}

bool CMyGameNetClient::Initstance(SOCKADDR_IN servAddr,int numOfClient)
{	
	//���� �ʱ�ȭ
	if(WSAStartup(MAKEWORD(2,2),&m_wsa) !=0 )
		return false;

	//���� �ʱ�ȭ
	m_socket = socket(AF_INET,SOCK_DGRAM,0);

	// Ver.MadCow SO_RCVTIMEO,SO_SNDTIMEO ���Ͽɼ� ��� �κ����ν� ���ѽð��� �� 3�ʷ� �Ѵ�.

	PackedData temp;
	if(m_socket == INVALID_SOCKET)
	{
		err_display("socket()");
		return false;
	}

	//���� �ּ� ����ü �ʱ�ȭ
	ZeroMemory(&m_addr,sizeof(m_addr));			//�ּ����� ����ü
	m_addr = servAddr;
	m_addr.sin_family = AF_INET;
	

	printf("[Client] Ŭ���̾�Ʈ �ʱ�ȭ �Ϸ�\n");

	bUsePublicData = false;
	
	return true;
}

//Ŭ���̾�Ʈ ����
void CMyGameNetClient::Running(CGameEngine *pGame)
{
	bool b_Ready = true;
	int retval;
	int addrlen=0;

	// Ver.MadCow ���񼭹��� ���� �Ͽ� �ش�.
	int nextServerNum = 1;
	// Ver.MadCow ������ �ٿ� ���¸� false�� �Ͽ� �ٿ� �Ǿ� ���� �ʴٶ�� �����Ͽ� �ش�.
	serverclose = false;

	//�Ӱ迵�� ��������
	cs = pGame->cs;

	//�ε� �Ϸ� ��ȣ ������ ����.
	retval = sendto(m_socket,(char *)&b_Ready,sizeof(b_Ready),0,(SOCKADDR*)&m_addr,sizeof(m_addr));
	if(retval ==SOCKET_ERROR)
	{
		err_display("�ε� �Ϸ� sendto()");
		exit(1);
	}
	printf("[Client] ������ ���� �ε� �Ϸ� ��ȣ ����\n");
	
	addrlen = sizeof(m_addr);
	//�ε��� �Ϸ� �Ǿ��ٴ� ������ ���� ��ȣ�� ����(�׳� �ޱ⸸ �ϴ°� �ǹ̴� ����)
	retval = recvfrom(m_socket,(char *)&sClientAddr,sizeof(sClientAddr),0,(SOCKADDR*)&m_addr,&addrlen);
	if(retval ==SOCKET_ERROR)
	{
		err_display("��ü �ε� �Ϸ� recvfrom()");
		exit(1);
	}

	printf("[Client] ��� �÷��̾��� ���� �غ� �Ϸ�\n");

	int optval = 10000;
	
	if(setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	
	//����ó�� ���μ������� ���� ����� �������� ������ ����
	while(pGame->Running())
	{
		if(!(serverclose))
		{
			bool ret;
			//������ ���� ������ ����.
			ret = SendMessage();
		
			ret = RecvMessage();		//������ ���� packed data�� ����
		
		}
		else
		{
			// Ver.MadCow �̸� ������ ���� ������ ��ȣ�� �ڽ��� ��ȣ�� ���ٸ� �����带 �����Ų��.
			if(nextServerNum == sendMydata.numOfPlayer)
			{
				pGame->serverclose = true;
				serverclose = true;
				break;
			}
			// Ver.MadCow �׷��� �ʴٸ� ����� �������� �ּҸ� �ٲپ� �ش�.
			else
			{
				Sleep(1000);
				ZeroMemory(&m_addr,sizeof(m_addr));			//	�ּ����� ����ü
				m_addr = sClientAddr.Addr[sendMydata.numOfPlayer-1];
				m_addr.sin_port = htons(9000);
				optval = NULL;
				if(setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
				{
					err_display("setsockopt()");
				}
				if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
				{
					err_display("setsockopt()");
				}
				
				/*retval = sendto(m_socket,(char *)&b_Ready,sizeof(b_Ready),0,(SOCKADDR*)&m_addr,sizeof(m_addr));
				if(retval ==SOCKET_ERROR)
				{
					err_display("�ε� �Ϸ� sendto()");
					exit(1);
				}
				*/
				//system("cls");
				printf("[Client] �ٲ� ������ ���� �ε� �Ϸ� ��ȣ ����\n");

				addrlen = sizeof(m_addr);
				//�ε��� �Ϸ� �Ǿ��ٴ� ������ ���� ��ȣ�� ����(�׳� �ޱ⸸ �ϴ°� �ǹ̴� ����)
				retval = recvfrom(m_socket,(char *)&sClientAddr,sizeof(sClientAddr),0,(SOCKADDR*)&m_addr,&addrlen);
				if(retval ==SOCKET_ERROR)
				{
					err_display("��ü �ε� �Ϸ� recvfrom()");
					exit(1);
				}
				printf("[Client] �ٲ� ������ ��� �÷��̾��� ���� �غ� �Ϸ�\n");
				optval = 3000;
				if(setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
				{
					err_display("setsockopt()");
				}
				if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
				{
					err_display("setsockopt()");
				}
				serverclose = false;
			}
		}

	}
}

//�޼��� ����
bool CMyGameNetClient::SendMessage()
{
	int retval;

	//���� ������ ������
	printf("������ �޼��� ����\n");
	retval = sendto(m_socket,(char*)&sendMydata,sizeof(sendMydata),0,(SOCKADDR*)&m_addr,sizeof(m_addr));
	if(retval == SOCKET_ERROR)
	{
		// Ver.MadCow SendMessage�� sendto�� ������ �ִٸ� ������ �ٿ� �� ���̴�.
		serverclose	=	true;
		printf("[Client] Server �ٿ�\n");
		return false;
	}	

	bUsePublicData = true;

	return true;
}
//�޼��� �ޱ�
bool CMyGameNetClient::RecvMessage()
{
	SOCKADDR_IN peeraddr;
	int addrlen;
	int retval;


	//packed data �ޱ�
	printf("�������Լ� �޽����� ����\n");
	addrlen = sizeof(peeraddr);
	ZeroMemory(&recvData,sizeof(recvData));
	retval = recvfrom(m_socket,(char*)&recvData,sizeof(recvData),0,(SOCKADDR*)&peeraddr,&addrlen);
	if(retval == SOCKET_ERROR)
	{
		serverclose	= true;
		return false;
	}
	
	EnterCriticalSection(&cs);

	memcpy(&publicPackedData,&recvData,sizeof(recvData));

	LeaveCriticalSection(&cs);	
	return true;
}

//���� ���ҽ� ó�� �Լ�
/*���� ó�� �����忡�� ������ �����͵��� ������ ������ ����
Ŭ���̾�Ʈ Ŭ������ �����͸� �����ϴ� ������ �ϴ� �Լ�
*/
void CMyGameNetClient::publicDataProcess(MYDATA *mData)
{
	memcpy(&sendMydata,mData,sizeof(sendMydata));
	if(bUsePublicData)
	{
		ZeroMemory(mData,sizeof(MYDATA));
		memset(&mData->itemInfo,-1,sizeof(ITEMINFO));
		memset(mData->breakenBlock,-1,sizeof(mData->breakenBlock));
		bUsePublicData = false;
	}
}

//���� ó�� �����忡�� ���� �����Ϳ� �����ϱ� ����
//�ش� �������� �����͸� �Ѱ��ִ� �Լ�
PackedData* CMyGameNetClient::GetPublicData()
{
	return &publicPackedData;
}


//���� �޼��� ��� �ϴ� �Լ�
void CMyGameNetClient::err_display(char *msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,0,NULL);
	MessageBox(NULL,(LPCTSTR)lpMsgBuf,msg,MB_ICONERROR);
	LocalFree(lpMsgBuf);
}


void CMyGameNetClient::CleanUp()
{
	closesocket(m_socket);
	WSACleanup();
}

