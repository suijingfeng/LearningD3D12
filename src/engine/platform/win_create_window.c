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
#include "resource.h"

extern WinVars_t g_wv;

extern LRESULT WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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

	int width = g_wv.desktopWidth = GetDesktopWidth();
	int height = g_wv.desktopHeight = GetDesktopHeight();

	int	stylebits;

	RECT r;
	int x, y, w, h;

	if ( win_fullscreen->integer )
	{
		// fullscreen 
		stylebits = WS_POPUP | WS_VISIBLE;
		g_wv.winWidth = g_wv.desktopWidth;
		g_wv.winHeight = g_wv.desktopHeight;

		g_wv.isFullScreen = 1;
		Cvar_Set("r_fullscreen", "1");
		Com_Printf(" Fullscreen mode. \n");
		x = 0;
		y = 0;
		w = g_wv.desktopWidth;
		h = g_wv.desktopWidth;
	}
	else
	{
		stylebits = WS_OVERLAPPEDWINDOW | WS_VISIBLE;


		int mode = R_GetModeInfo(&width, &height, win_mode->integer, g_wv.desktopWidth, g_wv.desktopHeight);
		
		g_wv.winWidth = width;
		g_wv.winHeight = height;
		g_wv.isFullScreen = 0;

	
		r.left = 0;
		r.top = 0;
		r.right = width;
		r.bottom = height;

		AdjustWindowRect(&r, stylebits, FALSE);

		x = CW_USEDEFAULT;
		y = CW_USEDEFAULT;
		w = r.right - r.left;
		h = r.bottom - r.top;
		Cvar_Set("r_fullscreen", "0");
		Com_Printf(" windowed mode: %d. \n", mode);
	}


#define	MAIN_WINDOW_CLASS_NAME	"OpenArena"
	// g_wv.hWnd = create_main_window(g_wv.winWidth, g_wv.winHeight, g_wv.isFullScreen);

	//
	// register the window class if necessary
	//

	static int isWinRegistered = 0;

	if (isWinRegistered != 1)
	{
		WNDCLASS wc;

		memset(&wc, 0, sizeof(wc));

		wc.style = 0;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = g_wv.hInstance;
		wc.hIcon = LoadIcon(g_wv.hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(void *)COLOR_GRAYTEXT;
		wc.lpszMenuName = 0;
		wc.lpszClassName = MAIN_WINDOW_CLASS_NAME;

		if (!RegisterClass(&wc))
		{
			Com_Error(ERR_FATAL, "create main window: could not register window class");
		}

		isWinRegistered = 1;

		Com_Printf(" Window class registered. \n");
	}



	HWND hwnd = CreateWindowEx(
		0,
		MAIN_WINDOW_CLASS_NAME,
		MAIN_WINDOW_CLASS_NAME,
		// The following are the window styles. After the window has been
		// created, these styles cannot be modified, except as noted.
		stylebits,
		x, y, w, h,
		NULL,
		NULL,
		// A handle to the instance of the module to be associated with the window
		g_wv.hInstance,
		&g_wv);

	if (!hwnd)
	{
		Com_Error(ERR_FATAL, " Couldn't create window ");
	}

#undef	MAIN_WINDOW_CLASS_NAME

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	g_wv.hWnd = hwnd;


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

void win_minimizeImpl(void)
{
	;
}

void GLimp_Init(struct glconfig_s * const pConfig, void **pContext)
{
	// These values force the UI to disable driver selection
	pConfig->driverType = GLDRV_ICD;
	pConfig->hardwareType = GLHW_GENERIC;

	// Only using SDL_SetWindowBrightness to determine if hardware gamma is supported
	pConfig->deviceSupportsGamma = qtrue;

	pConfig->textureEnvAddAvailable = qfalse; // not used
	pConfig->textureCompression = TC_NONE; // not used
	// init command buffers and SMP
	pConfig->stereoEnabled = qfalse;
	pConfig->smpActive = qfalse; // not used

	// hardcode it
	pConfig->colorBits = 32;
	pConfig->depthBits = 24;
	pConfig->stencilBits = 8;

	pConfig->displayFrequency = 60;

	win_createWindowImpl();
	
	win_InitDisplayModel();
	
	Cmd_AddCommand("minimize", win_minimizeImpl);

	win_InitLoging();

	pConfig->vidWidth = g_wv.winWidth;
	pConfig->vidHeight = g_wv.winHeight;
	pConfig->windowAspect = (float)g_wv.winWidth / (float)g_wv.winHeight;
	pConfig->isFullscreen = g_wv.isFullScreen ? qtrue : qfalse;


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

	Cmd_RemoveCommand("minimize");
	win_EndLoging();
}


void GLimp_EndFrame(void)
{
	;
}
