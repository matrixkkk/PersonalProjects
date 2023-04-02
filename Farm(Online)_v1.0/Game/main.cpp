#include "gameengine.h"
#include "playstate.h"
#include "NetSystem.h"


extern CPlayState playState;    //CPlayState ���� ����  

//���� ������ ��
DWORD WINAPI ProcessDataOfServer(LPVOID arg);	//������ �޼��� ó���� ���� ������  
DWORD WINAPI ProcessDataOfClient(LPVOID arg);	//Ŭ���̾�Ʈ�� �޼��� ó���� ���� ������

int main(int argc, char *argv[])
{

	/*char name[6][25] = { "�������� �󱼵�" , "�����̱���","���d�d", "����̱���","�������","��������" };
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
		printf("�ƱԸ�Ʈ�� �ùٸ��� �ʽ��ϴ�\n");
		exit(0);
	}
	
	Argument args;

	 printf("��ť��Ʈ ó�� ����\n");
	//�ƱԸ�Ʈ ó��
    if(argc > 1 )
	 {  
		  ZeroMemory(&args,sizeof(args));

		  //���� addr ����
		  args.serveraddr.sin_addr.s_addr =  inet_addr(argv[1]);
		  args.serveraddr.sin_port  = atoi(argv[2]);  

		  //�� ����
		  args.mapIndex = atoi(argv[3]);

		  //���� �ο�  
		  args.numOfUser = atoi(argv[4]);

		  //�ڱ� ��ȣ  
		  args.currPlayerNum = atoi(argv[5]);

		  printf("map = %d\n",args.mapIndex);
		  printf("�����ο� = %d\n",args.numOfUser);
		  printf("�ڱ� ��ȣ= %d\n",args.currPlayerNum);
		  printf("���� ������= %s",inet_ntoa(args.serveraddr.sin_addr));
		  
		  int argIdx=6;
		  args.playIDs = new char*[6];
		  int countIds = 0;
		  while(argv[argIdx]!=NULL)
		  {   
			   char *pNewChar;
			   pNewChar = argv[argIdx];
			   args.playIDs[countIds++]=pNewChar;
			   argIdx++;
			   printf("���̵� = %s\n",pNewChar);
		  }
	 }

	HANDLE	thread;
	
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048); //����� ���̺귯�� ���� 

	CGameEngine game;                               //������ ���� �ϴ� ���� Ŭ���� ����. 
 		
	//������ �ɼ� 0: ���� 1:Ŭ���̾�Ʈ
	int gameOption = 0;

	if(args.currPlayerNum == 0)
		gameOption = 0;
	else
		gameOption = 1;
	
	game.arg = &args;
	playState.SetArgument(&args);	//�ƱԸ�Ʈ�� �Ѱ���.

	if(gameOption == 0)
	{
		//���� �ʱ�ȭ
		printf("���� �ʱ�ȭ\n");
		game.Init("Farm Server v1.0",1024,768,24,false,0);
		game.ChangeState(&playState);                   //�ʱ� ���¸� playState�� ���� 


		//���� ���� ó�� ������ ����
		printf("============���� ���� PLUS v5.1(����) Server Process==============\n");
		thread = CreateThread(NULL,0,ProcessDataOfServer,(LPVOID)&game,0,NULL);
		if(thread == NULL)
			cout<<"������ ���� ����"<<endl;
	}
	else
	{
		//���� �ʱ�ȭ
		game.Init("Farm Client v1.0",1024,768,24,false,1);
		game.ChangeState(&playState);                   //�ʱ� ���¸� playState�� ���� 

		//Ŭ���̾�Ʈ ���� ó�� ������ ����
		printf("============���� ���� PLUS v5.1(����) Client Process==============\n");
		thread = CreateThread(NULL,0,ProcessDataOfClient,(LPVOID)&game,0,NULL);	//������ ���� �� ������ ����.
		
		if(thread == NULL)
			cout<<"������ ���� ����"<<endl;	
	}
	
	while ( game.Running() )
	{	
		//printf("���� ��ƾ ����\n");
		game.HandleEvents();                          //�̺�Ʈ ó�� 
		
		game.Update();                                //Draw �ܿ� �ΰ����� ���� ������Ʈ ó�� 
		//printf("[Run] Update Data\n");
		
		game.StorageData();							  //���������Ϳ� ���� ������ ����
		//printf("[Run] Storage Data\n");
		
		game.Draw();                                  //�̹��� �׸��� ���⼭ ó�� 
		//printf("[Run] Draw\n");
		
		game.RegulateFPS();                           //������ ����ȭ 
		game.UpdateScreen();                          //������ �ø��� 	
		
		// Ver.MadCow game�� serverclose�� ���̸� ������ �ٿ� �Ǿ����� ���� ������
		// �ڽ��̹Ƿ� �����ν� ������Ѵ�.
		if(game.serverclose)
		{
			//���� ������ ó�� ������ �����ϰ� ���� 
			/*CNetSystem *netsystem = game.GetNetClass();
			netsystem->CleanUp();
			delete netsystem;
			*/
			game.Init("Farm Server v1.0",1024,768,24,false,0);
			game.changeServerCount++;
			//game.ChangeState(&playState);                   //�ʱ� ���¸� playState�� ���� 
			system("cls");
			//���� ���� ó�� ������ ����
			printf("============���� ���� PLUS v2.1 Server Process==============\n");
			printf("���� ���� �Ϸ�\n");
			
			thread = CreateThread(NULL,0,ProcessDataOfServer,(LPVOID)&game,0,NULL);
			if(thread == NULL)
				cout<<"������ ���� ����"<<endl;
			game.serverclose = false;
			Sleep(3000);
		}
	}
	//��Ʈ��ũ �������� ���Ḧ ��ٸ�
	printf("��Ʈ��ũ �������� ����\n");
	TerminateThread(thread,0);
	delete []args.playIDs;
	Mix_CloseAudio();                               //����� ���̺귯�� �ݱ� 
	
	game.Cleanup();                                 //���� �ʱ�ȭ 	
	SDL_Quit();
	return 0;
}
 



