#include "CMyGameNetServer.h"



bool CMyGameNetServer::Initstance(SOCKADDR_IN servAddr,int numOfClient)
{	
	//���� �ʱ�ȭ
	if(WSAStartup(MAKEWORD(2,2),&m_wsa) !=0 )
		return false;
	
	//���� �ʱ�ȭ
	m_socket = socket(AF_INET,SOCK_DGRAM,0);
	if(m_socket == INVALID_SOCKET)
	{
		err_display("socket()");
		return false;
	}
	
	//���� �ּ� ����ü �ʱ�ȭ
	ZeroMemory(&m_addr,sizeof(m_addr));			//�ּ����� ����ü
	m_addr = servAddr;
	m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_addr.sin_family = AF_INET;
	
	
	//bind()
	int retval = bind(m_socket,(SOCKADDR*)&m_addr,sizeof(m_addr));
	if(retval == SOCKET_ERROR)
	{
		err_display("bind()");
		return false;
	}

	this->numOfClient = numOfClient;	//Ŭ���̾�Ʈ���� ��
	bUsePublicData = false;
	
	return true;
}

//Ŭ���̾�Ʈ ����
void CMyGameNetServer::Running(CGameEngine *pGame)
{
	int addrlen=0;
	int retval;
	bool b_Ready[6];
	

	//���� �ɼ� ����
	int optval = 3000;
	if(setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&optval,sizeof(optval))==SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&optval,sizeof(optval))==SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	
	//�Ӱ迵�� ��������
	cs = pGame->cs;

	ZeroMemory(sClientAddr.Addr,sizeof(sClientAddr.Addr));
	//�ε� �Ϸ� ��ȣ ������ Ŭ���̾�Ʈ �鿡�Լ� ����
	for(UINT i=0;i<numOfClient;i++)
	{
		//������� ����
		addrlen = sizeof(sClientAddr.Addr[i]);
		retval = recvfrom(m_socket,(char *)&b_Ready[i],sizeof(bool),0,(SOCKADDR*)&sClientAddr.Addr[i],&addrlen);
		if(retval ==SOCKET_ERROR)
		{
			//err_display("recvfrom()");
			continue;
		}
		printf("%s Ŭ���̾�Ʈ���� �ε��Ϸ� �޼��� ����\n",inet_ntoa(sClientAddr.Addr[i].sin_addr));
	}
	//������ ��� ��� (�ӽ�)
	cout<<"��� �÷��̾� �ε� �Ϸ�"<<endl;
	cout<<"=========������ ���==========\n";
	for(UINT i=0;i<numOfClient;i++)
	{		
		cout<<"[UDP Player No."<< i <<" "<< inet_ntoa(sClientAddr.Addr[i].sin_addr) << " / " << ntohs(sClientAddr.Addr[i].sin_port)<<"] ����"<<endl;
	}
	//��� Ŭ���̾�Ʈ�� �غ� �Ǿ��ٴ� ����� Ŭ���̾�Ʈ �鿡�� �˸�
	for(UINT i=0;i<numOfClient;i++)
	{
		sendto(m_socket,(char *)&sClientAddr,sizeof(sClientAddr),0,(SOCKADDR*)&sClientAddr.Addr[i],sizeof(sClientAddr.Addr[i]));
		if(retval ==SOCKET_ERROR)
		{
			//err_display("sendto()");
			continue;
		}
	}
	printf("�غ� �Ϸ� �޼��� Ŭ���̾�Ʈ �鿡�� ���� �Ϸ�\n");

	//�޼��� ����	
	while(pGame->Running())
	{
		bool ret;
		//Ŭ���̾�Ʈ �鿡�Լ� �����͸� ����
		ret = RecvMessage();
		if(ret==false)
		{
			MessageBox(NULL,NULL,"Recv����",MB_OK);
			break;
		}		
		//������ ���� ������ ����.
		ret = SendMessage();
		if(ret==false)
		{
			MessageBox(NULL,NULL,"Send����",MB_OK);
			break;
		}

		Packing();			//������ packing
		
	}
	printf("��Ʈ��ũ ��ƾ ����\n");
}

//���ӿ��� ó�� �ϱ� ���� ���� �����ͷ� ����
void CMyGameNetServer::Packing()
{
	EnterCriticalSection(&cs);
	
	memcpy(&publicPackedData,&sendData,sizeof(sendData));

	LeaveCriticalSection(&cs);
}

//�޼��� ����
bool CMyGameNetServer::SendMessage()
{
	int retval;
	
	//���� ������ ������
	for(unsigned int i = 0 ; i < numOfClient ; i++)
	{
		printf("%d Ŭ���̾�Ʈ�� �޽��� ����\n");
		retval = sendto(m_socket,(char *)&publicPackedData,sizeof(publicPackedData),0,(SOCKADDR*)&sClientAddr.Addr[i],sizeof(sClientAddr.Addr[i]));
		if(retval == SOCKET_ERROR)
		{
			//printf("SendMessage()");
			//err_display("sendto()");
			continue;
		}		
	}	
	
	return true;
}
//�޼��� �ޱ�
bool CMyGameNetServer::RecvMessage()
{
	SOCKADDR_IN peeraddr;
	int addrlen;
	vector<MYDATA>::iterator pos;
	MYDATA temp;
	int retval;

	addrlen = sizeof(peeraddr);
	
	//�����̳� Ŭ����
	gameData.clear();
	//���� �����͸� ���� ó���� ����
	gameData.push_back(storageData);	
	
	//�÷��̾� �� ��ŭ recv �մϴ�.
	for(unsigned int i=0; i<numOfClient ;i++)
	{
		printf("%d Ŭ���̾�Ʈ�� ���� �޽��� ����\n");
		ZeroMemory(&temp,sizeof(temp));
		retval = recvfrom(m_socket,(char *)&temp,sizeof(MYDATA),0,(SOCKADDR*)&peeraddr,&addrlen);
		if(retval == SOCKET_ERROR)
		{		
			//err_display("recvfrom()");
			continue;
		}	
		//���� ������ �����̳ʿ� ����
		gameData.push_back(temp);
	}
	
	bUsePublicData = true;


	//Ŭ���̾�Ʈ���� ���� �����͸� ����.
	for(unsigned int i=0; i<gameData.size() ;i++)
	{
		UINT num = gameData[i].numOfPlayer;			//�÷��̾� �ѹ�
		sendData.packedGameData[num] = gameData[i]; //�÷��̾� �ѹ��� �´� �迭�� �ε����� ����
		sendData.emptyPackageIndex[num] = true;		//�ش� �ε����� ��� ���� �ε���
	}
	//�����̳� ����
	gameData.clear();
	
	return true;
}	


//���� ���ҽ� ó�� �Լ�
/*  ���� ������ �ϴ� ���μ����� packed�޼����� ������ ����
	�ڱ��� �����͸� ���� ó������ �ִ� ���� �ϴ� �Լ� �Դϴ�.
*/
void CMyGameNetServer::publicDataProcess(MYDATA *mData)
{
	memcpy(&storageData,mData,sizeof(storageData));

	//������ Ŭ���̾�Ʈ �ʿ��� �����⸦ �Ϸ� ���� ��츸 ó��
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
PackedData*  CMyGameNetServer::GetPublicData()
{
	return &publicPackedData;
}


//���� �޼��� ��� �ϴ� �Լ�
void CMyGameNetServer::err_display(char *msg)
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


void CMyGameNetServer::CleanUp()
{
	closesocket(m_socket);
	WSACleanup();
}



