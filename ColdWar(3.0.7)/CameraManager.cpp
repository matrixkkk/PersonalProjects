#include "CameraManager.h"
#include "WorldManager.h"

CameraManager* CameraManager::m_gCamera = NULL;

CameraManager::CameraManager()
{
	m_bZoomOn = false;
	m_bDuck	  = false;
	m_bFirstPerson = false;
	
	m_fLastUpdateTime = 0;

	m_gCamera = this;			//ī�޶� ��������� ���� �������� static ������ ����

	//ī�޶� ����
	m_CameraData.spCamera = NiNew NiCamera;
	
	//ī�޶��� near�� far ����
	m_CameraData.spCamera->SetMinNearPlaneDist(0.001f);
	m_CameraData.spCamera->SetMaxFarNearRatio(1000000.0f);
	
	m_CameraData.spOriendNode = NiNew NiNode;

	m_fDistChange = -1.0f;
}

CameraManager::~CameraManager()
{
	m_CameraData.spOriendNode = NULL;
	m_CameraData.spCamera = NULL;
}

bool CameraManager::Initialize(NiNode *pkCharacterAttach)
{	
	if(!pkCharacterAttach || !m_CameraData.spCamera || !m_CameraData.spOriendNode)
	{
		NiMessageBox("CameraController:: Initialize Failue\n",NULL);
		return false;
	}
	assert(pkCharacterAttach);
	m_spCharacter = pkCharacterAttach;

	//���ڷ� �Ѿ�� ĳ���� ��忡 Orient�� ���δ�.
	pkCharacterAttach->AttachChild(m_CameraData.spOriendNode);

	//Orient��忡 ī�޶� ���δ�.
	m_CameraData.spOriendNode->AttachChild(m_CameraData.spCamera);
 	
	NiMatrix3 kRotX;
    kRotX.MakeXRotation(-NI_HALF_PI);
    m_CameraData.spOriendNode->SetRotate(kRotX);
    pkCharacterAttach->Update(0.0f);
	
	CameraReset();

	return true;
}

void CameraManager::SetFrustumFarDistance( float fDel )
{
	NiFrustum kFrustum = m_CameraData.spCamera->GetViewFrustum();

	kFrustum.m_fFar = fDel;

	m_CameraData.spCamera->SetViewFrustum( kFrustum );
	m_CameraData.spCamera->Update( 0.0f );
}


void CameraManager::BeginCamera()
{
	//������ ������
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	pkRenderer->BeginUsingDefaultRenderTargetGroup(NiRenderer::CLEAR_ALL);

	//���⼭�� "�극��ũ ������"�� ����Ѵ�.
	//�ϳ��� �н����� �ϳ��� �� �׷����� �����ϱ� ���ٴ� �� ����� �����ӹ��ۿ� ���� ����
	//�ȿ� ����� ȥ���ϸ鼭 �̱� �����ӿ��� �ټ��� �� �׷����� �����ϴ� �ɷ�
	//�� ���ø����̼��� ����Ѵ�.

	NiCamera* pkCamera = m_CameraData.spCamera;
	//�������� ī�޶� ��Ʈ�� �⺻���� ���Ѵ�.
	pkRenderer->SetCameraData(pkCamera);
}

void CameraManager::EndCamera()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	//�������� �ݴ´�.
	pkRenderer->EndUsingRenderTargetGroup();
}

//���� �ݵ��� ���� ī�޶��� Pitch
void CameraManager::UpwardCameraFramePitch(int iY)
{
	m_CameraData.fCamPitch -= NI_PI * 0.375f * (float)(iY) / (float)CGameApp::mp_Appication->GetAppHeight();
}

void CameraManager::CameraReset()
{
	//ĳ���Ϳ� ī�޶� ������ ����Ʈ �Ÿ�
	m_CameraData.fCamDist = 4.0f;
	
	//ĳ���Ϳ� �ٰ��� ī�޶��� ����
	//FPS���� ���� �ֻ��� ī�޶� ���̴� ĳ������ ���̴�.
	m_CameraData.fCamHeight = 1.65f;		//ĳ���� Ŭ������ �����Ƿ� �ӽ�
	
	m_CameraData.fCamPitch = 0.0f;

	//�� ����Ʈ : 1x
	m_CameraData.fZoomLevel = 1.0f;

	//���� �ӵ� : �� 1�� 
	m_CameraData.fZoomSpeed = 4.0f;

	//���� �ּ��� �ִ� : 1x ~ 4x
	m_CameraData.fMinZoomLevel = 1.0f;
	m_CameraData.fMaxZoomLevel = 4.0f;

	// The initial heading is specified by the soldier class, which parses it
    // from the data file.   ĳ���Ͱ� �����Ƿ� �ӽ�ó��
	m_CameraData.fHeading = 4.72f;

	m_CameraData.fFrameYaw   = 0.0f;
	m_CameraData.fFramePitch = 0.0f;
	m_CameraData.fLastWorldHeight = 0.0f;
}

