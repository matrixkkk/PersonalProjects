#pragma once

#include "CMonitor.h"

#define	MAX_RINGBUFSIZE 4096

class CRingBuffer : public CMonitor
{

public:

	CRingBuffer();
	~CRingBuffer();

	// 링 버퍼 메모리 할당
	bool Create( int nBufferSize = MAX_RINGBUFSIZE );

	// 초기화
	bool Initialize();

	// 할당된 버퍼 크기를 반환한다.
	inline int GetBufferSize(){ return m_nBufferSize; }

	// 해당하는 내부 버퍼의 포인터를 반환
	inline char* GetBeginMark()		{ return m_pBeginMark;		}
	inline char* GetCurrentMark()	{ return m_pCurrentMark;	}
	inline char* GetEndMark()		{ return m_pEndMark;		}

	//내부 버퍼의 현재 포인터를 전진
	char*	ForwardMark( int nForwardLength );
	char*	ForwardMark( int nForwardLength, int nNextLength, DWORD dwRemainLength );
	void	BackwardMark( int nBackwardLength );

	// 사용된 내부 버퍼 해제
	void	ReleaseBuffer( int nReleaseSize );

	// 사용된 내부 버퍼 크기 반환
	int		GetUsedBufferSize(){ return m_nUsedBufferSize; }

	// 누적 버퍼사용양 반환
	int		GetAllUsedBufferSize(){ return m_uiAllUserBufSize; }

	// 내부 버퍼 데이터 읽어서 반환
	char*	GetBuffer( int nReadSize, int *pReadSize );

	void	SetUsedBufferSize( int nUsedBufferSize );

private:

	char*	m_pRingBuffer;			// 실제 데이터를 저장하는 버퍼 포인터
	char*	m_pBeginMark;			// 버퍼의 처음 부분을 가리키고 있는 포인터
	char*	m_pEndMark;				// 버퍼의 마지막 부부을 가리키고 있는 포인터
	char*	m_pCurrentMark;			// 버퍼의 현재까지 사용된 부분을 가라키고 있는 포인터
	char*	m_pGettedBufferMark;	// 현재까지 데이터를 읽은 버퍼 포인터
	char*	m_pLastMoveMark;		// recycle되기 전에 마지막 포인터

	int				m_nBufferSize;		// 내부 버퍼의 총 크기
	int				m_nUsedBufferSize;	// 현재 사용된 내부 버퍼의 크기
	unsigned int	m_uiAllUserBufSize;	// 총 처리된 데이터 양


	CMonitor		m_csRingBuffer;			// 동기화 객체

private:

	CRingBuffer( const CRingBuffer &rhs );
	CRingBuffer &operator=( const CRingBuffer &rhs );


};