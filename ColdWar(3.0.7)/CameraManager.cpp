#include "CameraManager.h"
#include "WorldManager.h"

CameraManager* CameraManager::m_gCamera = NULL;

CameraManager::CameraManager()
{
	m_bZoomOn = false;
	m_bDuck	  = false;
	m_bFirstPerson = false;
	
	m_fLastUpdateTime = 0;

	m_gCamera = this;			//카메라를 자유자재로 갖다 쓰기위해 static 포인터 지정

	//카메라 생성
	m_CameraData.spCamera = NiNew NiCamera;
	
	//카메라의 near와 far 설정
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

	//인자로 넘어온 캐릭터 노드에 Orient를 붙인다.
	pkCharacterAttach->AttachChild(m_CameraData.spOriendNode);

	//Orient노드에 카메라를 붙인다.
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
	//렌더러 가져옴
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	pkRenderer->BeginUsingDefaultRenderTargetGroup(NiRenderer::CLEAR_ALL);

	//여기서는 "브레이크 렌더링"을 사용한다.
	//하나의 패스에서 하나의 씬 그래프를 렌더하기 보다는 이 방법은 프레임버퍼와 깊이 버퍼
	//안에 결과를 혼합하면서 싱글 프레임에서 다수의 씬 그래프를 렌더하는 능력
	//을 어플리케이션이 허용한다.

	NiCamera* pkCamera = m_CameraData.spCamera;
	//렌더러에 카메라 포트와 기본값을 셋한다.
	pkRenderer->SetCameraData(pkCamera);
}

void CameraManager::EndCamera()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	//프레임을 닫는다.
	pkRenderer->EndUsingRenderTargetGroup();
}

//총의 반동의 따른 카메라의 Pitch
void CameraManager::UpwardCameraFramePitch(int iY)
{
	m_CameraData.fCamPitch -= NI_PI * 0.375f * (float)(iY) / (float)CGameApp::mp_Appication->GetAppHeight();
}

void CameraManager::CameraReset()
{
	//캐릭터와 카메라 사이의 디폴트 거리
	m_CameraData.fCamDist = 4.0f;
	
	//캐릭터에 근거한 카메라의 높이
	//FPS에서 가장 최상의 카메라 높이는 캐릭터의 눈이다.
	m_CameraData.fCamHeight = 1.65f;		//캐릭터 클래스가 없으므로 임시
	
	m_CameraData.fCamPitch = 0.0f;

	//줌 디폴트 : 1x
	m_CameraData.fZoomLevel = 1.0f;

	//줌의 속도 : 약 1초 
	m_CameraData.fZoomSpeed = 4.0f;

	//줌의 최소의 최대 : 1x ~ 4x
	m_CameraData.fMinZoomLevel = 1.0f;
	m_CameraData.fMaxZoomLevel = 4.0f;

	// The initial heading is specified by the soldier class, which parses it
    // from the data file.   캐릭터가 없으므로 임시처리
	m_CameraData.fHeading = 4.72f;

	m_CameraData.fFrameYaw   = 0.0f;
	m_CameraData.fFramePitch = 0.0f;
	m_CameraData.fLastWorldHeight = 0.0f;
}

