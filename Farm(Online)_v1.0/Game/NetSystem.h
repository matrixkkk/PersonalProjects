#ifndef NETSYSTEM_H_
#define NETSYSTEM_H_

#include "common.h"


class CGameEngine;		//전방 선언

//네트워크 공통 클래스
class CNetSystem
{
protected:

	WSADATA			m_wsa;		
	SOCKET			m_socket;
	SOCKADDR_IN		m_addr;			//주소정보 구조체

	PackedData		publicPackedData;		//공유를 위한 packed Data (기획서 수정)
	bool			bUsePublicData;
public:
	virtual bool Initstance(SOCKADDR_IN servAddr,int numOfClient)=0;				//초기화.
	virtual void Running(CGameEngine *pGame)=0;					//네트워크 구동
	virtual void CleanUp()=0;					//
	virtual bool SendMessage()=0;				//메세지 보내기
	virtual bool RecvMessage()=0;				//메세지 받기
	virtual void publicDataProcess(MYDATA *mData) = 0;		//공유 데이터 처리(기획서 수정)
	
	virtual PackedData* GetPublicData()=0;		//공유 데이터 반환(기획서추가)
};

#endif