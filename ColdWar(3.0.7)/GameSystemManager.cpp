#include "GameSystemManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "CStateManager.h"
#include "CGamePlayState.h"

//#include "ZFLog.h"

GameSystemManager*		GameSystemManager::ms_gameSystemManager = NULL;


GameSystemManager::GameSystemManager()
{
	m_fCurrTime = 0.0f;
	m_fTotalTime = 0.0f;
	
	m_pkRadar = NULL;

	m_uiTotalCondition = 0;
	m_fPlayAccumTime = CGameApp::mp_Appication->GetAccumTime();
	m_fTimer = 0;

	m_bIsServer = false;

	m_uiArmor = 100;		//기본 아머 100
	
	ms_gameSystemManager = this;

	m_pkDamageEffect  = NULL;	//데미지 이펙트
	m_bDamageEffectOn = false;	
}

GameSystemManager::~GameSystemManager()
{
	m_spText = 0;
	m_spFont = 0;

	m_spOrigin = 0;
	m_spBodyTex = 0;

	m_fLastDamage = 0.0f;

	if( !m_kScoreboard.empty() )
		m_kScoreboard.clear();

	//스크린 텍스쳐들 제거
	m_kScreenTextureArray.RemoveAll();

	if(m_pkRadar)
		NiDelete m_pkRadar;
	m_pkRadar = NULL;

	if(m_pkDamageEffect)
		NiDelete m_pkDamageEffect;
}

GameSystemManager* GameSystemManager::GetInstance()
{
	return ms_gameSystemManager;
}

bool GameSystemManager::Init(DWORD	dwArmy,float fTotalTime)
{	
	//여기서 서버일때랑 클라이언트때랑 처리를 달리 한다.
	CGameServer* pkServer = NetworkManager::GetInstance()->GetServer();
	if(pkServer)
		m_bIsServer = true;
	else
		m_bIsServer = false;

	//게임 시간 설정
	m_fTotalTime = fTotalTime;
	
	if(!CreateScreenTexture())
		return false;

	if(!CreateFont())
		return false;

	//레이더 객체 생성
	if(!CreateRadar())
		return false;

	//데미지 이펙트 생성
	if(!CreateDamageEffect())
		return false;


	StartGame();	//임시 시작

	return true;
}

void GameSystemManager::StartGame()
{
	m_fCurrTime = m_fTotalTime;
}

void GameSystemManager::EndGame()
{
}

void GameSystemManager::ResetPlay()
{
	m_fCurrTime = m_fTotalTime;
}

//킬데스 처리
void GameSystemManager::AddDeath(DWORD id)
{
	BYTE camp = 0;	//총 맞은 캐릭터의 진영

	//죽은 캐릭터 정보 알림
	PlayersMap& conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	conPlayers[id].userInfo.state = DEATH;
	conPlayers[id].userInfo.deathTime = CGameApp::mp_Appication->GetAccumTime();
	camp = conPlayers[id].userInfo.camp;

	NetworkManager::GetInstance()->UnLockPlayers();
	
	//================================================================================
	//   UI 갱신 : 킬뎃 작성 
	//============================================================================
	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	CGamePlayState* pGamePlay = ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"));

	//죽은 사람이 현재 유저일 경우만 리스폰 프로그레시브 컨트롤 실행
	if(NetworkManager::GetInstance()->GetID() == id)
		pGamePlay->SetResponning( true );

	BYTE death = m_kScoreboard[id].death;		//현재 데스를 가져옴
	m_kScoreboard[id].death = ++death;
	pGamePlay->UpdateDeath(id,camp,death);		//UI 업데이트
	
}

void	GameSystemManager::ProcessResultScore()
{

	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	CGamePlayState* pGamePlay = ((CGamePlayState*)rStateManager.GetStateClass("GameResult"));

}