void CameraManager::UpdateCamera(float fTime)
{		
	//마우스 look 업데이트 코드
	NiInputMouse* pkMouse = CGameApp::mp_Appication->GetInputSystem()->GetMouse();

	if(!pkMouse)
		return;

	//이전의 시간부터 흐른 시간 계산
	float fDelta = fTime - m_fLastUpdateTime;

	int iX,iY,iZ;   //x : 마우스의 좌표 x축, y : 마우스의 좌표 y축, z : 마우스 휠 좌표
	
	if(pkMouse->GetPositionDelta(iX,iY,iZ))
	{		
		//마우스의 수평 모션으로 캐릭터의 heading의 변화를 계산한다.
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
			//마우스 수직 모션으로 Pitch변화 계산
			float fPitchDelta = NI_PI * 0.375f * (float)(iY) / (float)uiAppHeight;
			m_CameraData.fCamPitch += fPitchDelta;
			
			if(fDelta > 0.005f)
				m_CameraData.fFramePitch = -fPitchDelta / (NI_PI * fDelta);
			else
				m_CameraData.fFramePitch = 0.0f;
		}

//카메라의 위치를 조정하는 휠키 사용에 관한 부분( 캐릭터의 카메라에는 사용안함)
//#ifdef MODE_TEST   
		//float fDistChange = -iZ / 400.0f;		//120.0f는 카메라 줌/아웃이 이동거리 크기

        const float fMinCamDist = 1.00f;			// 3인칭 시점에서 뷰의 줌인 최소 거리
        const float fMaxCamDist = 5.0f;		// 3인칭 시점으로 줌 아웃할 최대 거리
        const float fFirstPersonDist = -0.1f;	// 1인칭 시점일 때의 카메라의 캐릭터 사이의 거리

        if ((m_CameraData.fCamDist >= fMinCamDist) 
            && ((m_CameraData.fCamDist + m_fDistChange) < fMinCamDist))
        {
            //만일 현재 카메라의 변화가 3인칭 시점 최소 거리보다 
			//가깝게 되면 1인칭 시점으로 위치 고정
            m_CameraData.fCamDist = fFirstPersonDist;  

			//시점 플래그 ON
			m_bFirstPerson = true;
        }
        else if (m_CameraData.fCamDist < fMinCamDist) 
        {
            //만일 우리가 1인칭 시점 카메라이고 카메라를 뒤로 줌 아웃하면
			//3인칭 카메라 최소 거리로 놓는다.
            if (m_fDistChange > 0)
                m_CameraData.fCamDist = fMinCamDist;
        }
        else
        {
			//시점 플래그 OFF
			m_bFirstPerson = false;

            //3인칭 카메라일 때의 처리
            m_CameraData.fCamDist += m_fDistChange;

			//최대 줌 아웃될 거리를 제한
            if (m_CameraData.fCamDist > fMaxCamDist)
                m_CameraData.fCamDist = fMaxCamDist;
        }		
//#endif
	}

	float fPitchMax = NI_PI * 0.30f;		//pitch의 각도 (고개를 들고 숙이는 각도)
	
	//Upward-looking을 위한 pitch 각도를 계산한다.
	//Upward-looking은 카메라 줌거리에 따라 제한적으로 조절된다.
	//캐릭터와 카메라의 거리가 1e-5f(1에 근접한 수) 보다 작다면(1인칭 시점) Pitch의 min은 -Pitch max이다.
	float fPitchMin =(m_CameraData.fCamDist > 1e-5f) ? 
        -NiASin(m_CameraData.fCamHeight 
        / (m_CameraData.fCamHeight + m_CameraData.fCamDist)) :
        -fPitchMax;

	//min max에 따라 pitch각도 제한
	if (m_CameraData.fCamPitch < fPitchMin)
        m_CameraData.fCamPitch = fPitchMin;
    else if (m_CameraData.fCamPitch > fPitchMax)
        m_CameraData.fCamPitch = fPitchMax;

	
	UpdateCameraTransform();

	//줌에 대한 설정
	if(pkMouse->ButtonWasPressed(NiInputMouse::NIM_RIGHT))
	{
		if(m_bZoomOn)  //줌 상태일 경우
		{
			m_bZoomOn = false;		//줌 X
			m_CameraData.fZoomLevel = m_CameraData.fMinZoomLevel;
		}
		else			//줌 상태가 아닐경우
		{
			m_bZoomOn = true;
			m_CameraData.fZoomLevel = m_CameraData.fMaxZoomLevel;
		}
	}

	//줌 레벨에 따른 frustrum 계산
	float fInvZoom = 1.0f / m_CameraData.fZoomLevel;
	const NiRenderTargetGroup* pkRTGroup = 
		NiRenderer::GetRenderer()->GetDefaultRenderTargetGroup();
	//종횡비
	float fAspect = (float)pkRTGroup->GetHeight(0) /
		(float)pkRTGroup->GetWidth(0);
	//frustum 적용
	NiFrustum kFrustum(-1.0f * fInvZoom, 1.0f * fInvZoom,
		fAspect * fInvZoom, -fAspect * fInvZoom,0.1f, 130.0f);
	m_CameraData.spCamera->SetViewFrustum(kFrustum);
	m_CameraData.spCamera->Update(fTime);	

		
	m_fLastUpdateTime = fTime;
	
}

void CameraManager::UpdateCameraTransform()
{			
	NiMatrix3 mtRotZCam;		//카메라 회전

	mtRotZCam.MakeZRotation(m_CameraData.fCamPitch);		//카메라를 z축을 중심으로 회전
	
	m_CameraData.spCamera->SetRotate(mtRotZCam);

	//머리의 노드를 가져온다.
	NiNode* pkHead = (NiNode*) m_spCharacter->GetObjectByName("Bip01 Head");
	assert(pkHead);

	//Head의 world 이동행렬을 가져온다.
	NiPoint3 kHeadWorld = pkHead->GetWorldTranslate();

	//카메라 로컬 = 헤드월드 - 캐릭터 로컬 - 오리엔트 로컬	
	NiPoint3 kCameraLocal = kHeadWorld - m_spCharacter->GetTranslate() - m_CameraData.spOriendNode->GetTranslate();
	
	//캠의 위치 설정
	// : 캠의 위치는 보는 방향에서 캐릭터의 거리만큼 - 이동
	// : fCamPitch는 마우스 상하 이동, radius로서 캐릭터가 상체를 상하로 움직이므로
	// : 그에따라 카메라의 위치를 전.후 상.하 운동을 해야한다.
	
	float fCamFrontMove = m_CameraData.fCamPitch * 0.2f;
	float fCameUpMove = NiAbs(m_CameraData.fCamPitch) * 0.33f;

	NiPoint3 kNewTrans = (mtRotZCam * NiPoint3(-m_CameraData.fCamDist + fCamFrontMove ,fCameUpMove,0.0f) 
		+ NiPoint3(0.0f,kCameraLocal.z,0.0f));

	m_CameraData.spCamera->SetTranslate(kNewTrans);
	
}

//캐릭터 시점의 변화
void CameraManager::ChangeView(DWORD eView)		//카메라 시점 바꾸기 1인칭/3인칭
{
	switch(eView)
	{
		case FIRST_PERSON:	//1인칭 시점
			m_fDistChange = -1.0f;
			break;
		case THIRD_PERSON:	//3인칭 시점
			m_fDistChange = 5.0f;
			break;
	}
}