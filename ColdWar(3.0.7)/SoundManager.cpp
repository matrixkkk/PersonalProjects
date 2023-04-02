#include "SoundManager.h"
#include <assert.h>

CSoundManager*	CSoundManager::mp_SoundManager = NULL;

CSoundManager*	CSoundManager::GetInstance()			//���� ������ ��ü ����
{
	return mp_SoundManager;
}

CSoundManager::CSoundManager(FMOD::System* system) : m_system(system), m_streamsound(0), m_streamchannel(0)
{
	assert(!mp_SoundManager);		//�̱������� ����
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
	//1)�ʱ�ȭ 
	FMOD::System* system;
	FMOD_RESULT result = FMOD::System_Create(&system);
	if(result != FMOD_OK) return false;

	//2)����
	// ���� �����ʰ� ���� �� �ִ� ����� ����
	result = system->init(DEF_SOUND_MAXCHANNEL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	if(result != FMOD_OK) return false;

	CSoundManager* soundmgr = new CSoundManager(system);
	
	//distancefactor = scale factor ������ ���⋚ ���, 1m == 1.0f�� �������� �Ѵ�.
	soundmgr->Set3DSettings(distancefactor);
	//���尡 ��Ÿ����� �������� �ɰ��ΰ� ����
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
	 //���ӻ��� 1m�� distancefactor�� ����ȭ ����.
}

bool CSoundManager::LoadSound(const char* filename,unsigned int eSound, eSoundType soundtype /* = DEF_SOUND_TYPE_3D */)
{
	if(GetSound(filename)) return true;	

	FMOD::Sound* sound;

	FMOD_MODE fmode = FMOD_DEFAULT;
	switch(soundtype)
	{
	case DEF_SOUND_TYPE_2D :  //2d�� ����(�������)
		fmode |= FMOD_2D;
		break;
	case DEF_SOUND_TYPE_3D :  //3d�� ����
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
	if(bLoop)//�����ϰ��
	{
		m_system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
		channel->setMode(FMOD_LOOP_NORMAL);
		channel->setPaused(false);
	}
	else//�ѹ��� �÷���
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
{//3d����� ��ġ���� �ʿ�
	FMOD::Sound* sound = m_SoundContainer[ eSound ];	
	if(!sound) return false;

	FMOD::Channel* channel;
	m_system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
	//��ġ������ �ι�° ���ڴ� �ӵ�
	channel->set3DAttributes(pos, 0);
	if(bLoop) channel->setMode(FMOD_LOOP_NORMAL);
	channel->setPaused(false);
	
	//������ ��ġ�ܿ� �������� ��ġ�� �����ؾ߰ٴ�. 
	//SetListener�Լ��� �Ҽ��ִ�.(�Ϲ������� ����� �ִ°��� ������?)

	//update�Լ��� �޼��� �������� �����ϵ�������.
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
	//���Ͱ� ��� ������ ����
	if(m_vecSoundData.empty())
		return;
	
	//���Ϳ� �÷����� ���� �����Ͱ� ������ ��� ������ �÷���
	for(unsigned int i=0;i<m_vecSoundData.size();i++)
	{
		SoundData	sndData = m_vecSoundData[i];
		//2D �������� 3D ���������� ���� �ٸ��� ó��
		if(sndData.b2DSound)
		{
			FMOD::Channel* pChannel;
			Play2D(sndData.eSoundCode,pChannel);
		}
		else	//2D ����� ���
		{
			Play3D(sndData.eSoundCode,(FMOD_VECTOR*)&sndData.kSoundSpot);
			//������ ��ġ ����
			SetListener( (FMOD_VECTOR*)&sndData.kListenPos,(FMOD_VECTOR*)&sndData.kDir,(FMOD_VECTOR*)&sndData.kUp);
		}
	}
	//��� �÷��� �����Ƿ� ���� Ŭ����
	m_vecSoundData.clear();
}

void CSoundManager::RegisterSound(unsigned int eSound, NiPoint3 kSpot,NiTransform kTransform,bool b2DSound)
{
	//NiTransform���� Pos,Rot ������ �����´�.
	NiPoint3 kDir,kUp;
	NiPoint3	kPos = kTransform.m_Translate;
	NiMatrix3	kRot = kTransform.m_Rotate;
	kRot.GetCol(0,kDir);
	kRot.GetCol(1,kUp);

	//ť�� �߻��� 3D���� ����
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

