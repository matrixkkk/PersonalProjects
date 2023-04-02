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

	unsigned long rGetHashCode(const char* pString);	//해쉬코드
	
	//사운드 관련 함수
	void Set3DSettings(float distancefactor, float dopplerscale = 1.f, float rolloffscale = 1.f);
	void SetMinMaxDistance(float fmin, float fmax);
	void SetListener(FMOD_VECTOR* pos, FMOD_VECTOR* dir, FMOD_VECTOR* up);
		
	bool Play2D(unsigned int eSound, FMOD::Channel* &Playing, bool bLoop = false);
	bool Play3D(unsigned int eSound, float x, float y, float z, bool bLoop = false);
	bool Play3D(unsigned int eSound, FMOD_VECTOR* pos, bool bLoop = false);

	bool PlayStream(const char* filename);
	void StopSteam();

	void RegisterSound(unsigned int eSound,NiPoint3 kSpot,NiTransform kTransform,bool b2DSound = false);	//플레이할 사운드 등록
	void PlayQueueData();														//큐에 담긴 사운드 플레이

	FMOD::Sound*	GetSound(const char* filename); 	//개별 사운드 가져오기
		
	static CSoundManager*	GetInstance();						//사운드 관리자 객체 리턴
protected:
	FMOD::System*	m_system;			//사운드 시스템 객체
	float			m_mindistance;		//최소 거리
	float			m_maxdistance;		//최대 거리

	FMOD::Sound*	m_streamsound;
	FMOD::Channel*	m_streamchannel;

	std::map <unsigned long, FMOD::Sound*>		m_SoundContainer;	//사운드 컨테이너
	std::map <unsigned long, FMOD::Channel*>	m_PlayingChannel;	//플레이 중인 채널

	std::vector<SoundData>	m_vecSoundData;						//처리해야할 사운드 큐

	static CSoundManager*	mp_SoundManager;
	
};

CSoundManager* GetSoundMgr();

#endif
