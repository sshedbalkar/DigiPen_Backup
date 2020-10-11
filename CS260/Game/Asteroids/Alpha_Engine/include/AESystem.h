// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AESystem.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the system module
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_SYSTEM_H
#define AE_SYSTEM_H

// ---------------------------------------------------------------------------

// window related variables
extern HINSTANCE	ghAESysAppInstance;
extern HWND			ghAESysWin;
extern WNDCLASS		gAESysWinClass;
//extern AE_API int	gAESysWinExists;

extern const char*	gpAESysWinTitle;
extern const char*	gpAESysWinClassName;

extern AE_API int	gAESysAppActive;

// ---------------------------------------------------------------------------
#ifdef __cplusplus 
extern "C"
{
#endif

AE_API int AESysInit				(HINSTANCE hAppInstance, int show, int WinWidth, int WinHeight, int CreateConsole, LRESULT (CALLBACK *pWinCallBack)(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp));
AE_API void AESysReset				();
AE_API void AESysUpdate				();
AE_API void AESysExit				();

AE_API HWND AESysGetWindowHandle	();
AE_API int* AESysGetAppActive		();
AE_API void AESysSetWindowTitle		(const char *pTitle);

// ---------------------------------------------------------------------------

AE_API void AESysFrameStart();
AE_API void AESysFrameEnd();

// ---------------------------------------------------------------------------


#ifdef __cplusplus 
}
#endif

#endif // AE_SYSTEM_H

