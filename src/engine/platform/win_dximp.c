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

#include "../renderer/tr_local.h"
#include "resource.h"
#include "win_local.h"

#define	MAIN_WINDOW_CLASS_NAME	"OpenArena"
const char APP_MANIFEST_TABLE[32] = {"Learning DirectX 12"};

extern refimport_t ri;

extern void WG_CheckHardwareGamma( void );
extern void WG_RestoreGamma( void );

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


static void SetMode(int mode, qboolean fullscreen)
{
	if (fullscreen)
	{
		ri.Printf(PRINT_ALL, "...setting fullscreen mode:");
		glConfig.vidWidth = GetDesktopWidth();
		glConfig.vidHeight = GetDesktopHeight();
		glConfig.windowAspect = 1.0f;
	}
	else
	{
		ri.Printf(PRINT_ALL, "...setting mode %d:", mode);
		if (!R_GetModeInfo(&glConfig.vidWidth, &glConfig.vidHeight, &glConfig.windowAspect, mode))
		{
			ri.Printf(PRINT_ALL, " invalid mode\n");
			ri.Error(ERR_FATAL, "SetMode - could not set the given mode (%d)\n", mode);
		}

		// Ensure that window size does not exceed desktop size.
		// CreateWindow Win32 API does not allow to create windows larger than desktop.
		int desktop_width = GetDesktopWidth();
		int desktop_height = GetDesktopHeight();

		if (glConfig.vidWidth > desktop_width || glConfig.vidHeight > desktop_height)
		{
			int default_mode = 4;
			ri.Printf(PRINT_WARNING, "\n Mode %d specifies width that is larger than desktop width: using default mode %d\n",
				mode, default_mode);

			ri.Printf(PRINT_ALL, "...setting mode %d:", default_mode);
			if (!R_GetModeInfo(&glConfig.vidWidth, &glConfig.vidHeight, &glConfig.windowAspect, default_mode)) {
				ri.Printf(PRINT_ALL, " invalid mode\n");
				ri.Error(ERR_FATAL, "SetMode - could not set the given mode (%d)\n", default_mode);
			}
		}
	}
	glConfig.isFullscreen = fullscreen;
	ri.Printf(PRINT_ALL, " %d %d %s\n", glConfig.vidWidth, glConfig.vidHeight, fullscreen ? "FS" : "W");
}


static HWND create_main_window(int width, int height, qboolean fullscreen)
{
	//
	// register the window class if necessary
	//


	cvar_t* cv = ri.Cvar_Get("win_wndproc", "", 0);
	WNDPROC	wndproc;
	sscanf(cv->string, "%p", (void **)&wndproc);

	WNDCLASS wc;

	memset(&wc, 0, sizeof(wc));

	wc.style = 0;
	wc.lpfnWndProc = wndproc;
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
		ri.Error(ERR_FATAL, "create main window: could not register window class");
	}

	ri.Printf(PRINT_ALL, "...registered window class\n");


	//
	// compute width and height
	//
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = width;
	r.bottom = height;

	int	stylebits;
	if (fullscreen)
	{
		stylebits = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
	}
	else
	{
		stylebits = WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_SYSMENU;
		AdjustWindowRect(&r, stylebits, FALSE);
	}

	int w = r.right - r.left;
	int h = r.bottom - r.top;

	int x, y;

	if (fullscreen)
	{
		x = 0;
		y = 0;
	}
	else
	{
		cvar_t* vid_xpos = ri.Cvar_Get("vid_xpos", "", 0);
		cvar_t* vid_ypos = ri.Cvar_Get("vid_ypos", "", 0);
		x = vid_xpos->integer;
		y = vid_ypos->integer;

		// adjust window coordinates if necessary 
		// so that the window is completely on screen
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;

		int desktop_width = GetDesktopWidth();
		int desktop_height = GetDesktopHeight();

		if (w < desktop_width && h < desktop_height)
		{
			if (x + w > desktop_width)
				x = (desktop_width - w);
			if (y + h > desktop_height)
				y = (desktop_height - h);
		}
	}


	HWND hwnd = CreateWindowEx(
		0,
		MAIN_WINDOW_CLASS_NAME,
		MAIN_WINDOW_CLASS_NAME,
		stylebits,
		x, y, w, h,
		NULL,
		NULL,
		g_wv.hInstance,
		NULL);

	if (!hwnd)
	{
		ri.Error(ERR_FATAL, "create main window() - Couldn't create window");
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	ri.Printf(PRINT_ALL, " %d x %d window created at (%d, %d). \n", w, h, x, y);
	return hwnd;
}



void dx_imp_init( void )
{
	ri.Printf(PRINT_ALL, " Initializing DX12 subsystem \n");

	// Create window.
	SetMode(r_mode->integer, (qboolean)r_fullscreen->integer);

	if (get_render_api() == RENDER_API_DX)
	{
		g_wv.hWnd_dx = create_main_window(glConfig.vidWidth, glConfig.vidHeight, (qboolean)r_fullscreen->integer);
		g_wv.hWnd = g_wv.hWnd_dx;
		// Brings the thread that created the specified window into the foreground 
		// and activates the window. Keyboard input is directed to the window, and
		// various visual cues are changed for the user. 
		// The system assigns a slightly higher priority to the thread that created
		// the foreground window than it does to other threads.
		if (SetForegroundWindow(g_wv.hWnd))
			ri.Printf(PRINT_ALL, " Bring window into the foreground successed. \n");

		// Sets the keyboard focus to the specified window.
		// A handle to the window that will receive the keyboard input.
		// If this parameter is NULL, keystrokes are ignored.
		SetFocus(g_wv.hWnd);
		WG_CheckHardwareGamma();
	}
}


void dx_imp_shutdown(void)
{
	ri.Printf(PRINT_ALL, "Shutting down DX12 subsystem\n");

	if (g_wv.hWnd_dx)
	{
		ri.Printf(PRINT_ALL, "...destroying DX12 window\n");
		DestroyWindow(g_wv.hWnd_dx);

		if (g_wv.hWnd == g_wv.hWnd_dx) {
			g_wv.hWnd = NULL;
		}
		g_wv.hWnd_dx = NULL;
	}

	// For DX12 mode we still have qgl pointers initialized with placeholder values.
	// Reset them the same way as we do in opengl mode.

	WG_RestoreGamma();

	memset(&glConfig, 0, sizeof(glConfig));
	memset(&glState, 0, sizeof(glState));


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