inline CCharacter*	CharacterManager::GetCharacterIndex(int index)
{
	return m_pkVecCharacter[index];
}

inline CUserCharacter* CharacterManager::GetUserCharacter()
{
	return m_pkUserCharacter;
}

inline std::vector<CCharacter*>& CharacterManager::GetColliderVector()
{
	return m_pkVecCharacter;
}
 