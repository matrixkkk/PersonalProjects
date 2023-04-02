#pragma once

#include "CMonitor.h"

#define	MAX_RINGBUFSIZE 4096

class CRingBuffer : public CMonitor
{

public:

	CRingBuffer();
	~CRingBuffer();

	// �� ���� �޸� �Ҵ�
	bool Create( int nBufferSize = MAX_RINGBUFSIZE );

	// �ʱ�ȭ
	bool Initialize();

	// �Ҵ�� ���� ũ�⸦ ��ȯ�Ѵ�.
	inline int GetBufferSize(){ return m_nBufferSize; }

	// �ش��ϴ� ���� ������ �����͸� ��ȯ
	inline char* GetBeginMark()		{ return m_pBeginMark;		}
	inline char* GetCurrentMark()	{ return m_pCurrentMark;	}
	inline char* GetEndMark()		{ return m_pEndMark;		}

	//���� ������ ���� �����͸� ����
	char*	ForwardMark( int nForwardLength );
	char*	ForwardMark( int nForwardLength, int nNextLength, DWORD dwRemainLength );
	void	BackwardMark( int nBackwardLength );

	// ���� ���� ���� ����
	void	ReleaseBuffer( int nReleaseSize );

	// ���� ���� ���� ũ�� ��ȯ
	int		GetUsedBufferSize(){ return m_nUsedBufferSize; }

	// ���� ���ۻ��� ��ȯ
	int		GetAllUsedBufferSize(){ return m_uiAllUserBufSize; }

	// ���� ���� ������ �о ��ȯ
	char*	GetBuffer( int nReadSize, int *pReadSize );

	void	SetUsedBufferSize( int nUsedBufferSize );

private:

	char*	m_pRingBuffer;			// ���� �����͸� �����ϴ� ���� ������
	char*	m_pBeginMark;			// ������ ó�� �κ��� ����Ű�� �ִ� ������
	char*	m_pEndMark;				// ������ ������ �κ��� ����Ű�� �ִ� ������
	char*	m_pCurrentMark;			// ������ ������� ���� �κ��� ����Ű�� �ִ� ������
	char*	m_pGettedBufferMark;	// ������� �����͸� ���� ���� ������
	char*	m_pLastMoveMark;		// recycle�Ǳ� ���� ������ ������

	int				m_nBufferSize;		// ���� ������ �� ũ��
	int				m_nUsedBufferSize;	// ���� ���� ���� ������ ũ��
	unsigned int	m_uiAllUserBufSize;	// �� ó���� ������ ��


	CMonitor		m_csRingBuffer;			// ����ȭ ��ü

private:

	CRingBuffer( const CRingBuffer &rhs );
	CRingBuffer &operator=( const CRingBuffer &rhs );


};