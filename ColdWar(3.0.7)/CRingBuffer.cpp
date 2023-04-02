#include "stdafx.h"
#include "CRingBuffer.h"

CRingBuffer::CRingBuffer()
{

	m_pRingBuffer		= NULL;		// ���� �����͸� �����ϴ� ���� ������
	m_pBeginMark		= NULL;		// ������ ó�� �κ��� ����Ű�� �ִ� ������
	m_pEndMark			= NULL;		// ������ ������ �κ��� ����Ű�� �ִ� ������
	m_pCurrentMark		= NULL;		// ������ ������� ���� �κ��� ����Ű�� �ִ� ������
	m_pGettedBufferMark	= NULL;		// ������� �����͸� ���� ���� ������
	m_pLastMoveMark		= NULL;		// recycle�Ǳ� ���� ������ ������

	m_nUsedBufferSize	= 0;		// ���� ���� ���� ������ ũ��
	m_uiAllUserBufSize	= 0;		// �� ó���� ������ ��

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
		// �� ���� �����÷� üũ 
		if( m_nUsedBufferSize + nForwardLength > m_nBufferSize )
			return NULL;

		if( (m_pEndMark - m_pCurrentMark ) >= nForwardLength )
		{
			pPreCurrentMark = m_pCurrentMark;
			m_pCurrentMark += nForwardLength;
		}
		else
		{
			//��ȯ�Ǳ� �� ������ ��ǥ�� ����
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
		//�� ���� �����÷� üũ
		if( (m_nUsedBufferSize + nForwardLength + nNextLength ) > m_nBufferSize )
			return NULL;

		if( ( m_pEndMark - m_pCurrentMark ) > ( nNextLength + nForwardLength ) )
			m_pCurrentMark += nForwardLength;
		else
		{
			// ��ȯ�Ǳ� �� ������ ��ǥ�� ����
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
		//nBackwardLength�縸ŭ ���� ���������͸� �ڷ� ������.
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
		
		//���������� �� �о��ٸ� �� �о�帱 ������ �����ʹ� �� ������ �ű��.
		if( m_pLastMoveMark == m_pGettedBufferMark )
		{
			m_pGettedBufferMark = m_pBeginMark;
			m_pLastMoveMark		= m_pEndMark;
		}

		// ���� ���ۿ� �ִ� size�� �о� �帱 size���� ũ�ٸ�
		if( m_nUsedBufferSize > nReadSize )
		{
			// �� ������ ������ �Ǵ�
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
			// �� ������ ������ �Ǵ�.
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



