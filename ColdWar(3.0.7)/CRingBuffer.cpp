#include "stdafx.h"
#include "CRingBuffer.h"

CRingBuffer::CRingBuffer()
{

	m_pRingBuffer		= NULL;		// 실제 데이터를 저장하는 버퍼 포인터
	m_pBeginMark		= NULL;		// 버퍼의 처음 부분을 가리키고 있는 포인터
	m_pEndMark			= NULL;		// 버퍼의 마지막 부부을 가리키고 있는 포인터
	m_pCurrentMark		= NULL;		// 버퍼의 현재까지 사용된 부분을 가라키고 있는 포인터
	m_pGettedBufferMark	= NULL;		// 현재까지 데이터를 읽은 버퍼 포인터
	m_pLastMoveMark		= NULL;		// recycle되기 전에 마지막 포인터

	m_nUsedBufferSize	= 0;		// 현재 사용된 내부 버퍼의 크기
	m_uiAllUserBufSize	= 0;		// 총 처리된 데이터 양

}


CRingBuffer::~CRingBuffer()
{
	
	if( NULL != m_pBeginMark )
		delete[] m_pBeginMark;

}

bool CRingBuffer::Initialize()
{
	CMonitor::Owner lock( m_csRingBuffer );
	{		
		m_pCurrentMark		= m_pBeginMark;
		m_pGettedBufferMark = m_pBeginMark;
		m_pLastMoveMark		= m_pEndMark;
		m_uiAllUserBufSize	= 0;
		m_nUsedBufferSize	= 0;
	}

	return true;
}

bool CRingBuffer::Create( int nBufferSize )
{

	if( NULL != m_pBeginMark )
		delete[] m_pBeginMark;

	m_pBeginMark = new char[ nBufferSize ];

	if( NULL == m_pBeginMark )
		return false;

	m_pEndMark		= m_pBeginMark + nBufferSize - 1;
	m_nBufferSize	= nBufferSize;

	Initialize();

	return true;
}


char* CRingBuffer::ForwardMark( int nForwardLength )
{

	char *pPreCurrentMark = NULL;

	CMonitor::Owner lock( m_csRingBuffer );
	{
		// 링 버퍼 오버플로 체크 
		if( m_nUsedBufferSize + nForwardLength > m_nBufferSize )
			return NULL;

		if( (m_pEndMark - m_pCurrentMark ) >= nForwardLength )
		{
			pPreCurrentMark = m_pCurrentMark;
			m_pCurrentMark += nForwardLength;
		}
		else
		{
			//순환되기 전 마지막 좌표를 저장
			m_pLastMoveMark = m_pCurrentMark;
			m_pCurrentMark = m_pBeginMark + nForwardLength;
			pPreCurrentMark = m_pBeginMark;
		}

		m_nUsedBufferSize += nForwardLength;
		m_uiAllUserBufSize += nForwardLength;

	}

	//printf( "m_nUsedBufferSize = %d, m_nUsedBufferSize = %d\n", m_nUsedBufferSize, m_uiAllUserBufSize );

	return pPreCurrentMark;

}

char* CRingBuffer::ForwardMark(int nForwardLength, int nNextLength, DWORD dwRemainLength )
{

	CMonitor::Owner lock( m_csRingBuffer );
	{
		//링 버퍼 오버플로 체크
		if( (m_nUsedBufferSize + nForwardLength + nNextLength ) > m_nBufferSize )
			return NULL;

		if( ( m_pEndMark - m_pCurrentMark ) > ( nNextLength + nForwardLength ) )
			m_pCurrentMark += nForwardLength;
		else
		{
			// 순환되기 전 마지막 좌표를 저장
			m_pLastMoveMark = m_pCurrentMark;
			::CopyMemory( m_pBeginMark, m_pCurrentMark - ( dwRemainLength - nForwardLength ), dwRemainLength );
			m_pCurrentMark = m_pBeginMark + dwRemainLength;
		}

		m_nUsedBufferSize += nForwardLength;
		m_uiAllUserBufSize += nForwardLength;
	}

	return m_pCurrentMark;
}

void CRingBuffer::BackwardMark( int nBackwardLength )
{
	CMonitor::Owner lock( m_csRingBuffer );
	{
		//nBackwardLength양만큼 현재 버퍼포인터를 뒤로 보낸다.
		m_nUsedBufferSize -= nBackwardLength;
		m_pCurrentMark -= nBackwardLength;
	}
}

void CRingBuffer::SetUsedBufferSize( int nUsedBufferSize )
{
	CMonitor::Owner lock( m_csRingBuffer );
	{
		m_nUsedBufferSize += nUsedBufferSize;
		m_uiAllUserBufSize += nUsedBufferSize;
	}
}


void CRingBuffer::ReleaseBuffer( int nReleaseSize )
{
	CMonitor::Owner lock( m_csRingBuffer );
	{
		m_nUsedBufferSize -= nReleaseSize;
	}
}

char* CRingBuffer::GetBuffer(int nReadSize, int *pReadSize)
{

	char *pRet = NULL;

	CMonitor::Owner lock( m_csRingBuffer );
	{
		
		//마지막까지 다 읽었다면 그 읽어드릴 버퍼의 포인터는 맨 앞으로 옮긴다.
		if( m_pLastMoveMark == m_pGettedBufferMark )
		{
			m_pGettedBufferMark = m_pBeginMark;
			m_pLastMoveMark		= m_pEndMark;
		}

		// 현재 버퍼에 있는 size가 읽어 드릴 size보다 크다면
		if( m_nUsedBufferSize > nReadSize )
		{
			// 링 버퍼의 끝인지 판단
			if( ( m_pLastMoveMark - m_pGettedBufferMark ) >= nReadSize )
			{
				*pReadSize = nReadSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += nReadSize;
			}
			else
			{
				*pReadSize = (int)( m_pLastMoveMark - m_pGettedBufferMark );
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
		else if( m_nUsedBufferSize > 0 )
		{
			// 링 버퍼의 끝인지 판단.
			if(( m_pLastMoveMark - m_pGettedBufferMark) >= m_nUsedBufferSize )
			{
				*pReadSize = m_nUsedBufferSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += m_nUsedBufferSize;
			}
			else
			{
				*pReadSize = (int)( m_pLastMoveMark - m_pGettedBufferMark );
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
	}

	return pRet;
}



