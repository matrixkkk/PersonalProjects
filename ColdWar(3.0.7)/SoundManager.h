#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include <map>
#include <vector>
#include <NiTransform.h>
#include "fmod.hpp"
#include "protocol.h"

#define DEF_SOUND_MAXCHANNEL		64


class CSoundManager
{
public:
	CSoundManager(FMOD::System* system);
	~CSoundManager(void);
	
	enum eSoundType
	{
		DEF_SOUND_TYPE_2D,
		DEF_SOUND_TYPE_3D,
	};
	
	void Release();

	static bool CreateSoundMgr(float distancefactor, float mindistance, float maxdistance);
	static void DestroySoundMgr();

	void Update();
	bool LoadSound(const char* filename,unsigned int eSound, eSoundType soundtype = DEF_SOUND_TYPE_3D);
	void StopSound(const char* filename);

	unsigned long rGetHashCode(const char* pString);	//�ؽ��ڵ�
	
	//���� ���� �Լ�
	void Set3DSettings(float distancefactor, float dopplerscale = 1.f, float rolloffscale = 1.f);
	void SetMinMaxDistance(float fmin, float fmax);
	void SetListener(FMOD_VECTOR* pos, FMOD_VECTOR* dir, FMOD_VECTOR* up);
		
	bool Play2D(unsigned int eSound, FMOD::Channel* &Playing, bool bLoop = false);
	bool Play3D(unsigned int eSound, float x, float y, float z, bool bLoop = false);
	bool Play3D(unsigned int eSound, FMOD_VECTOR* pos, bool bLoop = false);

	bool PlayStream(const char* filename);
	void StopSteam();

	void RegisterSound(unsigned int eSound,NiPoint3 kSpot,NiTransform kTransform,bool b2DSound = false);	//�÷����� ���� ���
	void PlayQueueData();														//ť�� ��� ���� �÷���

	FMOD::Sound*	GetSound(const char* filename); 	//���� ���� ��������
		
	static CSoundManager*	GetInstance();						//���� ������ ��ü ����
protected:
	FMOD::System*	m_system;			//���� �ý��� ��ü
	float			m_mindistance;		//�ּ� �Ÿ�
	float			m_maxdistance;		//�ִ� �Ÿ�

	FMOD::Sound*	m_streamsound;
	FMOD::Channel*	m_streamchannel;

	std::map <unsigned long, FMOD::Sound*>		m_SoundContainer;	//���� �����̳�
	std::map <unsigned long, FMOD::Channel*>	m_PlayingChannel;	//�÷��� ���� ä��

	std::vector<SoundData>	m_vecSoundData;						//ó���ؾ��� ���� ť

	static CSoundManager*	mp_SoundManager;
	
};

CSoundManager* GetSoundMgr();

#endif
