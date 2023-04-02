#pragma warning(disable:4786)
#pragma warning(disable:4251)

#include "CStateManager.h"
#include "GameApp.h"

const char* g_szInvalidState = "invalid state";

//----------------------------------------------------------------------------
// CStateManager
//----------------------------------------------------------------------------

CStateManager::CStateManager()
{
	Clear();
}

CStateManager::~CStateManager()
{
	Release();
}

void CStateManager::Clear()
{
	m_CurrentState.clear();
	m_State.clear();

	while(!m_CmdQueue.empty())
		m_CmdQueue.pop();

	m_bFirstUpdate = false;
	m_bInitialized = false;
	m_fCurrentTime = 0.0f;
}

void CStateManager::Init()
{
	m_bFirstUpdate = true;
	m_bInitialized = true;
}

void CStateManager::Release()
{
	if(!m_bInitialized)
		return;

    // Make sure we properly exit the currently active state
    for(int i = m_CurrentState.size() - 1; i >= 0; i--)
        GetStateClass(m_CurrentState[i].c_str())->OnLeave(g_szInvalidState);

	Clear();
}


bool CStateManager::RegisterState(const char* szStateName, CBaseState* pState)
{
	if(!m_bInitialized)
		return false;

	// Attempt to find the given string ID
	GameStateClassMap::iterator itr = m_State.find(szStateName);
	
	// If the string ID already exists in the state map, return an error
	if(itr != m_State.end())
		return false;

	// Assign the object pointer to the ID location in the map
	m_State[szStateName] = pState;

	// If this is the first state registered, set it as the default state
	if(m_State.size() == 1)
		m_CurrentState.push_back(szStateName);

	return true;
}


bool CStateManager::IsStateChangePending() const
{
	return (m_CmdQueue.empty()) ? false : true;
}

const char* CStateManager::GetState() const
{
	if (m_CurrentState.size() == 0)
		return 0;
	return m_CurrentState[m_CurrentState.size() - 1].c_str();
}

int CStateManager::GetStateStackSize() const
{
	return m_CurrentState.size();
}


CBaseState* CStateManager::GetStateClass(const char* szState)
{
	// Make sure the string is not too long or a null string
	if (0 == szState)
		return(0);

	if (strlen(szState) >= MAX_STATE_STRING_LENGTH)
		return(0);

	if (m_State.empty())
		return(0);

	return m_State[szState];
}

CBaseState* CStateManager::GetCurrentStateClass()
{
	if (m_CurrentState.empty())
		return(0);

	return GetStateClass(m_CurrentState[m_CurrentState.size() - 1].c_str());
}

void CStateManager::ChangeState(const char* szState, float fDelay, bool bFlush)
{
	// Make sure the string is not too long or a null string
	if (strlen(szState) >= MAX_STATE_STRING_LENGTH)
		return;

	if (0 == szState)
		return;

	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "change state" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_CHANGE;
	strcpy_s(cmd.szState, MAX_STATE_STRING_LENGTH, szState);
	cmd.fDelay = fDelay + m_fCurrentTime;
	m_CmdQueue.push(cmd);
}

void CStateManager::PushState(const char* szState, float fDelay, bool bFlush)
{
	// Make sure the string is not too long or a null string
	if (strlen(szState) >= MAX_STATE_STRING_LENGTH)
		return;

	if (0 == szState)
		return;

	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "push state" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_PUSH;
	strcpy_s(cmd.szState, MAX_STATE_STRING_LENGTH, szState);
	cmd.fDelay = fDelay + m_fCurrentTime;
	m_CmdQueue.push(cmd);
}

void CStateManager::PopState(int iStatesToPop, float fDelay, bool bFlush)
{
	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "pop state" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_POP;
	cmd.szState[0] = 0;
	cmd.fDelay = fDelay + m_fCurrentTime;
	while(iStatesToPop > 0)
	{
		m_CmdQueue.push(cmd);
		// Only the first state command can have a non-zero value
		cmd.fDelay = 0.0f;
		iStatesToPop--;
	}
}

void CStateManager::PopAllStates(float fDelay, bool bFlush)
{
	// Clear the queue
	if (bFlush)
		while (!m_CmdQueue.empty())
			m_CmdQueue.pop();
	// Push a "pop all states" command onto the queue
	GSCommand cmd;
	cmd.eCmd = GS_CMD_POP_ALL;
	cmd.szState[0] = 0;
	cmd.fDelay = fDelay + m_fCurrentTime;
	m_CmdQueue.push(cmd);
}

void CStateManager::Update(float dt)
{
	// Update the total run time
	m_fCurrentTime += dt;

	// Check for the first update and make the appropriate calls
	if(m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
		GetCurrentStateClass()->OnEnter(g_szInvalidState);
	}
	// Empty our comment queue, consisting of commands to either
	// push new states onto the stack, pop states off the stack, or
	// to switch the states on the top of the stack.  In each case
	// we transmit the new state to the old one, and vice-versa.
	GSCommand cmd;
	while(!m_CmdQueue.empty() && (m_CmdQueue.front().fDelay <= m_fCurrentTime))
	{
		cmd = m_CmdQueue.front();
		if(cmd.eCmd == GS_CMD_PUSH)
		{
			const char* szPrev = GetState();
			if(strcmp(szPrev, cmd.szState) != 0)
			{
				GetCurrentStateClass()->OnOverride(cmd.szState);
				m_CurrentState.push_back(cmd.szState);
				GetCurrentStateClass()->OnEnter(szPrev);
			}
			else
			{
			}
		}
		else if(cmd.eCmd == GS_CMD_POP)
		{
			const char* szNext = g_szInvalidState;
			const char* szPrev = GetState();
			// If we only have one state, we shouldn't be popping it off the
			// current state stack.  We should be switching it instead.  This
			// code was hit because someone tried to pop off too many states.
			if(m_CurrentState.size() > 1)
			{
				GetCurrentStateClass()->OnLeave(m_CurrentState[m_CurrentState.size() - 2].c_str());
				m_CurrentState.pop_back();
				GetCurrentStateClass()->OnResume(szPrev);
			}
			else
			{
			}
		}
		else if(cmd.eCmd == GS_CMD_POP_ALL)
		{
			while(m_CurrentState.size() > 1)
			{
				const char* szNext = g_szInvalidState;
				const char* szPrev = GetState();
				GetCurrentStateClass()->OnLeave(m_CurrentState[m_CurrentState.size() - 2].c_str());
				m_CurrentState.pop_back();
				GetCurrentStateClass()->OnResume(szPrev);
			}
		}
		else
		{
			const char* szPrev = GetState();
			if(strcmp(szPrev, cmd.szState) != 0)
			{
				GetCurrentStateClass()->OnLeave(cmd.szState);
				m_CurrentState.pop_back();
				m_CurrentState.push_back(cmd.szState);
				
				GetCurrentStateClass()->OnEnter(szPrev);
				CGameApp::mp_Appication->SetState( GetCurrentStateClass() );
			}
			else
			{
			}
		}
		m_CmdQueue.pop();
	}
	// After all state transitions are finished, do the current state stack updates
	// EGT: Nov 2007 - modified the call to "Update" to pass in the current simulation time
	for(int i = m_CurrentState.size() - 1; i >= 0; i--)
		GetStateClass(m_CurrentState[i].c_str())->OnUpdate(m_fCurrentTime);
}




