#include "SoundManager.h"
#include <assert.h>

CSoundManager*	CSoundManager::mp_SoundManager = NULL;

CSoundManager*	CSoundManager::GetInstance()			//사운드 관리자 객체 리턴
{
	return mp_SoundManager;
}

CSoundManager::CSoundManager(FMOD::System* system) : m_system(system), m_streamsound(0), m_streamchannel(0)
{
	assert(!mp_SoundManager);		//싱글톤으로 생성
	//mp_SoundManager = this;	
}

CSoundManager::~CSoundManager(void)
{
	assert(mp_SoundManager);

	Release();
	mp_SoundManager = 0;
}

bool CSoundManager::CreateSoundMgr(float distancefactor, float mindistance, float maxdistance)
{
	assert(!mp_SoundManager);
	//1)초기화 
	FMOD::System* system;
	FMOD_RESULT result = FMOD::System_Create(&system);
	if(result != FMOD_OK) return false;

	//2)설정
	// 실제 리스너가 들을 수 있는 오디오 갯수
	result = system->init(DEF_SOUND_MAXCHANNEL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	if(result != FMOD_OK) return false;

	CSoundManager* soundmgr = new CSoundManager(system);
	
	//distancefactor = scale factor 단위를 맞출떄 사용, 1m == 1.0f를 기준으로 한다.
	soundmgr->Set3DSettings(distancefactor);
	//사운드가 어떤거리부터 가감속이 될것인가 지정
	soundmgr->SetMinMaxDistance(mindistance, maxdistance);

	mp_SoundManager = soundmgr;

	return true;
}

void CSoundManager::DestroySoundMgr()
{
	delete mp_SoundManager;
}

void CSoundManager::Release()
{
	std::map <unsigned long, FMOD::Sound*>::iterator it;
	for(it = m_SoundContainer.begin(); it != m_SoundContainer.end(); it++)
	{
		FMOD::Sound* sound = it->second;
		if(sound) sound->release();
	}
	m_SoundContainer.clear();
	
	if(m_streamsound)
	{
		m_streamsound->release();
		m_streamsound = 0;
	}
	m_streamchannel = 0;

	if(m_system)
	{
		m_system->close();
		m_system->release();

		m_system = 0;
	}
}

void CSoundManager::SetMinMaxDistance(float fmin, float fmax)
{
	m_mindistance = fmin;
	m_maxdistance = fmax;
}

void CSoundManager::Set3DSettings(float distancefactor, float dopplerscale, float rolloffscale)
{
	// Set the distance units.
    m_system->set3DSettings(dopplerscale, distancefactor, rolloffscale);
	 //게임상의 1m와 distancefactor를 동기화 하자.
}

bool CSoundManager::LoadSound(const char* filename,unsigned int eSound, eSoundType soundtype /* = DEF_SOUND_TYPE_3D */)
{
	if(GetSound(filename)) return true;	

	FMOD::Sound* sound;

	FMOD_MODE fmode = FMOD_DEFAULT;
	switch(soundtype)
	{
	case DEF_SOUND_TYPE_2D :  //2d용 사운드(배경음등)
		fmode |= FMOD_2D;
		break;
	case DEF_SOUND_TYPE_3D :  //3d용 사운드
		fmode |= FMOD_SOFTWARE | FMOD_3D;
		break;
	}
	FMOD_RESULT result = m_system->createSound(filename, fmode, 0, &sound);
	if (result != FMOD_OK) return false;

	sound->set3DMinMaxDistance(m_mindistance, m_maxdistance);

	unsigned long hashcode = rGetHashCode(filename);
	m_SoundContainer[ eSound ] = sound;
	return true;
}

bool CSoundManager::Play2D(unsigned int eSound,FMOD::Channel* &Playing, bool bLoop)
{
	//FMOD::Sound* sound = GetSound(filename);
	FMOD::Sound* sound = m_SoundContainer[ eSound ];
	if(!sound) return false;

	FMOD::Channel* channel;
	if(bLoop)//루프일경우
	{
		m_system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
		channel->setMode(FMOD_LOOP_NORMAL);
		channel->setPaused(false);
	}
	else//한번만 플레이
	{
		m_system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
	}

	Playing = channel;

	return true;
}

bool CSoundManager::Play3D(unsigned int eSound, float x, float y, float z, bool bLoop)
{
	FMOD_VECTOR pos;
	pos.x = x; pos.y = y; pos.z = z;
	return Play3D(eSound, &pos, bLoop);
}

bool CSoundManager::Play3D(unsigned int eSound, FMOD_VECTOR* pos, bool bLoop)
{//3d사운드는 위치값이 필요
	FMOD::Sound* sound = m_SoundContainer[ eSound ];	
	if(!sound) return false;

	FMOD::Channel* channel;
	m_system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
	//위치값전달 두번째 인자는 속도
	channel->set3DAttributes(pos, 0);
	if(bLoop) channel->setMode(FMOD_LOOP_NORMAL);
	channel->setPaused(false);
	
	//음악의 위치외에 리스너의 위치를 설정해야겟다. 
	//SetListener함수로 할수있다.(일반적으로 뷰행렬 넣는것이 낫겟지?)

	//update함수는 메세지 루프에서 수행하도록하자.
	return true;
}

bool CSoundManager::PlayStream(const char* filename)
{
	if(m_streamchannel) 
	{
		StopSteam();
	}

	FMOD_RESULT result = m_system->createStream(filename, FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_streamsound);
	if (result != FMOD_OK) return false;

	m_system->playSound(FMOD_CHANNEL_FREE, m_streamsound, false, &m_streamchannel);
	//m_streamchannel->setPriority(0);
	return true;
}

void CSoundManager::StopSteam()
{
	if(m_streamchannel)
	{
		m_streamchannel->stop();
		m_streamchannel = 0;
	}

	if(m_streamsound)
	{
		m_streamsound->release();
		m_streamsound = 0;
	}
}

void CSoundManager::StopSound(const char* filename)
{
	unsigned long hashcode = rGetHashCode(filename);

	std::map<unsigned long,FMOD::Sound*>::iterator iter = m_SoundContainer.find(hashcode);
	if(iter != m_SoundContainer.end())
	{
		iter->second->release();
	}
}

FMOD::Sound* CSoundManager::GetSound(const char* filename)
{
	unsigned long hashcode = rGetHashCode(filename);
	std::map <unsigned long, FMOD::Sound*>::iterator it = m_SoundContainer.find(hashcode);
	if(it != m_SoundContainer.end())
	{
		return it->second;
	}
	return 0;
}

void CSoundManager::SetListener(FMOD_VECTOR* pos, FMOD_VECTOR* dir, FMOD_VECTOR* up)
{
	m_system->set3DListenerAttributes(
			0, 
			pos, 
			0, 
			dir, 
			up);
}

unsigned long CSoundManager::rGetHashCode(const char* pString)
{
   unsigned long hash = 5381;
   int len = strlen(pString);
   for(int i = 0; i < len; ++i)
   {
      hash = ((hash << 5) + hash) + (unsigned long)pString[i]; // hash * 33 + ch
   }   
   return hash;
}

void CSoundManager::PlayQueueData()
{	
	//벡터가 비어 있으면 리턴
	if(m_vecSoundData.empty())
		return;
	
	//벡터에 플레이할 사운드 데이터가 있으면 모두 꺼내서 플레이
	for(unsigned int i=0;i<m_vecSoundData.size();i++)
	{
		SoundData	sndData = m_vecSoundData[i];
		//2D 사운드인지 3D 사운드인지에 따라 다르게 처리
		if(sndData.b2DSound)
		{
			FMOD::Channel* pChannel;
			Play2D(sndData.eSoundCode,pChannel);
		}
		else	//2D 사운드로 재생
		{
			Play3D(sndData.eSoundCode,(FMOD_VECTOR*)&sndData.kSoundSpot);
			//리스너 위치 설정
			SetListener( (FMOD_VECTOR*)&sndData.kListenPos,(FMOD_VECTOR*)&sndData.kDir,(FMOD_VECTOR*)&sndData.kUp);
		}
	}
	//모두 플레이 했으므로 벡터 클리어
	m_vecSoundData.clear();
}

void CSoundManager::RegisterSound(unsigned int eSound, NiPoint3 kSpot,NiTransform kTransform,bool b2DSound)
{
	//NiTransform에서 Pos,Rot 정보를 가져온다.
	NiPoint3 kDir,kUp;
	NiPoint3	kPos = kTransform.m_Translate;
	NiMatrix3	kRot = kTransform.m_Rotate;
	kRot.GetCol(0,kDir);
	kRot.GetCol(1,kUp);

	//큐에 발생된 3D사운드 저장
	SoundData	kSoundData;

	kSoundData.eSoundCode	= eSound;
	kSoundData.kListenPos	= kPos;
	kSoundData.kSoundSpot	= kSpot;
	kSoundData.kDir			= kDir;
	kSoundData.kUp			= kUp;
	kSoundData.b2DSound		= b2DSound;

	m_vecSoundData.push_back(kSoundData);
}

void CSoundManager::Update()
{
	PlayQueueData();

	m_system->update();
}

