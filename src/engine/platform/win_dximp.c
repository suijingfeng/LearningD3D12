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

#include <stdio.h>

#include "../client/client.h"
#include "win_local.h"
#include "win_mode.h"
#include "win_config.h"
#include "win_gamma.h"

extern HWND create_main_window(int width, int height, bool fullscreen);


FILE* log_fp;


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



void dx_imp_init( void )
{
	Com_Printf( " Initializing DX12 subsystem \n" );

	// Create window.
	// SetMode(r_mode->integer, (qboolean)r_fullscreen->integer);

	cvar_t* win_fullscreen = Cvar_Get("r_fullscreen", "1", 0);

	cvar_t* win_mode = Cvar_Get("r_mode", "3", 0);


	if ( win_fullscreen->integer )
	{
		// fullscreen 
		g_wv.winWidth = GetDesktopWidth();
		g_wv.winHeight = GetDesktopHeight();
		g_wv.isFullScreen = true;
		Com_Printf(" Setting fullscreen mode. ");

		win_fullscreen->integer = 1;
	}
	else
	{
		R_GetModeInfo(&g_wv.winWidth, &g_wv.winHeight, win_mode->integer);
		g_wv.isFullScreen = false;
		win_fullscreen->integer = 0;
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

	win_checkHardwareGamma();

	setWinConfig(g_wv.winWidth, g_wv.winHeight, g_wv.isFullScreen, 3, 60);
}


void dx_imp_shutdown(void)
{
	Com_Printf( " Shutting down DX12 subsystem. \n");

	if (g_wv.hWnd)
	{
		Com_Printf( " ...destroying DX12 window. \n");
		
		DestroyWindow( g_wv.hWnd );

		g_wv.hWnd = NULL;
	}

	// For DX12 mode we still have qgl pointers initialized with placeholder values.
	// Reset them the same way as we do in opengl mode.

	win_restoreGamma();

	if (log_fp)
	{
		fclose(log_fp);
		log_fp = 0;
	}
}


void GLimp_LogComment(char * const comment)
{
	if (log_fp) {
		fprintf(log_fp, "%s", comment);
	}
}