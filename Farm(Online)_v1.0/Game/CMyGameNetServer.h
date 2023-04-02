#ifndef CMYGAMENETSERVER_H_
#define CMYGAMENETSERVER_H_

#include "NetSystem.h"
#include "common.h"
#include "gameengine.h"

//서버 클래스
class CMyGameNetServer : public CNetSystem
{
public:
	//공통 추상 메소드
	bool Initstance(SOCKADDR_IN servAddr,int numOfClient);	//초기화.
	void Running(CGameEngine *pGame);					//네트워크 구동
	void CleanUp();					//
	bool SendMessage();				//메세지 보내기
	bool RecvMessage();
	void publicDataProcess(MYDATA *mData); //공유 데이터 처리

	PackedData* GetPublicData();	//공유 데이터 반환

	//서버만을 위한 함수
	void Packing();			//데이터 packing

	void err_display(char *msg);

	CRITICAL_SECTION cs;		//임계영역

private:
	vector<MYDATA>	gameData;		//클라이언트에게 받은 데이터를 저장할 변수임니돠
	MYDATA			storageData;	//임시로 저장할 데이터
	PackedData		sendData;		//클라이언트에세 보낼 묶음 데이터

	ClientAddr		sClientAddr;			//Ver.MadCow 클라이언트들의 주소록
	UINT			numOfClient;	//클라이언트들의 수
	//SOCKADDR_IN		clientAddr[6];	//클라이언트들의 주소록
};


#endif


