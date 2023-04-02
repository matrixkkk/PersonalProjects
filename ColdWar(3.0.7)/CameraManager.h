#ifndef CAMERA_H_
#define CAMERA_H_

//#include "main.h"
#include "GameApp.h"

struct CameraData
{
	NiCameraPtr		spCamera;
	NiNodePtr		spOriendNode;			//카메라의 부모 노드

	float			fCamDist;				//캐릭터 오브젝트와 카메라 사이의 거리
	float			fCamHeight;				//카메라의 높이
	float			fCamPitch;				//카메라의 Pitch 회전값
	float			fZoomLevel;				//Zooming 값
	float			fZoomSpeed;				//줌 속도
	float			fMinZoomLevel;			//최소 줌 레벨
	float			fMaxZoomLevel;			//최대 줌 레벨
	float			fHeading;
	float			fFrameYaw;				//속도와 관련된 Yaw 아웃풋
	float			fFramePitch;	
	float			fLastWorldHeight;		//지형의 높이 저장
};

class CameraManager : public NiMemObject
{
public:
	CameraManager();
	~CameraManager();

	//static 카메라
	static CameraManager* m_gCamera;		//카메라의 정적 멤버 포인터
	static CameraManager* Get(); 

	//---------------- Get 멤버 함수--------------
	NiCamera* GetCamera();		
	
	float GetCameraHeading();			//카메라 heading 값 넘겨주기 
	float GetCameraFrameYaw();	
	float GetCameraFramePitch() ;
	float GetCameraPitch();
	bool  GetFirstPerson();

	void SetFrustumFarDistance( float fDel );
		
	//---------------- Set 멤버 함수------------------
	void  UpwardCameraFramePitch(int iY);
	
	//카메라 초기화(부모로 쓰일 오브젝트가 인자)
	bool Initialize(NiNode* pkCharacterAttach);

	void CameraReset();			//카메라 값 초기값으로 리셋
	void BeginCamera();			//카메라 렌더 시작
	void EndCamera();			//카메라 렌더 끝
	void UpdateCamera(float fTime);		//카메라 업데이트
	void UpdateCameraTransform();
	void SetDuckFlag(bool bDuck);
	void ChangeView(DWORD eView);		//카메라 시점 바꾸기 1인칭/3인칭

	void SetTest(float test) { m_CameraData.fCamHeight += test; }

	CameraData		m_CameraData;			//카메라 데이터

protected:
	NiNodePtr		m_spCharacter;				//카메라가 어태치될 캐릭터	
	bool			m_bZoomOn;					//줌 온/오프
	bool			m_bDuck;
	bool			m_bFirstPerson;				//1인칭 시점인지 여부
	//시점 거리
	float			m_fDistChange;				//시점 거리
	
	float m_fLastUpdateTime;		

};

#include "CameraManager.inl"

#endif