void GameSystemManager::AddKill(DWORD id)
{
	BYTE camp = 0;	//총 맞은 캐릭터의 진영

	//죽은 캐릭터 정보 알림
	PlayersMap& conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	camp = conPlayers[id].userInfo.camp;

	NetworkManager::GetInstance()->UnLockPlayers();
	
	//================================================================================
	//   UI 갱신 : 킬뎃 작성 
	//============================================================================
	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	CGamePlayState* pGamePlay = ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"));
	
	BYTE kill = m_kScoreboard[id].kill;		//현재 데스를 가져옴
	m_kScoreboard[id].kill = ++kill;
	pGamePlay->UpdateKill(id,camp,kill);		//UI 업데이트
}

void GameSystemManager::MeasureTime(float fTime)
{
	float fDelta = fTime - m_fPlayAccumTime;
	//현재 시간은 다운 카운트 한다.
	m_fCurrTime -= fDelta;

	// 시간 초과시 승리 조건에 따라 처리
	if( m_fCurrTime <= 0.0f )
	{
		// 네트워크 모드일때 시간에 따른 승리 여부 체크
		if( WorldManager::GetWorldManager()->IsNetMode() )
			NetworkManager::GetInstance()->GetServer()->CheckWinTeam();

		m_fCurrTime = m_fTotalTime;

		return;
	}
	
	m_fPlayAccumTime = fTime;

	//타이머 시간 증가
	m_fTimer += fDelta;

	//1초가 넘어가면 서버에 메시지를 보낸다.
	if(m_fTimer > 1.0f)
	{
		NetworkManager::GetInstance()->GetServer()->SendTimeToClient(m_fCurrTime);
		m_fTimer = 0.0f;
	}
}

void GameSystemManager::SetGameTime(float fCurrTime)
{
	m_fCurrTime = fCurrTime;
}

void GameSystemManager::DamageByInjury(float fTime)					//부상으로 인한 Tick데미지
{	
	if(fTime - m_fLastDamage > 1.0f) //마지막으로 데미지 피해로 부터 1초가 지난경우
	{
		m_kHealth.chest > 0 ? m_kHealth.chest -= 1 : m_kHealth.chest = 0;
		m_kHealth.head  > 0 ? m_kHealth.head  -= 1 : m_kHealth.head  = 0;
		
		m_fLastDamage = fTime;		//데미지 입음 
	}
}

void GameSystemManager::CalculateDamageByPart(BYTE part,short sDamage)
{
	short sDamageSum;	//데미지 총합, 피해받을 데미지
	float fPartWeight;

	m_uiArmor > 0 ? fPartWeight = 0.5f : fPartWeight = 1.0f;
	
	switch(part)
	{
	case BODY : 
		{
			//아머로 인한 부위별 가중치			
			sDamageSum = (short)(sDamage * fChestAddDamage * fPartWeight); 

			m_kHealth.chest -= sDamageSum;
			if(m_kHealth.chest < 0 ) m_kHealth.chest = 0;
		}
		break;
	case HEAD :
		{
			sDamageSum = (short)(sDamage * fHeadAddDamage * fPartWeight); 
			
			m_kHealth.head -= sDamageSum;
			if(m_kHealth.head < 0 ) m_kHealth.head = 0;
		}
		break;
	case LEFT_ARM :
		m_kHealth.leftArm -= sDamage;
		if(m_kHealth.leftArm < 0 ) m_kHealth.leftArm = 0;
		break;
	case RIGHT_ARM :
		m_kHealth.rightArm -= sDamage;
		if(m_kHealth.rightArm < 0 ) m_kHealth.rightArm = 0;
		break;
	case LEFT_LEG :
		m_kHealth.leftLeg -= sDamage;
		if(m_kHealth.leftLeg < 0 ) m_kHealth.leftLeg = 0;
		break;
	case RIGHT_LEG :
		m_kHealth.rightLeg -= sDamage;
		if(m_kHealth.rightLeg < 0 ) m_kHealth.rightLeg = 0;
		break;	
	}

	//아머 내구력 감소
	m_uiArmor -= sDamage;
}

