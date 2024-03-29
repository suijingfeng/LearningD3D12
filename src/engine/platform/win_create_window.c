/*
** WIN_DXIMP.C
**
** This file contains ALL Win32 create window specific stuff 
** for the DirectX12/Vulkan/OpenGL renderer. 
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

cvar_t* r_mode;


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


static void win_createWindowImpl(void)
{
	const char MAIN_WINDOW_CLASS_NAME[] = { "OpenArena" };

	Com_Printf(" Initializing window subsystem. \n");


	r_mode = Cvar_Get("r_mode", "3", 0);

	g_wv.desktopWidth = GetDesktopWidth();
	g_wv.desktopHeight = GetDesktopHeight();
	g_wv.m_windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;


	int x, y, w, h;
	// we never create a fullscreen at started,
	// we can toggle to fullscreen at leter time .

	if (0)
	{
		// fullscreen 
		// WS_POPUP | WS_VISIBLE ?
		// CS_HREDRAW | CS_VREDRAW

		g_wv.winWidth = g_wv.desktopWidth;
		g_wv.winHeight = g_wv.desktopHeight;
		//g_wv.m_windowStyle &= ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME);
		g_wv.m_windowStyle = WS_POPUP;
		g_wv.isFullScreen = 1;

		Com_Printf(" Fullscreen mode. \n");
		x = 0;
		y = 0;
		w = g_wv.desktopWidth;
		h = g_wv.desktopHeight;
	}
	else
	{

		int width;
		int height;
		RECT r;

		int mode = R_GetModeInfo(&width, &height, r_mode->integer, g_wv.desktopWidth, g_wv.desktopHeight);

		g_wv.winWidth = width;
		g_wv.winHeight = height;
		g_wv.isFullScreen = 0;


		r.left = 0;
		r.top = 0;
		r.right = width;
		r.bottom = height;
		// Compute window rectangle dimensions based on requested client area dimensions.
		AdjustWindowRect(&r, g_wv.m_windowStyle, FALSE);

		x = CW_USEDEFAULT;
		y = CW_USEDEFAULT;
		w = r.right - r.left;
		h = r.bottom - r.top;
		Cvar_Set("r_fullscreen", "0");
		Com_Printf(" windowed mode: %d. \n", mode);
	}


	//
	// register the window class if necessary
	//

	static int isWinRegistered = 0;

	if (isWinRegistered != 1)
	{
		WNDCLASSEX wc;

		memset(&wc, 0, sizeof(wc));

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = g_wv.hInstance;
		wc.hIcon = LoadIcon(g_wv.hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(void *)COLOR_GRAYTEXT;
		wc.lpszMenuName = 0;
		wc.lpszClassName = MAIN_WINDOW_CLASS_NAME;

		if (!RegisterClassEx(&wc))
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
		g_wv.m_windowStyle,
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


	ShowWindow(hwnd, SW_SHOW );
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

	// to determine if hardware gamma is supported
	pConfig->deviceSupportsGamma = qtrue;

	pConfig->textureEnvAddAvailable = qfalse; // not used
	pConfig->textureCompression = TC_NONE; // not used
	// init command buffers and SMP
	pConfig->stereoEnabled = qfalse; // not used
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