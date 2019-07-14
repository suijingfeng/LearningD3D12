#include "../renderer/tr_local.h"
#include "../platform/win_local.h"
#include "../platform/win_gamma.h"

extern refimport_t ri;
static HINSTANCE vk_library_handle; // HINSTANCE for the Vulkan library

extern void makeDummyQglProc(void);
extern void SetMode(int mode, qboolean fullscreen);
extern HWND create_main_window(int width, int height, qboolean fullscreen);
extern HWND create_twin_window(int width, int height, RenderApi render_api);
extern void CommonCleanUP(void);

void vk_imp_init()
{
	ri.Printf(PRINT_ALL, " Initializing Vulkan subsystem \n");

	// This will set qgl pointers to no-op placeholders.
	makeDummyQglProc();
	// Load Vulkan DLL.
	const char* dll_name = "vulkan-1.dll";

	ri.Printf(PRINT_ALL, "...calling LoadLibrary('%s'): ", dll_name);
	vk_library_handle = LoadLibrary(dll_name);

	if (vk_library_handle == NULL)
	{
		ri.Printf(PRINT_ALL, "failed\n");
		ri.Error(ERR_FATAL, "vk_imp_init - could not load %s\n", dll_name);
	}
	ri.Printf( PRINT_ALL, "succeeded\n" );

	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vk_library_handle, "vkGetInstanceProcAddr");

	// Create window.
	SetMode(r_mode->integer, (qboolean)r_fullscreen->integer);

	if (get_render_api() == RENDER_API_VK)
	{
		g_wv.hWnd_vulkan = create_main_window(glConfig.vidWidth, glConfig.vidHeight, (qboolean)r_fullscreen->integer);
		g_wv.hWnd = g_wv.hWnd_vulkan;
		SetForegroundWindow(g_wv.hWnd);
		SetFocus(g_wv.hWnd);
		WG_CheckHardwareGamma();
	}
	else {
		g_wv.hWnd_vulkan = create_twin_window(glConfig.vidWidth, glConfig.vidHeight, RENDER_API_VK);
	}
}

void vk_imp_shutdown()
{
	ri.Printf(PRINT_ALL, "Shutting down Vulkan subsystem\n");

	if (g_wv.hWnd_vulkan)
	{
		ri.Printf(PRINT_ALL, "...destroying Vulkan window\n");
		DestroyWindow(g_wv.hWnd_vulkan);

		if (g_wv.hWnd == g_wv.hWnd_vulkan) {
			g_wv.hWnd = NULL;
		}
		g_wv.hWnd_vulkan = NULL;
	}

	if (vk_library_handle != NULL) {
		ri.Printf(PRINT_ALL, "...unloading Vulkan DLL\n");
		FreeLibrary(vk_library_handle);
		vk_library_handle = NULL;
	}
	vkGetInstanceProcAddr = nullptr;

	// For vulkan mode we still have qgl pointers initialized with placeholder values.
	// Reset them the same way as we do in opengl mode.

	CommonCleanUP();

	memset(&glConfig, 0, sizeof(glConfig));
	memset(&glState, 0, sizeof(glState));
}


void vk_imp_create_surface()
{
	VkWin32SurfaceCreateInfoKHR desc;
	desc.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	desc.pNext = nullptr;
	desc.flags = 0;
	// This function returns a module handle for the specified module 
	// if the file is mapped into the address space of the calling process.
	desc.hinstance = GetModuleHandle(nullptr);
	desc.hwnd = g_wv.hWnd_vulkan;
	VK_CHECK( vkCreateWin32SurfaceKHR(vk.instance, &desc, nullptr, &vk.surface) );
}
