//----------------------------------------------------------
//  File: GameApp.h 
//
//  Desc: ���� ������ ���� Ŭ����, ��ü���� ������ ������ ��
//		  ������� ����Ѵ�.
//  written by ������
//-------------------------------------------------------------  

#ifndef GAMEAPP_H_
#define GAMEAPP_H_

#include <NiFont.h>
#include <NiString2D.h>
#include <NiColor.h>
#include "main.h"
#include "GameState.h"
#include "NiDX9RendererEx.h"
#include "SoundManager.h"
#include "resource4.h"

//------------- UI -------------
#include "CBaseState.h"
#include "CStateManager.h"
#include "CEGUIGBRenderSystem.h"
//------------------------------

NiSmartPointer(NiFastAlphaAccumulator);

class CGameApp : public NiMemObject
{
public:
	//------------------------------------------
	// ������ �� �Ҹ���
	//------------------------------------------
	CGameApp();
	~CGameApp();

	//------------------------------------------
	// public ����
	//-----------------------------------------

	static CGameApp* mp_Appication;					//���ø����̼� ������ ���� ���� ������
	
	//-------------------------------------------
	// public �Լ� 
    //-------------------------------------------
	
	
	//	MessageProc
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MainWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	
	//Media Path
	static void		SetMediaPath(const char* pcPath);			//�̵��  ���� ���
	const char*		GetMediaPath();
	static const char*		ConvertMediaFilename(const char* pcFilename);
	static void GetMinMax( NiAVObject* pkObject, NiPoint3 *min, NiPoint3 *max );

	//	������Ʈ�� ���������� ����
	static void ChangeNodeAlpha(NiAVObject *pkObject, float fAlpha);	

	// ������
	NiRenderer*		GetRenderer()		 { return m_spRenderer;}
	
	//InputSystem
	NiInputSystem*	GetInputSystem() { return  m_spInputSystem; }  
	unsigned int	GetAppWidth()	 { return  m_nWidth; }
	unsigned int	GetAppHeight()	 { return  m_nHeight; }
	HWND			GetAppHandle()	 { return  m_hWnd;	}
	float			GetAccumTime()	 { return  m_fAccumTime; }
	NiCursor*		GetCursor()		 { return  m_spCursor; }
	CBaseState*		GetState()		 { return  m_CurrState; }
	void			SetState(CBaseState* pState){ m_CurrState = pState; }
	
	//������ �ʱ�ȭ �Լ�
	bool		Initialize();
	bool		CreateMainWindow(LPCTSTR lpCaption,HINSTANCE hInstance, int iWinMode,bool bFullScreen=false);
	bool		ShutDown();

	// ���ø����̼� ����
	int			BeginGame();

	
	//idle processing
	void		OnIdle();				//���� ����
	void		RenderFrame();			//������ ����
	void		UpdateFrame();			//������Ʈ	
	void		ProcessInput();			//Ű �Է� ó��
	void		SetInputMode();			//��ǲ ��� ����
	
	void		MoveCursor(int x,int y);//Ŀ�� �̵�
//	bool		ChangeState(CGameState* state);
	
	NiColor		GetBackGroundColor();

	void		QuitApplication();		// ���ø����̼� ����
	float		GetFrameTime(){ return m_fFrameTime; }

	bool IsIME(){ return m_bIME; }		// IME Ȱ�� ����
	void SetIME( bool bIme ){ m_bIME = bIme; }

	bool IsActiveEditBox(){ return m_bActiveEditBox; }
	void SetActiveEditBox( bool bActive ){ m_bActiveEditBox = bActive; }

	const char*	GetFontName() const;
	
	// ������Ʈ ������
	inline static CStateManager &GetStateManager()
	{
		return(mp_Appication->m_StateManager);
	}

	//----- ������ ���� �Լ�
	void		EnterCS();
	void		LeaveCS();
	
private:
	//---------------------------------------------------
	// private �Լ�
	//---------------------------------------------------
	bool		CreateRenderer();				//������ ����
	bool		CreateInputSystem();			//��ǲ �ý��� ����
	bool		CreateCursor();					//Ŀ�� ����
	//bool		CreateFont();					//��Ʈ ��ü ����
	bool		CreateSound();					//���� �Ŵ���

	bool		MeasureTime();					//Idle���� �ð� ����

	void		SetMaxFrameRate(float fMax);	//�������� ����

	bool		InitializeGUIFramework();		// ���Ӻ긮���� ���� CEGUI ������ ����
	void		RegisterBaseStates();			// ���� ������Ʈ ���� ���� �� ���

	void		IMEComposition( HWND hWnd, LPARAM lParam );	// IME ���� �ѱ�ó�� �Լ�
	
	//-----------------------------------------------------
	// private ����
	//-----------------------------------------------------
			
	//���� ���� ��������
	CBaseState*			m_CurrState;
	
	//�⺻ ������Ʈ
	NiRendererPtr		m_spRenderer;
	NiInputSystemPtr	m_spInputSystem;

	CStateManager			m_StateManager;					// ������Ʈ ������
	CEGUIGBRenderSystemPtr	m_spCEGUIGBRenderer;			// ���Ӻ긮���� GEGUI ������

	NiCursorPtr				m_spCursor;
		
	//---------------------------------------------------------
	//������ ���� ����
	NiWindowRef			 m_hWnd;							//���� ������ �ڵ�
	NiWindowRef			 m_RenderWnd;						//���� ������ �ڵ�
	NiInstanceRef		 m_hInst;
	HMENU				 m_hMenu;							//�޴� ID
	HICON				 m_hIcon;							//������
	
	NiAcceleratorRef     ms_pAccel;
	
	bool				 m_bFullScreen;						//Ǯ��ũ��
	bool				 m_bExclusiveMouse;					//���콺 ���� ���� Ÿ��
	bool				 m_bIsHangul;						//���� ����
	bool				 m_bActiveEditBox;					//����Ʈ �ڽ� ��� ����
	unsigned long	     m_ulWindowStyle;					//������ ��Ÿ��
	unsigned int		 m_nWidth;							//Ǯ ��ũ�� �϶��� width�ȼ���
	unsigned int		 m_nHeight;							//Ǯ ��ũ�� �϶��� height�� �ȼ���

	//---------------------------------------------------------

	//---------------------------------------------------------
	//�ð� ���� ����
	
	int m_iMaxTimer;
    int m_iTimer;
    float m_fCurrentTime; // Time reported by system
    float m_fLastTime; // Last time reported by system
    float m_fAccumTime; // Time elapsed since application start
    float m_fFrameTime; // Time elapsed since previous frame
    int m_iClicks;
    float m_fFrameRate;
    float m_fLastFrameRateTime;
    int m_iLastFrameRateClicks;

	float m_fCullTime;
    float m_fRenderTime;
    float m_fUpdateTime;
    float m_fBeginUpdate;
    float m_fBeginCull;
    float m_fBeginRender;

	// ������ ���� ���� ����
    float m_fMinFramePeriod;
    float m_fLastFrame;
	
	CRITICAL_SECTION	m_cs;


	//---------------------------------------------------------
	
	//ȭ�鿡 ���� ���
	NiFontPtr			m_spFont;
	NiString2DPtr		m_spText;

	std::string			m_strFont;

	NiColor				m_BackGroundColor;	// ��׶��� �÷�

protected:
	//�̵�� ���� ��� 
    static char ms_acMediaPath[NI_MAX_PATH];
    static char ms_acTempMediaFilename[NI_MAX_PATH];

	static bool			 m_bIME;							//IME ����
	
};



#endif
