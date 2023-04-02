//----------------------------------------------------------
//  File: main.h 
//
//  Desc: ���� ���ø����̼��� ��������/���� ��ũ�� ��� ���� 
//  written by ������
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

//������ ������ ���õ� ��ũ�� ���
#define WINDOWWIDTH  1024				//�������� ���� ũ��
#define WINDOWHEIGHT 768				//�������� ���� ũ��
#define FPS 100


//#define MODE_TEST		//3��Ī �׽�Ʈ ���� ����
#define NET_TEST
#define FIRE_TEST		//�߻� ������ ����� ���۵ǰ� �޴��� �׽�Ʈ 
#define ZFLOG

#pragma warning(disable:4005)

//----------------------------------------------



#endif
