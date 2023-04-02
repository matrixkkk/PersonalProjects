#ifndef CAMERA_H_
#define CAMERA_H_

//#include "main.h"
#include "GameApp.h"

struct CameraData
{
	NiCameraPtr		spCamera;
	NiNodePtr		spOriendNode;			//ī�޶��� �θ� ���

	float			fCamDist;				//ĳ���� ������Ʈ�� ī�޶� ������ �Ÿ�
	float			fCamHeight;				//ī�޶��� ����
	float			fCamPitch;				//ī�޶��� Pitch ȸ����
	float			fZoomLevel;				//Zooming ��
	float			fZoomSpeed;				//�� �ӵ�
	float			fMinZoomLevel;			//�ּ� �� ����
	float			fMaxZoomLevel;			//�ִ� �� ����
	float			fHeading;
	float			fFrameYaw;				//�ӵ��� ���õ� Yaw �ƿ�ǲ
	float			fFramePitch;	
	float			fLastWorldHeight;		//������ ���� ����
};

class CameraManager : public NiMemObject
{
public:
	CameraManager();
	~CameraManager();

	//static ī�޶�
	static CameraManager* m_gCamera;		//ī�޶��� ���� ��� ������
	static CameraManager* Get(); 

	//---------------- Get ��� �Լ�--------------
	NiCamera* GetCamera();		
	
	float GetCameraHeading();			//ī�޶� heading �� �Ѱ��ֱ� 
	float GetCameraFrameYaw();	
	float GetCameraFramePitch() ;
	float GetCameraPitch();
	bool  GetFirstPerson();

	void SetFrustumFarDistance( float fDel );
		
	//---------------- Set ��� �Լ�------------------
	void  UpwardCameraFramePitch(int iY);
	
	//ī�޶� �ʱ�ȭ(�θ�� ���� ������Ʈ�� ����)
	bool Initialize(NiNode* pkCharacterAttach);

	void CameraReset();			//ī�޶� �� �ʱⰪ���� ����
	void BeginCamera();			//ī�޶� ���� ����
	void EndCamera();			//ī�޶� ���� ��
	void UpdateCamera(float fTime);		//ī�޶� ������Ʈ
	void UpdateCameraTransform();
	void SetDuckFlag(bool bDuck);
	void ChangeView(DWORD eView);		//ī�޶� ���� �ٲٱ� 1��Ī/3��Ī

	void SetTest(float test) { m_CameraData.fCamHeight += test; }

	CameraData		m_CameraData;			//ī�޶� ������

protected:
	NiNodePtr		m_spCharacter;				//ī�޶� ����ġ�� ĳ����	
	bool			m_bZoomOn;					//�� ��/����
	bool			m_bDuck;
	bool			m_bFirstPerson;				//1��Ī �������� ����
	//���� �Ÿ�
	float			m_fDistChange;				//���� �Ÿ�
	
	float m_fLastUpdateTime;		

};

#include "CameraManager.inl"

#endif
