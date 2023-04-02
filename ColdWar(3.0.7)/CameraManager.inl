inline CameraManager* CameraManager::Get()
{
	return m_gCamera;
}

inline NiCamera* CameraManager::GetCamera()
{ 
	return m_CameraData.spCamera;
}

inline float CameraManager::GetCameraHeading()
{ 
	return m_CameraData.fHeading; 
}

inline float CameraManager::GetCameraFrameYaw()	
{ 
	return m_CameraData.fFrameYaw;
}

inline float CameraManager::GetCameraFramePitch()
{ 
	return m_CameraData.fFramePitch;
}

inline float CameraManager::GetCameraPitch()
{
	return m_CameraData.fCamPitch;
}

inline void CameraManager::SetDuckFlag(bool bDuck)
{
	m_bDuck = bDuck;
}

inline bool CameraManager::GetFirstPerson()
{
	return m_bFirstPerson;
}



 