void CameraManager::UpdateCamera(float fTime)
{		
	//���콺 look ������Ʈ �ڵ�
	NiInputMouse* pkMouse = CGameApp::mp_Appication->GetInputSystem()->GetMouse();

	if(!pkMouse)
		return;

	//������ �ð����� �帥 �ð� ���
	float fDelta = fTime - m_fLastUpdateTime;

	int iX,iY,iZ;   //x : ���콺�� ��ǥ x��, y : ���콺�� ��ǥ y��, z : ���콺 �� ��ǥ
	
	if(pkMouse->GetPositionDelta(iX,iY,iZ))
	{		
		//���콺�� ���� ������� ĳ������ heading�� ��ȭ�� ����Ѵ�.
		//
		unsigned int uiAppWidth = CGameApp::mp_Appication->GetAppWidth();

		if(uiAppWidth > 0)
		{			
			float fHeadingDelta = NI_PI * 0.5f * (float)(iX) / (float)uiAppWidth;
			m_CameraData.fHeading += fHeadingDelta;

			//
			if(fDelta > 0.005f)
				m_CameraData.fFrameYaw = -fHeadingDelta / (NI_PI * fDelta);
			else
				m_CameraData.fFrameYaw = 0.0f;
		}

		unsigned int uiAppHeight = CGameApp::mp_Appication->GetAppHeight();
		if(uiAppHeight > 0)
		{
			//���콺 ���� ������� Pitch��ȭ ���
			float fPitchDelta = NI_PI * 0.375f * (float)(iY) / (float)uiAppHeight;
			m_CameraData.fCamPitch += fPitchDelta;
			
			if(fDelta > 0.005f)
				m_CameraData.fFramePitch = -fPitchDelta / (NI_PI * fDelta);
			else
				m_CameraData.fFramePitch = 0.0f;
		}

//ī�޶��� ��ġ�� �����ϴ� ��Ű ��뿡 ���� �κ�( ĳ������ ī�޶󿡴� ������)
//#ifdef MODE_TEST   
		//float fDistChange = -iZ / 400.0f;		//120.0f�� ī�޶� ��/�ƿ��� �̵��Ÿ� ũ��

        const float fMinCamDist = 1.00f;			// 3��Ī �������� ���� ���� �ּ� �Ÿ�
        const float fMaxCamDist = 5.0f;		// 3��Ī �������� �� �ƿ��� �ִ� �Ÿ�
        const float fFirstPersonDist = -0.1f;	// 1��Ī ������ ���� ī�޶��� ĳ���� ������ �Ÿ�

        if ((m_CameraData.fCamDist >= fMinCamDist) 
            && ((m_CameraData.fCamDist + m_fDistChange) < fMinCamDist))
        {
            //���� ���� ī�޶��� ��ȭ�� 3��Ī ���� �ּ� �Ÿ����� 
			//������ �Ǹ� 1��Ī �������� ��ġ ����
            m_CameraData.fCamDist = fFirstPersonDist;  

			//���� �÷��� ON
			m_bFirstPerson = true;
        }
        else if (m_CameraData.fCamDist < fMinCamDist) 
        {
            //���� �츮�� 1��Ī ���� ī�޶��̰� ī�޶� �ڷ� �� �ƿ��ϸ�
			//3��Ī ī�޶� �ּ� �Ÿ��� ���´�.
            if (m_fDistChange > 0)
                m_CameraData.fCamDist = fMinCamDist;
        }
        else
        {
			//���� �÷��� OFF
			m_bFirstPerson = false;

            //3��Ī ī�޶��� ���� ó��
            m_CameraData.fCamDist += m_fDistChange;

			//�ִ� �� �ƿ��� �Ÿ��� ����
            if (m_CameraData.fCamDist > fMaxCamDist)
                m_CameraData.fCamDist = fMaxCamDist;
        }		
//#endif
	}

	float fPitchMax = NI_PI * 0.30f;		//pitch�� ���� (���� ��� ���̴� ����)
	
	//Upward-looking�� ���� pitch ������ ����Ѵ�.
	//Upward-looking�� ī�޶� �ܰŸ��� ���� ���������� �����ȴ�.
	//ĳ���Ϳ� ī�޶��� �Ÿ��� 1e-5f(1�� ������ ��) ���� �۴ٸ�(1��Ī ����) Pitch�� min�� -Pitch max�̴�.
	float fPitchMin =(m_CameraData.fCamDist > 1e-5f) ? 
        -NiASin(m_CameraData.fCamHeight 
        / (m_CameraData.fCamHeight + m_CameraData.fCamDist)) :
        -fPitchMax;

	//min max�� ���� pitch���� ����
	if (m_CameraData.fCamPitch < fPitchMin)
        m_CameraData.fCamPitch = fPitchMin;
    else if (m_CameraData.fCamPitch > fPitchMax)
        m_CameraData.fCamPitch = fPitchMax;

	
	UpdateCameraTransform();

	//�ܿ� ���� ����
	if(pkMouse->ButtonWasPressed(NiInputMouse::NIM_RIGHT))
	{
		if(m_bZoomOn)  //�� ������ ���
		{
			m_bZoomOn = false;		//�� X
			m_CameraData.fZoomLevel = m_CameraData.fMinZoomLevel;
		}
		else			//�� ���°� �ƴҰ��
		{
			m_bZoomOn = true;
			m_CameraData.fZoomLevel = m_CameraData.fMaxZoomLevel;
		}
	}

	//�� ������ ���� frustrum ���
	float fInvZoom = 1.0f / m_CameraData.fZoomLevel;
	const NiRenderTargetGroup* pkRTGroup = 
		NiRenderer::GetRenderer()->GetDefaultRenderTargetGroup();
	//��Ⱦ��
	float fAspect = (float)pkRTGroup->GetHeight(0) /
		(float)pkRTGroup->GetWidth(0);
	//frustum ����
	NiFrustum kFrustum(-1.0f * fInvZoom, 1.0f * fInvZoom,
		fAspect * fInvZoom, -fAspect * fInvZoom,0.1f, 130.0f);
	m_CameraData.spCamera->SetViewFrustum(kFrustum);
	m_CameraData.spCamera->Update(fTime);	

		
	m_fLastUpdateTime = fTime;
	
}

