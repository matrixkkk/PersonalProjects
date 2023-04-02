#ifndef CMYGAMENETCLIENT_H_
#define CMYGAMENETCLIENT_H_

#include "common.h"
#include "NetSystem.h"
#include "gameengine.h"

class CMyGameNetClient : public CNetSystem
{
public:
	//오버라이딩 함수
	bool Initstance(SOCKADDR_IN servAddr,int numOfClient);				//초기화.
	void Running(CGameEngine *pGame);					//네트워크 구동
	void CleanUp();					//
	bool SendMessage();				//메세지 보내기
	bool RecvMessage();				//메세지 받기
	void publicDataProcess(MYDATA *mData); //공유 데이터 처리
	
	PackedData* GetPublicData();	//공유 데이터 반환

	//Client 클래스만을 위한 함수
	
	void UnPacking();				//Packed Data 언팩
	void collectData();				//서버에 데이터를 보내기 위해 구조체로 모으기 위한 함수

	//오류 출력
	void err_display(char *msg);

	bool serverclose;				// Ver.MadCow SO_SNDTIMEO,SO_RCVTIMEO 소켓함수 옵션을 사용하여 서버가
									// 다운되었는지 아닌지를 확인하는 bool타입 변수

	CRITICAL_SECTION cs;			//임계영역

private:	
	MYDATA		   sendMydata;	//서버에 보내기 위한 클라이언트 데이터
	PackedData	   recvData;	//서버에게 받을 묶음 데이터
	ClientAddr		sClientAddr;	// Ver.MadCow 기존 서버가 다운될 시 서버를 교체하기 위하여 각 서버의 주소를 가지고 있다.

};

#endif
