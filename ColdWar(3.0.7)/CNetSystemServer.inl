inline SOCKET	CNetSystemServer::GetListenSocket()
{
	return m_SockListen;
}

inline int& CNetSystemServer::GetClientCnt()
{
	return m_nClientCnt;
}