//데미지에 따른 상태 처리
DWORD GameSystemManager::ProcessCondition(FireInfo* pFireInfo)
{	
	//데미지 이펙트 그리기
	m_bDamageEffectOn = true;

	//데미지 처리
	for(unsigned int i=0; i<pFireInfo->intersectInfoSize;i++)
	{
		short damage = (short)pFireInfo->damage[i];
		
		CalculateDamageByPart( (BYTE)pFireInfo->part[i], (short)pFireInfo->damage[i]);
	}
	
	//머리나 가슴의 체력이 0이면 죽은 것
	if(m_kHealth.chest <= 0  || m_kHealth.head <=0)
		return DEATH;
	else if(m_kHealth.leftArm <=0 && m_kHealth.rightArm <=0
		|| m_kHealth.leftLeg <=0  && m_kHealth.rightLeg <=0 )
	{
		return INCAPACITY;
	}
	else
		return NORMAL;
}


void GameSystemManager::HealthBodyUpdate()
{
	NiSourceTexture* pkMakeTex = NULL;
	NiSourceTexture* pkTexSrc = NULL;

	NiTexture* pkTexture = NULL;
	
	unsigned int i=0;
	//테스트 코드 - 텍스쳐의 픽셀데이터를 직접 수정 가능한지
	for(i=0;i<m_kScreenTextureArray.GetSize();i++)
	{
		pkTexture = m_kScreenTextureArray.GetAt(i)->GetTexture();

		if(pkTexture->GetName() == "body")
		{
			pkTexSrc = NiDynamicCast(NiSourceTexture,pkTexture);
			break;			
		}
	}
	if(pkTexSrc == NULL) return ;
		
	//SourceTexture에서 픽셀 데이터를 가져온다.
	NiPixelData*	  kPixel   = pkTexSrc->GetSourcePixelData();
	
	//부위별로 텍스쳐를 바꾼다.
	ChangeTexture(kPixel,m_kHealthBody.m_kHead,m_kHealth.head);		//머리
	ChangeTexture(kPixel,m_kHealthBody.m_kChest,m_kHealth.chest);	//가슴
	ChangeTexture(kPixel,m_kHealthBody.m_kLeftArm,m_kHealth.leftArm);	//왼팔
	ChangeTexture(kPixel,m_kHealthBody.m_kLeftLeg,m_kHealth.leftLeg);	//왼발
	ChangeTexture(kPixel,m_kHealthBody.m_kRightArm,m_kHealth.rightArm);	//오른팔
	ChangeTexture(kPixel,m_kHealthBody.m_kRightLeg,m_kHealth.rightLeg);	//오른발
	
	kPixel->MarkAsChanged();
	pkMakeTex = NiSourceTexture::Create(kPixel);
	pkMakeTex->SetName("body");
	
	m_kScreenTextureArray.GetAt(i)->SetTexture(pkMakeTex);
}

bool GameSystemManager::CreateRadar()
{
	m_pkRadar = NiNew CRadar;
	assert(m_pkRadar);

	if(!m_pkRadar->CreateRadar())
		return false;

	return true;
}

void GameSystemManager::ChangeTexture(NiPixelData* kPixel,NiRect<unsigned char> kBody, unsigned short kHealth)
{
	//픽셀의 가로,세로 크기를 구한다.
	unsigned int uiWidth = kPixel->GetWidth();
	unsigned int uiHeight= kPixel->GetHeight();

	//한 픽셀에 몇 바이트가 할당되어 있는지 알아온다.
	unsigned int PixelStride = kPixel->GetPixelStride();

	unsigned char* ucPixel = kPixel->GetPixels();
	
	//오리지널 픽셀 데이터
	NiPixelData* pkOriginPixel = m_spOrigin->GetSourcePixelData();
	unsigned char* ucOriginPixel = pkOriginPixel->GetPixels();

	//temp
	NiInputKeyboard* pkKB = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();
	
						
	for(unsigned int i = kBody.m_left;i<kBody.m_right;i++)
	{	
		for(unsigned int j= kBody.m_top;j<kBody.m_bottom;j++)
		{
			int idx = (PixelStride * i) + (uiWidth * j * PixelStride);
			unsigned char* pPixel = &ucPixel[idx];	
			//해당 픽셀의 알파값이 0인 경우는 제외
			if( *(pPixel + 3) != 0)
			{				
				unsigned char* pOriginPixel = &ucOriginPixel[idx];				
				unsigned char ucRed   = *pOriginPixel + (100 - kHealth) * 4;
				if(kHealth > 50)
					*(pPixel) = ucRed; 
				else
				{
					unsigned char ucGreen = *(pOriginPixel+1) - (100 - kHealth);
					if(ucGreen < 0 ) ucGreen = 0;
					*(pPixel+1)= ucGreen;
				}
			}
		}			
	}
}

