#include "gameengine.h"
#include "playstate.h"
#include "NetSystem.h"


extern CPlayState playState;    //CPlayState 전방 선언  

//사용될 스레드 들
DWORD WINAPI ProcessDataOfServer(LPVOID arg);	//서버의 메세지 처리를 위한 스레드  
DWORD WINAPI ProcessDataOfClient(LPVOID arg);	//클라이언트의 메세지 처리를 위한 스레드

int main(int argc, char *argv[])
{

	/*char name[6][25] = { "김현구와 얼굴들" , "변태이광우","아햏햏", "개페미군단","어리버리뿡","삽질마왕" };
	int map= 8,user = 2,num = 0;
	SOCKADDR_IN addrs;
	addrs.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrs.sin_family = AF_INET;
	addrs.sin_port = htons(10000);
	argv[0] = (char*)&addrs;
	argv[1] = (char*)&map;
	argv[2] = (char*)&user;
	argv[3] = (char*)&num;	
	argv[4] = name[0];
	argv[5] = name[1];
	argv[6] = name[2];
	argv[7] = name[3];
	argv[8] = name[4];
	argv[9] = name[5];
	argv[10] = 0;
	argc = 2;
	*/
	if(argc < 2)
	{
		printf("아규먼트가 올바르지 않습니다\n");
		exit(0);
	}
	
	Argument args;

	 printf("아큐먼트 처리 시작\n");
	//아규먼트 처리
    if(argc > 1 )
	 {  
		  ZeroMemory(&args,sizeof(args));

		  //서버 addr 정보
		  args.serveraddr.sin_addr.s_addr =  inet_addr(argv[1]);
		  args.serveraddr.sin_port  = atoi(argv[2]);  

		  //맵 정보
		  args.mapIndex = atoi(argv[3]);

		  //참여 인원  
		  args.numOfUser = atoi(argv[4]);

		  //자기 번호  
		  args.currPlayerNum = atoi(argv[5]);

		  printf("map = %d\n",args.mapIndex);
		  printf("참여인원 = %d\n",args.numOfUser);
		  printf("자기 번호= %d\n",args.currPlayerNum);
		  printf("서버 아이피= %s",inet_ntoa(args.serveraddr.sin_addr));
		  
		  int argIdx=6;
		  args.playIDs = new char*[6];
		  int countIds = 0;
		  while(argv[argIdx]!=NULL)
		  {   
			   char *pNewChar;
			   pNewChar = argv[argIdx];
			   args.playIDs[countIds++]=pNewChar;
			   argIdx++;
			   printf("아이디 = %s\n",pNewChar);
		  }
	 }

	HANDLE	thread;
	
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048); //오디오 라이브러리 오픈 

	CGameEngine game;                               //게임을 관리 하는 엔진 클래스 선언. 
 		
	//게임의 옵션 0: 서버 1:클라이언트
	int gameOption = 0;

	if(args.currPlayerNum == 0)
		gameOption = 0;
	else
		gameOption = 1;
	
	game.arg = &args;
	playState.SetArgument(&args);	//아규먼트들 넘겨줌.

	if(gameOption == 0)
	{
		//게임 초기화
		printf("게임 초기화\n");
		game.Init("Farm Server v1.0",1024,768,24,false,0);
		game.ChangeState(&playState);                   //초기 상태를 playState로 설정 


		//서버 게임 처리 스레드 생성
		printf("============동물 농장 PLUS v5.1(최종) Server Process==============\n");
		thread = CreateThread(NULL,0,ProcessDataOfServer,(LPVOID)&game,0,NULL);
		if(thread == NULL)
			cout<<"스레드 생성 실패"<<endl;
	}
	else
	{
		//게임 초기화
		game.Init("Farm Client v1.0",1024,768,24,false,1);
		game.ChangeState(&playState);                   //초기 상태를 playState로 설정 

		//클라이언트 게임 처리 스레드 생성
		printf("============동물 농장 PLUS v5.1(최종) Client Process==============\n");
		thread = CreateThread(NULL,0,ProcessDataOfClient,(LPVOID)&game,0,NULL);	//데이터 전송 용 스레드 생성.
		
		if(thread == NULL)
			cout<<"스레드 생성 실패"<<endl;	
	}
	
	while ( game.Running() )
	{	
		//printf("게임 루틴 시작\n");
		game.HandleEvents();                          //이벤트 처리 
		
		game.Update();                                //Draw 외에 부가적인 게임 업데이트 처리 
		//printf("[Run] Update Data\n");
		
		game.StorageData();							  //공유데이터에 게임 데이터 저장
		//printf("[Run] Storage Data\n");
		
		game.Draw();                                  //이미지 그리기 여기서 처리 
		//printf("[Run] Draw\n");
		
		game.RegulateFPS();                           //프레임 동기화 
		game.UpdateScreen();                          //페이지 플리핑 	
		
		// Ver.MadCow game의 serverclose가 참이면 서버가 다운 되었으며 예비 서버가
		// 자신이므로 서버로써 재생성한다.
		if(game.serverclose)
		{
			//기존 데이터 처리 스레드 말끔하게 정리 
			/*CNetSystem *netsystem = game.GetNetClass();
			netsystem->CleanUp();
			delete netsystem;
			*/
			game.Init("Farm Server v1.0",1024,768,24,false,0);
			game.changeServerCount++;
			//game.ChangeState(&playState);                   //초기 상태를 playState로 설정 
			system("cls");
			//서버 게임 처리 스레드 생성
			printf("============동물 농장 PLUS v2.1 Server Process==============\n");
			printf("서버 이전 완료\n");
			
			thread = CreateThread(NULL,0,ProcessDataOfServer,(LPVOID)&game,0,NULL);
			if(thread == NULL)
				cout<<"스레드 생성 실패"<<endl;
			game.serverclose = false;
			Sleep(3000);
		}
	}
	//네트워크 스레드의 종료를 기다림
	printf("네트워크 스레드의 종료\n");
	TerminateThread(thread,0);
	delete []args.playIDs;
	Mix_CloseAudio();                               //오디오 라이브러리 닫기 
	
	game.Cleanup();                                 //게임 초기화 	
	SDL_Quit();
	return 0;
}
 



