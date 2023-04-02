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

	m_uiArmor = 100;		//�⺻ �Ƹ� 100
	
	ms_gameSystemManager = this;

	m_pkDamageEffect  = NULL;	//������ ����Ʈ
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

	//��ũ�� �ؽ��ĵ� ����
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
	//���⼭ �����϶��� Ŭ���̾�Ʈ���� ó���� �޸� �Ѵ�.
	CGameServer* pkServer = NetworkManager::GetInstance()->GetServer();
	if(pkServer)
		m_bIsServer = true;
	else
		m_bIsServer = false;

	//���� �ð� ����
	m_fTotalTime = fTotalTime;
	
	if(!CreateScreenTexture())
		return false;

	if(!CreateFont())
		return false;

	//���̴� ��ü ����
	if(!CreateRadar())
		return false;

	//������ ����Ʈ ����
	if(!CreateDamageEffect())
		return false;


	StartGame();	//�ӽ� ����

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

//ų���� ó��
void GameSystemManager::AddDeath(DWORD id)
{
	BYTE camp = 0;	//�� ���� ĳ������ ����

	//���� ĳ���� ���� �˸�
	PlayersMap& conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	conPlayers[id].userInfo.state = DEATH;
	conPlayers[id].userInfo.deathTime = CGameApp::mp_Appication->GetAccumTime();
	camp = conPlayers[id].userInfo.camp;

	NetworkManager::GetInstance()->UnLockPlayers();
	
	//================================================================================
	//   UI ���� : ų�� �ۼ� 
	//============================================================================
	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	CGamePlayState* pGamePlay = ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"));

	//���� ����� ���� ������ ��츸 ������ ���α׷��ú� ��Ʈ�� ����
	if(NetworkManager::GetInstance()->GetID() == id)
		pGamePlay->SetResponning( true );

	BYTE death = m_kScoreboard[id].death;		//���� ������ ������
	m_kScoreboard[id].death = ++death;
	pGamePlay->UpdateDeath(id,camp,death);		//UI ������Ʈ
	
}

void	GameSystemManager::ProcessResultScore()
{

	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	CGamePlayState* pGamePlay = ((CGamePlayState*)rStateManager.GetStateClass("GameResult"));

}

void GameSystemManager::AddKill(DWORD id)
{
	BYTE camp = 0;	//�� ���� ĳ������ ����

	//���� ĳ���� ���� �˸�
	PlayersMap& conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	camp = conPlayers[id].userInfo.camp;

	NetworkManager::GetInstance()->UnLockPlayers();
	
	//================================================================================
	//   UI ���� : ų�� �ۼ� 
	//============================================================================
	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	CGamePlayState* pGamePlay = ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"));
	
	BYTE kill = m_kScoreboard[id].kill;		//���� ������ ������
	m_kScoreboard[id].kill = ++kill;
	pGamePlay->UpdateKill(id,camp,kill);		//UI ������Ʈ
}