bool GameSystemManager::CreateFont()
{	
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	unsigned int  uiWidth = pkRenderer->GetDefaultRenderTargetGroup()->GetWidth(0);
	unsigned int  uiHeight = pkRenderer->GetDefaultRenderTargetGroup()->GetHeight(0);


	m_spFont = NiFont::Create(pkRenderer,"Data/UI/모아소나무B20_BA.NFF");
	assert(m_spFont);

	//금색 156 131 41
	//은색 223 223 217
	NiColorA kColor(223.0f / 255.0f , 223.0f / 255.0f ,217.0f /255.0f ,0.5f);
	m_spText = NiNew NiString2D(m_spFont,NiFontString::COLORED,512,NULL,kColor, unsigned int(0.25f * uiWidth),360);
	assert(m_spText);

	m_spText->SetPosition((unsigned int)(0.2f * uiWidth),(unsigned int)(0.8f * uiHeight));
	
	int tmpHeath = 100;
	//체력 표시
	m_spText->sprintf("%d %%",tmpHeath);

	//시간 폰트 위치 설정
	m_kPosTime.x = 0.5f * uiWidth;
	m_kPosTime.y = 0.2f * uiHeight;

	//추가 -- 장탄수 화면에 출력
	m_kPosBulletData.x = 0.8f * uiWidth;	//남은 발수 폰트 위치 --추가
	m_kPosBulletData.y = 0.9f * uiHeight;
	
	return true;
}

void GameSystemManager::DrawUI()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	//스크린 텍스쳐 그리기
	for(unsigned int i=0;i<m_kScreenTextureArray.GetSize();i++)
	{
		NiScreenTexture*	pkTexture = m_kScreenTextureArray.GetAt(i);
		if(pkTexture)
			pkTexture->Draw(pkRenderer);
	}
	//폰트 그리기
	DrawFont(pkRenderer);

	//레이더 그리기
	m_pkRadar->Draw(pkRenderer);

	//데미지 효과 그리기
	if(m_bDamageEffectOn)
	{
		m_pkDamageEffect->Draw(pkRenderer);
		m_bDamageEffectOn = false;
	}
}

void GameSystemManager::Update(float fTime)
{	
	//서버일 경우 직접 계산
	if(m_bIsServer)
		MeasureTime(fTime);

	//헬스바디업데이트
	HealthBodyUpdate();

	//레이더 업데이트
	m_pkRadar->Update();	
}

void GameSystemManager::DrawFont(NiRenderer *pkRenderer)
{
	//------------ 남은 게임 시간 그리기---------
	//현재 시간을 분. 초로 나눈다.
	unsigned short fMinute = ((unsigned short)m_fCurrTime) / 60;
	unsigned short fSecond = ((unsigned short)m_fCurrTime) % 60;

	char times[4];
	times[0] = fMinute/10;
	times[1] = fMinute%10;
	times[2] = fSecond/10;
	times[3] = fSecond%10;
	
	m_spText->SetPosition((UINT)m_kPosTime.x,(UINT)m_kPosTime.y);
	m_spText->sprintf("%d%d:%d%d",times[0],times[1],times[2],times[3]);
	m_spText->Draw(pkRenderer);

	//========추가=======
	//장탄수 화면에 출력
	CUserCharacter* pkUserChar = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter();
	BulletData kBullet = pkUserChar->GetEquipWeapon()->GetBulletData();
	m_spText->SetPosition( (UINT)m_kPosBulletData.x,(UINT)m_kPosBulletData.y);
	m_spText->sprintf("%d / %d",kBullet.remainBullet,kBullet.maxBullet);		//장탄 수 표시
	m_spText->Draw(pkRenderer);
}