//Ŭ���̾�Ʈ�� ������ ������ ���� ������
DWORD WINAPI ProcessDataOfClient(LPVOID arg)
{
	CGameEngine *game = (CGameEngine*)arg;
	
	CNetSystem *gameNet;				//��Ʈ��ũ�� ó���� ���� ������ ����

	gameNet = game->GetNetClass();		//��Ʈ��ũ ��ü�� �����͸� ������.
		
	bool retval;
	//��Ʈ��ũ �޼��� ó��
	retval = gameNet->Initstance(game->arg->serveraddr,game->arg->numOfUser-1);		
	if(retval == false)
	{
		MessageBox(NULL,NULL,"��Ʈ��ũ �ʱ�ȭ ����\n",MB_OK);
		return 0;
	}
	Sleep(2000);
	gameNet->Running(game);
	gameNet->CleanUp();

	if(gameNet)
		delete gameNet;

	return 0;
}


//������ ������ ������ ���� ������
DWORD WINAPI ProcessDataOfServer(LPVOID arg)
{	
	//
	printf("������ ����\n");
	CGameEngine *game = (CGameEngine*)arg;
	
	CNetSystem *gameNet;				//��Ʈ��ũ�� ó���� ���� ������ ����

	gameNet = game->GetNetClass();		//��Ʈ��ũ ��ü�� �����͸� ������.

	bool retval;
	//��Ʈ��ũ �޼��� ó��
	printf("���� �ʱ�ȭ\n");
	retval = gameNet->Initstance(game->arg->serveraddr,game->arg->numOfUser-1);		
	if(retval == false)
	{
		MessageBox(NULL,NULL,"��Ʈ��ũ �ʱ�ȭ ����\n",MB_OK);
		return 0;
	}
	printf("���� ���� ����\n");
	gameNet->Running(game);
	gameNet->CleanUp();

	if(gameNet)
		delete gameNet;

	printf("���� ������ ����\n");

	return 0;
}