void GameSystemManager::MeasureTime(float fTime)
{
	float fDelta = fTime - m_fPlayAccumTime;
	//���� �ð��� �ٿ� ī��Ʈ �Ѵ�.
	m_fCurrTime -= fDelta;

	// �ð� �ʰ��� �¸� ���ǿ� ���� ó��
	if( m_fCurrTime <= 0.0f )
	{
		// ��Ʈ��ũ ����϶� �ð��� ���� �¸� ���� üũ
		if( WorldManager::GetWorldManager()->IsNetMode() )
			NetworkManager::GetInstance()->GetServer()->CheckWinTeam();

		m_fCurrTime = m_fTotalTime;

		return;
	}
	
	m_fPlayAccumTime = fTime;

	//Ÿ�̸� �ð� ����
	m_fTimer += fDelta;

	//1�ʰ� �Ѿ�� ������ �޽����� ������.
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

void GameSystemManager::DamageByInjury(float fTime)					//�λ����� ���� Tick������
{	
	if(fTime - m_fLastDamage > 1.0f) //���������� ������ ���ط� ���� 1�ʰ� �������
	{
		m_kHealth.chest > 0 ? m_kHealth.chest -= 1 : m_kHealth.chest = 0;
		m_kHealth.head  > 0 ? m_kHealth.head  -= 1 : m_kHealth.head  = 0;
		
		m_fLastDamage = fTime;		//������ ���� 
	}
}

void GameSystemManager::CalculateDamageByPart(BYTE part,short sDamage)
{
	short sDamageSum;	//������ ����, ���ع��� ������
	float fPartWeight;

	m_uiArmor > 0 ? fPartWeight = 0.5f : fPartWeight = 1.0f;
	
	switch(part)
	{
	case BODY : 
		{
			//�Ƹӷ� ���� ������ ����ġ			
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

	//�Ƹ� ������ ����
	m_uiArmor -= sDamage;
}

//�������� ���� ���� ó��
DWORD GameSystemManager::ProcessCondition(FireInfo* pFireInfo)
{	
	//������ ����Ʈ �׸���
	m_bDamageEffectOn = true;

	//������ ó��
	for(unsigned int i=0; i<pFireInfo->intersectInfoSize;i++)
	{
		short damage = (short)pFireInfo->damage[i];
		
		CalculateDamageByPart( (BYTE)pFireInfo->part[i], (short)pFireInfo->damage[i]);
	}
	
	//�Ӹ��� ������ ü���� 0�̸� ���� ��
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
	//�׽�Ʈ �ڵ� - �ؽ����� �ȼ������͸� ���� ���� ��������
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
		
	//SourceTexture���� �ȼ� �����͸� �����´�.
	NiPixelData*	  kPixel   = pkTexSrc->GetSourcePixelData();
	
	//�������� �ؽ��ĸ� �ٲ۴�.
	ChangeTexture(kPixel,m_kHealthBody.m_kHead,m_kHealth.head);		//�Ӹ�
	ChangeTexture(kPixel,m_kHealthBody.m_kChest,m_kHealth.chest);	//����
	ChangeTexture(kPixel,m_kHealthBody.m_kLeftArm,m_kHealth.leftArm);	//����
	ChangeTexture(kPixel,m_kHealthBody.m_kLeftLeg,m_kHealth.leftLeg);	//�޹�
	ChangeTexture(kPixel,m_kHealthBody.m_kRightArm,m_kHealth.rightArm);	//������
	ChangeTexture(kPixel,m_kHealthBody.m_kRightLeg,m_kHealth.rightLeg);	//������
	
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
	//�ȼ��� ����,���� ũ�⸦ ���Ѵ�.
	unsigned int uiWidth = kPixel->GetWidth();
	unsigned int uiHeight= kPixel->GetHeight();

	//�� �ȼ��� �� ����Ʈ�� �Ҵ�Ǿ� �ִ��� �˾ƿ´�.
	unsigned int PixelStride = kPixel->GetPixelStride();

	unsigned char* ucPixel = kPixel->GetPixels();
	
	//�������� �ȼ� ������
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
			//�ش� �ȼ��� ���İ��� 0�� ���� ����
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


	m_spFont = NiFont::Create(pkRenderer,"Data/UI/��Ƽҳ���B20_BA.NFF");
	assert(m_spFont);

	//�ݻ� 156 131 41
	//���� 223 223 217
	NiColorA kColor(223.0f / 255.0f , 223.0f / 255.0f ,217.0f /255.0f ,0.5f);
	m_spText = NiNew NiString2D(m_spFont,NiFontString::COLORED,512,NULL,kColor, unsigned int(0.25f * uiWidth),360);
	assert(m_spText);

	m_spText->SetPosition((unsigned int)(0.2f * uiWidth),(unsigned int)(0.8f * uiHeight));
	
	int tmpHeath = 100;
	//ü�� ǥ��
	m_spText->sprintf("%d %%",tmpHeath);

	//�ð� ��Ʈ ��ġ ����
	m_kPosTime.x = 0.5f * uiWidth;
	m_kPosTime.y = 0.2f * uiHeight;

	//�߰� -- ��ź�� ȭ�鿡 ���
	m_kPosBulletData.x = 0.8f * uiWidth;	//���� �߼� ��Ʈ ��ġ --�߰�
	m_kPosBulletData.y = 0.9f * uiHeight;
	
	return true;
}

void GameSystemManager::DrawUI()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	//��ũ�� �ؽ��� �׸���
	for(unsigned int i=0;i<m_kScreenTextureArray.GetSize();i++)
	{
		NiScreenTexture*	pkTexture = m_kScreenTextureArray.GetAt(i);
		if(pkTexture)
			pkTexture->Draw(pkRenderer);
	}
	//��Ʈ �׸���
	DrawFont(pkRenderer);

	//���̴� �׸���
	m_pkRadar->Draw(pkRenderer);

	//������ ȿ�� �׸���
	if(m_bDamageEffectOn)
	{
		m_pkDamageEffect->Draw(pkRenderer);
		m_bDamageEffectOn = false;
	}
}

void GameSystemManager::Update(float fTime)
{	
	//������ ��� ���� ���
	if(m_bIsServer)
		MeasureTime(fTime);

	//�ｺ�ٵ������Ʈ
	HealthBodyUpdate();

	//���̴� ������Ʈ
	m_pkRadar->Update();	
}

void GameSystemManager::DrawFont(NiRenderer *pkRenderer)
{
	//------------ ���� ���� �ð� �׸���---------
	//���� �ð��� ��. �ʷ� ������.
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

	//========�߰�=======
	//��ź�� ȭ�鿡 ���
	CUserCharacter* pkUserChar = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter();
	BulletData kBullet = pkUserChar->GetEquipWeapon()->GetBulletData();
	m_spText->SetPosition( (UINT)m_kPosBulletData.x,(UINT)m_kPosBulletData.y);
	m_spText->sprintf("%d / %d",kBullet.remainBullet,kBullet.maxBullet);		//��ź �� ǥ��
	m_spText->Draw(pkRenderer);
}


bool GameSystemManager::CreateScreenTexture()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	unsigned int  uiWidth = pkRenderer->GetDefaultRenderTargetGroup()->GetWidth(0);
	unsigned int  uiHeight = pkRenderer->GetDefaultRenderTargetGroup()->GetHeight(0);

	unsigned int  uiTexWidth = 0;
	unsigned int  uiTexHeight = 0;

	//ü�� ��ũ
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

	//�������� �ٵ�
	m_spOrigin = NiSourceTexture::Create("Data/UI/body.TGA");
	assert(m_spOrigin);
	m_spOrigin->LoadPixelDataFromFile();

	//�ٵ�
	m_spBodyTex = NiSourceTexture::Create("Data/UI/body.TGA");
	if(!m_spBodyTex)
	{
		NiMessageBox("GameSystemManager::CreateScreenTexture() - Not Found army.tga",NULL);
		return false;
	}
	//�ȼ������͸� �ε��Ѵ�.
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
	//���� ����
	const NiRenderTargetGroup* pkRTGroup = NiRenderer::GetRenderer()->
        GetDefaultRenderTargetGroup();
	
	unsigned int iWidth  = pkRTGroup->GetWidth(0);
	unsigned int iHeight = pkRTGroup->GetHeight(0);

	//ũ�ν���� ���� ����
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

//ü�� ȸ��
void GameSystemManager::RecoveryHealth()
{
	//ü�� ȸ��
	m_kHealth.Reset();

	//�Ƹ� ȸ��
	m_uiArmor = 100;

	//�ٵ� �̹�������
	m_spBodyTex->DestroyAppPixelData();
	m_spBodyTex->LoadPixelDataFromFile();
	m_kScreenTextureArray.GetAt(0)->SetTexture(m_spBodyTex);
}


