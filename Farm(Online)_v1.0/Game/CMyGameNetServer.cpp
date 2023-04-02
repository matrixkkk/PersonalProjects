#include "CMyGameNetServer.h"



bool CMyGameNetServer::Initstance(SOCKADDR_IN servAddr,int numOfClient)
{	
	//윈속 초기화
	if(WSAStartup(MAKEWORD(2,2),&m_wsa) !=0 )
		return false;
	
	//소켓 초기화
	m_socket = socket(AF_INET,SOCK_DGRAM,0);
	if(m_socket == INVALID_SOCKET)
	{
		err_display("socket()");
		return false;
	}
	
	//소켓 주소 구조체 초기화
	ZeroMemory(&m_addr,sizeof(m_addr));			//주소정보 구조체
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

	this->numOfClient = numOfClient;	//클라이언트들의 수
	bUsePublicData = false;
	
	return true;
}

//클라이언트 구동
void CMyGameNetServer::Running(CGameEngine *pGame)
{
	int addrlen=0;
	int retval;
	bool b_Ready[6];
	

	//소켓 옵션 설정
	int optval = 3000;
	if(setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&optval,sizeof(optval))==SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&optval,sizeof(optval))==SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	
	//임계영역 가져오기
	cs = pGame->cs;

	ZeroMemory(sClientAddr.Addr,sizeof(sClientAddr.Addr));
	//로딩 완료 신호 서버를 클라이언트 들에게서 받음
	for(UINT i=0;i<numOfClient;i++)
	{
		//여기부터 수정
		addrlen = sizeof(sClientAddr.Addr[i]);
		retval = recvfrom(m_socket,(char *)&b_Ready[i],sizeof(bool),0,(SOCKADDR*)&sClientAddr.Addr[i],&addrlen);
		if(retval ==SOCKET_ERROR)
		{
			//err_display("recvfrom()");
			continue;
		}
		printf("%s 클라이언트에게 로딩완료 메세지 받음\n",inet_ntoa(sClientAddr.Addr[i].sin_addr));
	}
	//접속자 명단 출력 (임시)
	cout<<"모든 플레이어 로딩 완료"<<endl;
	cout<<"=========접속자 명단==========\n";
	for(UINT i=0;i<numOfClient;i++)
	{		
		cout<<"[UDP Player No."<< i <<" "<< inet_ntoa(sClientAddr.Addr[i].sin_addr) << " / " << ntohs(sClientAddr.Addr[i].sin_port)<<"] 접속"<<endl;
	}
	//모든 클라이언트가 준비가 되었다는 사실을 클라이언트 들에게 알림
	for(UINT i=0;i<numOfClient;i++)
	{
		sendto(m_socket,(char *)&sClientAddr,sizeof(sClientAddr),0,(SOCKADDR*)&sClientAddr.Addr[i],sizeof(sClientAddr.Addr[i]));
		if(retval ==SOCKET_ERROR)
		{
			//err_display("sendto()");
			continue;
		}
	}
	printf("준비 완료 메세지 클라이언트 들에게 전송 완료\n");

	//메세지 루프	
	while(pGame->Running())
	{
		bool ret;
		//클라이언트 들에게서 데이터를 받음
		ret = RecvMessage();
		if(ret==false)
		{
			MessageBox(NULL,NULL,"Recv실패",MB_OK);
			break;
		}		
		//서버로 게임 데이터 전달.
		ret = SendMessage();
		if(ret==false)
		{
			MessageBox(NULL,NULL,"Send실패",MB_OK);
			break;
		}

		Packing();			//데이터 packing
		
	}
	printf("네트워크 루틴 종료\n");
}

//게임에서 처리 하기 위한 공유 데이터로 복사
void CMyGameNetServer::Packing()
{
	EnterCriticalSection(&cs);
	
	memcpy(&publicPackedData,&sendData,sizeof(sendData));

	LeaveCriticalSection(&cs);
}

//메세지 전달
bool CMyGameNetServer::SendMessage()
{
	int retval;
	
	//게임 데이터 보내기
	for(unsigned int i = 0 ; i < numOfClient ; i++)
	{
		printf("%d 클라이언트로 메시지 전송\n");
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
//메세지 받기
bool CMyGameNetServer::RecvMessage()
{
	SOCKADDR_IN peeraddr;
	int addrlen;
	vector<MYDATA>::iterator pos;
	MYDATA temp;
	int retval;

	addrlen = sizeof(peeraddr);
	
	//컨테이너 클리어
	gameData.clear();
	//서버 데이터를 제일 처음에 저장
	gameData.push_back(storageData);	
	
	//플레이어 수 많큼 recv 합니다.
	for(unsigned int i=0; i<numOfClient ;i++)
	{
		printf("%d 클라이언트로 부터 메시지 받음\n");
		ZeroMemory(&temp,sizeof(temp));
		retval = recvfrom(m_socket,(char *)&temp,sizeof(MYDATA),0,(SOCKADDR*)&peeraddr,&addrlen);
		if(retval == SOCKET_ERROR)
		{		
			//err_display("recvfrom()");
			continue;
		}	
		//받은 데이터 컨테이너에 저장
		gameData.push_back(temp);
	}
	
	bUsePublicData = true;


	//클라이언트에게 보낼 데이터를 만듬.
	for(unsigned int i=0; i<gameData.size() ;i++)
	{
		UINT num = gameData[i].numOfPlayer;			//플레이어 넘버
		sendData.packedGameData[num] = gameData[i]; //플레이어 넘버에 맞는 배열의 인덱스에 대입
		sendData.emptyPackageIndex[num] = true;		//해당 인덱스는 사용 중인 인덱스
	}
	//컨테이너 비우기
	gameData.clear();
	
	return true;
}	


//공유 리소스 처리 함수
/*  서버 역할을 하는 프로세스가 packed메세지를 보내기 위해
	자기의 데이터를 제일 처음으로 넣는 일을 하는 함수 입니다.
*/
void CMyGameNetServer::publicDataProcess(MYDATA *mData)
{
	memcpy(&storageData,mData,sizeof(storageData));

	//서버나 클라이언트 쪽에서 보내기를 완료 했을 경우만 처리
	if(bUsePublicData)
	{
		ZeroMemory(mData,sizeof(MYDATA));
		memset(&mData->itemInfo,-1,sizeof(ITEMINFO));
		memset(mData->breakenBlock,-1,sizeof(mData->breakenBlock));
		bUsePublicData = false;
	}
}

//게임 처리 스레드에서 공유 데이터에 접근하기 위해
//해당 데이터의 포인터를 넘겨주는 함수
PackedData*  CMyGameNetServer::GetPublicData()
{
	return &publicPackedData;
}


//에러 메세지 출력 하는 함수
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