//클라이언트의 데이터 전송을 위한 스레드
DWORD WINAPI ProcessDataOfClient(LPVOID arg)
{
	CGameEngine *game = (CGameEngine*)arg;
	
	CNetSystem *gameNet;				//네트워크를 처리를 위한 포인터 변수

	gameNet = game->GetNetClass();		//네트워크 객체의 포인터를 가져옴.
		
	bool retval;
	//네트워크 메세지 처리
	retval = gameNet->Initstance(game->arg->serveraddr,game->arg->numOfUser-1);		
	if(retval == false)
	{
		MessageBox(NULL,NULL,"네트워크 초기화 실패\n",MB_OK);
		return 0;
	}
	Sleep(2000);
	gameNet->Running(game);
	gameNet->CleanUp();

	if(gameNet)
		delete gameNet;

	return 0;
}


//서버의 데이터 전송을 위한 스레드
DWORD WINAPI ProcessDataOfServer(LPVOID arg)
{	
	//
	printf("스레드 시작\n");
	CGameEngine *game = (CGameEngine*)arg;
	
	CNetSystem *gameNet;				//네트워크를 처리를 위한 포인터 변수

	gameNet = game->GetNetClass();		//네트워크 객체의 포인터를 가져옴.

	bool retval;
	//네트워크 메세지 처리
	printf("서버 초기화\n");
	retval = gameNet->Initstance(game->arg->serveraddr,game->arg->numOfUser-1);		
	if(retval == false)
	{
		MessageBox(NULL,NULL,"네트워크 초기화 실패\n",MB_OK);
		return 0;
	}
	printf("서버 구동 시작\n");
	gameNet->Running(game);
	gameNet->CleanUp();

	if(gameNet)
		delete gameNet;

	printf("서버 스레드 종료\n");

	return 0;
}