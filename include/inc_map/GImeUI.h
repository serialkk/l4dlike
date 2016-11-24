#pragma once
#include "GCoreStd.h"
#include <imm.h>
#pragma comment( lib, "IMM32.lib" )

#define MAX_CANDIDATE_LENGTH 256
#define COUNTOF(a) ( sizeof( a ) / sizeof( ( a )[0] ) )
#define LANG_CHT	MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define INDICATOR_NON_IME	0
#define INDICATOR_CHS		1
#define INDICATOR_CHT		2
#define INDICATOR_KOREAN	3
#define INDICATOR_JAPANESE	4
#define GETLANG()		LOWORD(g_hklCurrent)
#define GETPRIMLANG()	((WORD)PRIMARYLANGID(GETLANG()))
#define GETSUBLANG()	SUBLANGID(GETLANG())

class GImeUI : public GSingleton < GImeUI >
{
private:
	friend class GSingleton<GImeUI>;
public:
	T_STR    m_CompString;
	T_STR    m_CompWord;
	T_STR    m_CurrentString;

	LPTSTR		m_pszIndicatior;
	HWND		m_hWndMain;
	HIMC		m_himcOrg;

	bool		m_bInitialized;
	HMODULE		m_hImmDll;
	DWORD		m_dwConversion;
	int			m_nComSize;
public:
	bool		ImeUi_Initialize(HWND hwnd, bool bDisable = false);
	void		ImeUi_Uninitialize();
	void		Uninitialize();
	void		Initialize(HWND hWnd);
	void		CheckInputLocale();
	void		ResetCompositionString();
	int			WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL		OnImeComposition(WPARAM wParam, LPARAM lParam);
public:
	GImeUI();
	virtual ~GImeUI();
};
#define I_Ime GImeUI::GetInstance()