bool GameSystemManager::CreateScreenTexture()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	unsigned int  uiWidth = pkRenderer->GetDefaultRenderTargetGroup()->GetWidth(0);
	unsigned int  uiHeight = pkRenderer->GetDefaultRenderTargetGroup()->GetHeight(0);

	unsigned int  uiTexWidth = 0;
	unsigned int  uiTexHeight = 0;

	//체력 마크
	/*NiSourceTexture*	pkTextureHealth = NiSourceTexture::Create("Data/UI/heath.tga");
	if(!pkTextureHealth)
	{
		NiMessageBox("GameSystemManager::CreateScreenTexture() - Not Found heath.tga",NULL);
		return false;
	}
	NiScreenTexture*	pkScrTexHealth = NiNew NiScreenTexture(pkTextureHealth);
	assert(pkScrTexHealth);
	pkScrTexHealth->AddNewScreenRect((unsigned int)(0.8f * uiHeight),(unsigned int)(0.08f * uiWidth),20,21,0,0);
	m_kScreenTextureArray.Add(pkScrTexHealth);
	*/	

	//오리지널 바디
	m_spOrigin = NiSourceTexture::Create("Data/UI/body.TGA");
	assert(m_spOrigin);
	m_spOrigin->LoadPixelDataFromFile();

	//바디
	m_spBodyTex = NiSourceTexture::Create("Data/UI/body.TGA");
	if(!m_spBodyTex)
	{
		NiMessageBox("GameSystemManager::CreateScreenTexture() - Not Found army.tga",NULL);
		return false;
	}
	//픽셀데이터를 로드한다.
	m_spBodyTex->LoadPixelDataFromFile();
	m_spBodyTex->SetName("body");
	
	uiTexWidth  = m_spBodyTex->GetWidth();
	uiTexHeight = m_spBodyTex->GetHeight();
	
	NiScreenTexture*	pkScreenTexture = NiNew NiScreenTexture(m_spBodyTex);
	assert(pkScreenTexture);

	pkScreenTexture->AddNewScreenRect((unsigned int)(0.7f * uiHeight),(unsigned int)(0.8f * uiWidth),uiTexWidth,uiTexHeight,0,0);

	m_kScreenTextureArray.Add(pkScreenTexture);
	
	return true;
}

bool GameSystemManager::CreateDamageEffect()
{
	//색상 설정
	const NiRenderTargetGroup* pkRTGroup = NiRenderer::GetRenderer()->
        GetDefaultRenderTargetGroup();
	
	unsigned int iWidth  = pkRTGroup->GetWidth(0);
	unsigned int iHeight = pkRTGroup->GetHeight(0);

	//크로스헤어 색상 설정
	NiMaterialProperty* pkProp = NiNew NiMaterialProperty;
	assert(pkProp);

	pkProp->SetEmittance(NiColor(1.0f,0.0f,0.0f));
	pkProp->SetAlpha(0.5f);

	m_pkDamageEffect = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,0));
	assert(m_pkDamageEffect);
	m_pkDamageEffect->Insert(4);
	
	m_pkDamageEffect->SetRectangle(0,0.0f,0.0f, 1.0f, 1.0f);
	//pkCenter->SetTranslate(0.5f,0.5f,0.3f);
	m_pkDamageEffect->AttachProperty(pkProp);

	m_pkDamageEffect->UpdateProperties();
	m_pkDamageEffect->Update(0.0f);

	return true;
}

void GameSystemManager::DisplayDamageEffect()
{
	m_bDamageEffectOn = true;	
}


Healths* GameSystemManager::GetHealths()
{
	return &m_kHealth;
}

//체력 회복
void GameSystemManager::RecoveryHealth()
{
	//체력 회복
	m_kHealth.Reset();

	//아머 회복
	m_uiArmor = 100;

	//바디 이미지복구
	m_spBodyTex->DestroyAppPixelData();
	m_spBodyTex->LoadPixelDataFromFile();
	m_kScreenTextureArray.GetAt(0)->SetTexture(m_spBodyTex);
}


