inline NiPoint2* CUserCharacter::GetFirePoint()		
{
	return &m_kFirePoint; 
}

inline CollisionManager::IntersectRecord& CUserCharacter::GetIntersectionRecord()
{
	return m_kIntersectionRecord;
}

inline void CUserCharacter::ResetIntersectionRecord()			//충돌 Record 리셋
{
	m_kIntersectionRecord.m_bInCollision = false;
	m_kIntersectionRecord.m_fTime = FLT_MAX;
	m_kIntersectionRecord.m_kObjectNormal = NiPoint3::ZERO;
}

inline CrossHair* CUserCharacter::GetCrossHairDisplay()
{
	return m_kCrossHairObject;
}

inline CWeapon*	 CUserCharacter::GetEquipWeapon()
{
	return m_pkEquipWeapon;
}






