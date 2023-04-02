#include "CMyGameNetClient.h"
#include <stdlib.h>
#include <time.h>

//클라이언트 초기화.

void gotoxy(int x, int y)  {
	COORD Cur = {x,y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);

}

bool CMyGameNetClient::Initstance(SOCKADDR_IN servAddr,int numOfClient)
{	
	//윈속 초기화
	if(WSAStartup(MAKEWORD(2,2),&m_wsa) !=0 )
		return false;

	//소켓 초기화
	m_socket = socket(AF_INET,SOCK_DGRAM,0);

	// Ver.MadCow SO_RCVTIMEO,SO_SNDTIMEO 소켓옵션 사용 부분으로써 제한시간은 약 3초로 한다.

	PackedData temp;
	if(m_socket == INVALID_SOCKET)
	{
		err_display("socket()");
		return false;
	}

	//소켓 주소 구조체 초기화
	ZeroMemory(&m_addr,sizeof(m_addr));			//주소정보 구조체
	m_addr = servAddr;
	m_addr.sin_family = AF_INET;
	

	printf("[Client] 클라이언트 초기화 완료\n");

	bUsePublicData = false;
	
	return true;
}

//클라이언트 구동
void CMyGameNetClient::Running(CGameEngine *pGame)
{
	bool b_Ready = true;
	int retval;
	int addrlen=0;

	// Ver.MadCow 예비서버를 지정 하여 준다.
	int nextServerNum = 1;
	// Ver.MadCow 서버의 다운 상태를 false로 하여 다운 되어 있지 않다라고 선언하여 준다.
	serverclose = false;

	//임계영역 가져오기
	cs = pGame->cs;

	//로딩 완료 신호 서버로 보냄.
	retval = sendto(m_socket,(char *)&b_Ready,sizeof(b_Ready),0,(SOCKADDR*)&m_addr,sizeof(m_addr));
	if(retval ==SOCKET_ERROR)
	{
		err_display("로딩 완료 sendto()");
		exit(1);
	}
	printf("[Client] 서버에 게임 로딩 완료 신호 전송\n");
	
	addrlen = sizeof(m_addr);
	//로딩이 완료 되었다는 서버로 부터 신호를 받음(그냥 받기만 하는거 의미는 없음)
	retval = recvfrom(m_socket,(char *)&sClientAddr,sizeof(sClientAddr),0,(SOCKADDR*)&m_addr,&addrlen);
	if(retval ==SOCKET_ERROR)
	{
		err_display("전체 로딩 완료 recvfrom()");
		exit(1);
	}

	printf("[Client] 모든 플레이어의 게임 준비 완료\n");

	int optval = 10000;
	
	if(setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		err_display("setsockopt()");
	}
	
	//게임처리 프로세스에서 종료 명령이 떨어지면 스레드 종료
	while(pGame->Running())
	{
		if(!(serverclose))
		{
			bool ret;
			//서버로 게임 데이터 전달.
			ret = SendMessage();
		
			ret = RecvMessage();		//서버로 부터 packed data를 받음
		
		}
		else
		{
			// Ver.MadCow 미리 정해진 예비 서버의 번호와 자신의 번호가 같다면 쓰레드를 종료시킨다.
			if(nextServerNum == sendMydata.numOfPlayer)
			{
				pGame->serverclose = true;
				serverclose = true;
				break;
			}
			// Ver.MadCow 그렇지 않다면 예비된 서버로의 주소를 바꾸어 준다.
			else
			{
				Sleep(1000);
				ZeroMemory(&m_addr,sizeof(m_addr));			//	주소정보 구조체
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
					err_display("로딩 완료 sendto()");
					exit(1);
				}
				*/
				//system("cls");
				printf("[Client] 바뀐 서버에 게임 로딩 완료 신호 전송\n");

				addrlen = sizeof(m_addr);
				//로딩이 완료 되었다는 서버로 부터 신호를 받음(그냥 받기만 하는거 의미는 없음)
				retval = recvfrom(m_socket,(char *)&sClientAddr,sizeof(sClientAddr),0,(SOCKADDR*)&m_addr,&addrlen);
				if(retval ==SOCKET_ERROR)
				{
					err_display("전체 로딩 완료 recvfrom()");
					exit(1);
				}
				printf("[Client] 바뀐 서버로 모든 플레이어의 게임 준비 완료\n");
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

//메세지 전달
bool CMyGameNetClient::SendMessage()
{
	int retval;

	//게임 데이터 보내기
	printf("서버에 메세지 전송\n");
	retval = sendto(m_socket,(char*)&sendMydata,sizeof(sendMydata),0,(SOCKADDR*)&m_addr,sizeof(m_addr));
	if(retval == SOCKET_ERROR)
	{
		// Ver.MadCow SendMessage의 sendto에 문제가 있다면 서버가 다운 된 것이다.
		serverclose	=	true;
		printf("[Client] Server 다운\n");
		return false;
	}	

	bUsePublicData = true;

	return true;
}
//메세지 받기
bool CMyGameNetClient::RecvMessage()
{
	SOCKADDR_IN peeraddr;
	int addrlen;
	int retval;


	//packed data 받기
	printf("서버에게서 메시지를 받음\n");
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

//공유 리소스 처리 함수
/*게임 처리 스레드에서 저장한 데이터들을 서버로 보내기 위해
클라이언트 클래스에 데이터를 저장하는 역할을 하는 함수
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

//게임 처리 스레드에서 공유 데이터에 접근하기 위해
//해당 데이터의 포인터를 넘겨주는 함수
PackedData* CMyGameNetClient::GetPublicData()
{
	return &publicPackedData;
}


//에러 메세지 출력 하는 함수
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

