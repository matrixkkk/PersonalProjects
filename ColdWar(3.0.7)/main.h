//----------------------------------------------------------
//  File: main.h 
//
//  Desc: 메인 어플리케이션의 포함파일/공통 매크로 상수 정의 
//  written by 김현구
//-------------------------------------------------------------  

#ifndef _MAIN_H_
#define _MAIN_H_

#include "protocol.h"

#include <NiCriticalSection.h>
#include <NiThreadProcedure.h>
#include <NiThread.h>

#include <NiMain.h>
//#include <time.h>

#define WIN32_LEAN_AND_MEAN

#if defined(_DX9)
#include <NiDX9Renderer.h> 
#endif

#include <NiDX9SystemDesc.h>
#include <NiImageConverter.h>
#include <NiInputSystem.h>
#include <NiDI8InputSystem.h>
#include <NiSystemCursor.h>
#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiPortal.h>
#include <NiParticle.h>

//#pragma comment(lib, "NiDX9Renderer.lib")
#pragma comment(lib, "dxguid.lib")

//윈도우 생성과 관련된 매크로 상수
#define WINDOWWIDTH  1024				//윈도우의 가로 크기
#define WINDOWHEIGHT 768				//윈도우의 세로 크기
#define FPS 100


//#define MODE_TEST		//3인칭 테스트 모드로 실행
#define NET_TEST
#define FIRE_TEST		//발사 정보가 제대로 전송되고 받는지 테스트 
#define ZFLOG

#pragma warning(disable:4005)

//----------------------------------------------



#endif