void CameraManager::UpdateCameraTransform()
{			
	NiMatrix3 mtRotZCam;		//ī�޶� ȸ��

	mtRotZCam.MakeZRotation(m_CameraData.fCamPitch);		//ī�޶� z���� �߽����� ȸ��
	
	m_CameraData.spCamera->SetRotate(mtRotZCam);

	//�Ӹ��� ��带 �����´�.
	NiNode* pkHead = (NiNode*) m_spCharacter->GetObjectByName("Bip01 Head");
	assert(pkHead);

	//Head�� world �̵������ �����´�.
	NiPoint3 kHeadWorld = pkHead->GetWorldTranslate();

	//ī�޶� ���� = ������ - ĳ���� ���� - ������Ʈ ����	
	NiPoint3 kCameraLocal = kHeadWorld - m_spCharacter->GetTranslate() - m_CameraData.spOriendNode->GetTranslate();
	
	//ķ�� ��ġ ����
	// : ķ�� ��ġ�� ���� ���⿡�� ĳ������ �Ÿ���ŭ - �̵�
	// : fCamPitch�� ���콺 ���� �̵�, radius�μ� ĳ���Ͱ� ��ü�� ���Ϸ� �����̹Ƿ�
	// : �׿����� ī�޶��� ��ġ�� ��.�� ��.�� ��� �ؾ��Ѵ�.
	
	float fCamFrontMove = m_CameraData.fCamPitch * 0.2f;
	float fCameUpMove = NiAbs(m_CameraData.fCamPitch) * 0.33f;

	NiPoint3 kNewTrans = (mtRotZCam * NiPoint3(-m_CameraData.fCamDist + fCamFrontMove ,fCameUpMove,0.0f) 
		+ NiPoint3(0.0f,kCameraLocal.z,0.0f));

	m_CameraData.spCamera->SetTranslate(kNewTrans);
	
}

//ĳ���� ������ ��ȭ
void CameraManager::ChangeView(DWORD eView)		//ī�޶� ���� �ٲٱ� 1��Ī/3��Ī
{
	switch(eView)
	{
		case FIRST_PERSON:	//1��Ī ����
			m_fDistChange = -1.0f;
			break;
		case THIRD_PERSON:	//3��Ī ����
			m_fDistChange = 5.0f;
			break;
	}
}