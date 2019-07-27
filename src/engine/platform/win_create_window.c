/*
** WIN_DXIMP.C
**
** This file contains ALL Win32 specific stuff having to do with the
** directx 12 refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** DXimp_EndFrame
** DXimp_Init
** DXimp_LogComment
** DXimp_Shutdown
**
*/

#include "../client/client.h"
#include "win_mode.h"
#include "win_gamma.h"
#include "win_log.h"
#include "win_public.h"

extern WinVars_t g_wv;
extern HWND create_main_window(int width, int height, bool fullscreen);



static int GetDesktopColorDepth(void)
{
	HDC hdc = GetDC(GetDesktopWindow());
	int value = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(GetDesktopWindow(), hdc);
	return value;
}


static int GetDesktopWidth(void)
{
	HDC hdc = GetDC(GetDesktopWindow());
	int value = GetDeviceCaps(hdc, HORZRES);
	ReleaseDC(GetDesktopWindow(), hdc);
	return value;
}


static int GetDesktopHeight(void)
{
	HDC hdc = GetDC(GetDesktopWindow());
	int value = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(GetDesktopWindow(), hdc);
	return value;
}



static void win_createWindowImpl( void )
{
	Com_Printf( " Initializing window subsystem. \n" );

	cvar_t* win_fullscreen = Cvar_Get("r_fullscreen", "1", 0);

	cvar_t* win_mode = Cvar_Get("r_mode", "3", 0);

	g_wv.desktopWidth = g_wv.winWidth = GetDesktopWidth();
	g_wv.desktopHeight = g_wv.winHeight = GetDesktopHeight();

	if ( win_fullscreen->integer )
	{
		// fullscreen 

		g_wv.winWidth = g_wv.desktopWidth;
		g_wv.winHeight = g_wv.desktopHeight;

		g_wv.isFullScreen = true;
		Cvar_Set("r_fullscreen", "1");
		Com_Printf(" Setting fullscreen mode. ");
	}
	else
	{
		R_GetModeInfo(&g_wv.winWidth, &g_wv.winHeight, win_mode->integer);
		g_wv.isFullScreen = false;
		win_fullscreen->integer = 0;
		Cvar_Set("r_fullscreen", "0");
		Com_Printf(" Setting windowed mode. ");
	}


	g_wv.hWnd = create_main_window(g_wv.winWidth, g_wv.winHeight, g_wv.isFullScreen);

	// Brings the thread that created the specified window into the foreground 
	// and activates the window. Keyboard input is directed to the window, and
	// various visual cues are changed for the user. 
	// The system assigns a slightly higher priority to the thread that created
	// the foreground window than it does to other threads.
	if ( SetForegroundWindow( g_wv.hWnd ) )
		Com_Printf( " Bring window into the foreground successed. \n" );

	// Sets the keyboard focus to the specified window.
	// A handle to the window that will receive the keyboard input.
	// If this parameter is NULL, keystrokes are ignored.
	SetFocus(g_wv.hWnd);
}


static void win_destroyWindowImpl(void)
{
	Com_Printf( " Shutting down DX12 subsystem. \n");

	if (g_wv.hWnd)
	{
		Com_Printf( " Destroying window system. \n");
		
		DestroyWindow( g_wv.hWnd );

		g_wv.hWnd = NULL;
	}
}



void GLimp_Init(struct glconfig_s * const pConfig, void **pContext)
{

	win_createWindowImpl();
	
	win_InitDisplayModel();

	win_InitLoging();

	pConfig->vidWidth = g_wv.winWidth;
	pConfig->vidHeight = g_wv.winHeight;
	pConfig->windowAspect = (float)g_wv.winWidth / (float)g_wv.winHeight;
	pConfig->isFullscreen = g_wv.isFullScreen ? qtrue : qfalse;
	pConfig->stereoEnabled = qfalse;
	pConfig->smpActive = qfalse;
	pConfig->UNUSED_displayFrequency = 60;
	pConfig->deviceSupportsGamma = qtrue;

	// allways enable stencil
	pConfig->stencilBits = 8;
	pConfig->depthBits = 24;
	pConfig->colorBits = 32;
	pConfig->deviceSupportsGamma = win_checkHardwareGamma();

	////
	*pContext = &g_wv;
}


void GLimp_Shutdown(void)
{
	win_destroyWindowImpl();

	// For DX12 mode we still have qgl pointers initialized with placeholder values.
	// Reset them the same way as we do in opengl mode.

	win_restoreGamma();

	win_EndDisplayModel();

	win_EndLoging();
}


void GLimp_EndFrame(void)
{
	;
}
