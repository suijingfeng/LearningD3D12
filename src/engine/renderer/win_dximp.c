
/*
** WIN_DXIMP.C
**
** This file contains ALL Win32 specific stuff having to do with the
** OpenGL/Vulkan refresh.  When a port is being made the following functions
** must be implemented by the port:
**
**
** vk_imp_init
** vk_imp_shutdown
** vk_imp_create_surface
*/

#include "../renderer/tr_local.h"
#include "../platform/resource.h"
#include "../platform/win_local.h"
#include "../platform/win_gamma.h"
#include <stdio.h>


extern void makeDummyQglProc(void);
extern void SetMode(int mode, qboolean fullscreen);
extern HWND create_main_window(int width, int height, qboolean fullscreen);
extern HWND create_twin_window(int width, int height, RenderApi render_api);
extern void CommonCleanUP(void);

void dx_imp_init(void)
{
	ri.Printf(PRINT_ALL, " Initializing D3D12 subsystem \n");

	// This will set qgl pointers to no-op placeholders.

	makeDummyQglProc();

	// Create window.
	SetMode(r_mode->integer, (qboolean)r_fullscreen->integer);

	if (get_render_api() == RENDER_API_DX)
	{
		g_wv.hWnd_dx = create_main_window(glConfig.vidWidth, glConfig.vidHeight, (qboolean)r_fullscreen->integer);
		g_wv.hWnd = g_wv.hWnd_dx;
		SetForegroundWindow(g_wv.hWnd);
		SetFocus(g_wv.hWnd);
		WG_CheckHardwareGamma();
	}
	else
	{
		g_wv.hWnd_dx = create_twin_window(glConfig.vidWidth, glConfig.vidHeight, RENDER_API_DX);
	}
}


void dx_imp_shutdown()
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
	CommonCleanUP();



	memset(&glConfig, 0, sizeof(glConfig));
	memset(&glState, 0, sizeof(glState));

}