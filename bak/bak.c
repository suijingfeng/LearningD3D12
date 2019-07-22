#if 0
	// if we find the CD, add a +set cddir xxx command line
	Sys_ScanForCD();
#endif

/*
================
Sys_CheckCD

Return true if the proper CD is in the drive
================
*/
qboolean Sys_CheckCD( void )
{
  // FIXME: mission pack
  return qtrue;
	//return Sys_ScanForCD();
}


/*
================
Sys_ScanForCD

Search all the drives to see if there is a valid CD to grab
the cddir from
================
*/
qboolean Sys_ScanForCD( void )
{
	static char	cddir[MAX_OSPATH];
	char		drive[4];
	FILE		*f;
	char		test[MAX_OSPATH];
#if 0
	// don't override a cdpath on the command line
	if ( strstr( sys_cmdline, "cdpath" ) ) {
		return;
	}
#endif

	drive[0] = 'c';
	drive[1] = ':';
	drive[2] = '\\';
	drive[3] = 0;

	// scan the drives
	for ( drive[0] = 'c' ; drive[0] <= 'z' ; drive[0]++ )
    {
		if ( GetDriveType (drive) != DRIVE_CDROM ) {
			continue;
		}

		sprintf (cddir, "%s%s", drive, CD_BASEDIR);
		sprintf (test, "%s\\%s", cddir, CD_EXE);
		f = fopen( test, "r" );
		if ( f ) {
			fclose (f);
			return qtrue;
    }
    else {
      sprintf(cddir, "%s%s", drive, CD_BASEDIR_LINUX);
      sprintf(test, "%s\\%s", cddir, CD_EXE_LINUX);
  		f = fopen( test, "r" );
	  	if ( f ) {
		  	fclose (f);
			  return qtrue;
      }
    }
	}

	return qfalse;
}

// suijingfeng -- remove strange defines
// message that will be supported by the OS 
// #ifndef WM_MOUSEWHEEL
// #define WM_MOUSEWHEEL (WM_MOUSELAST+1)  
// #endif
//
//

#if defined (_MSC_VER) && (_MSC_VER >= 1200)
// #pragma warning(disable : 4201)
#pragma warning( push )
#endif

#include <windows.h>

#if defined (_MSC_VER) && (_MSC_VER >= 1200)
#pragma warning( pop )
#endif

  1  常用去警告：
 
         #pragma warning(disable:4035) //no return value
         #pragma warning(disable:4068) // unknown pragma
         #pragma warning(disable:4201) //nonstandard extension used : nameless struct/union
         #pragma warning(disable:4267)
         #pragma warning(disable:4018) //signed/unsigned mismatch
         #pragma warning(disable:4127) //conditional expression is constant
         #pragma warning(disable:4146)
         #pragma warning(disable:4244) //conversion from 'LONG_PTR' to 'LONG', possible loss of data
         #pragma warning(disable:4311) //'type cast' : pointer truncation from 'BYTE *' to 'ULONG'
         #pragma warning(disable:4312) //'type cast' : conversion from 'LONG' to 'WNDPROC' of greater size
         #pragma warning(disable:4346) //_It::iterator_category' : dependent name is not a type
         #pragma warning(disable:4786)
         #pragma warning(disable:4541) //'dynamic_cast' used on polymorphic type
         #pragma warning(disable:4996) //declared deprecated ?
         #pragma warning(disable:4200) //zero-sized array in struct/union
         #pragma warning(disable:4800) //forcing value to bool 'true' or 'false' (performance warning)
 
 
 2  常用用法:

        #pragma   warning(   push) 
        #pragma   warning(   disable:XXXX)    //
 
         需要消除警告的代码

        #pragma   warning(   pop   )

 

       or:

        #pragma   warning(disable:XXXX) 

//////////////////////////////////////////////////
static HWND create_twin_window(int width, int height, RenderApi render_api)
{
    //
    // register the window class if necessary
    //
    if (!s_twin_window_class_registered)
    {
        WNDCLASS wc;

        memset( &wc, 0, sizeof( wc ) );

        wc.style         = 0;
        wc.lpfnWndProc   = DefWindowProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = g_wv.hInstance;
        wc.hIcon         = LoadIcon( g_wv.hInstance, MAKEINTRESOURCE(IDI_ICON1));
        wc.hCursor       = LoadCursor (NULL,IDC_ARROW);
        wc.hbrBackground = (HBRUSH) (void *)COLOR_GRAYTEXT;
        wc.lpszMenuName  = 0;
        wc.lpszClassName = TWIN_WINDOW_CLASS_NAME;

        if ( !RegisterClass( &wc ) )
        {
            ri.Error( ERR_FATAL, "create_twin_window: could not register window class" );
        }
		s_twin_window_class_registered = true;
        ri.Printf( PRINT_ALL, "...registered twin window class\n" );
    }

    //
    // compute width and height
    //
    RECT r;
    r.left = 0;
    r.top = 0;
    r.right  = width;
    r.bottom = height;

    int stylebits = WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_SYSMENU;
    AdjustWindowRect (&r, stylebits, FALSE);

    int w = r.right - r.left;
    int h = r.bottom - r.top;

    cvar_t* vid_xpos = ri.Cvar_Get ("vid_xpos", "", 0);
    cvar_t* vid_ypos = ri.Cvar_Get ("vid_ypos", "", 0);
	int x, y;

	bool first_twin_window =
			(get_render_api() != RENDER_API_GL && render_api == RENDER_API_GL) ||
			(get_render_api() == RENDER_API_GL && render_api == RENDER_API_VK);

	if (first_twin_window) {
		x = vid_xpos->integer + width + 5;
		y = vid_ypos->integer;
	}  else {
		x = vid_xpos->integer + 2*width + 10;
		y = vid_ypos->integer;
	}

	int desktop_width = GetDesktopWidth();
    int desktop_height = GetDesktopHeight();

    if (x < 0)
        x = 0;
	else if (x >= desktop_width - 20)
		x = desktop_width - 20;

    if (y < 0)
        y = 0;
	else if (y >= desktop_height - 20)
		y = desktop_height - 20;
	
	char window_name[1024];
	const char* api_name = "invalid-render-api";
	if (render_api == RENDER_API_GL)
		api_name = "OpenGL";
	else if (render_api == RENDER_API_VK)
		api_name = "Vulkan";
	else if (render_api == RENDER_API_DX)
		api_name = "DX12";
	sprintf(window_name, "%s [%s]", MAIN_WINDOW_CLASS_NAME, api_name);

    HWND hwnd = CreateWindowEx(
        0, 
        TWIN_WINDOW_CLASS_NAME,
        window_name,
        stylebits,
        x, y, w, h,
        NULL,
        NULL,
        g_wv.hInstance,
        NULL);

    if (!hwnd)
    {
        ri.Error (ERR_FATAL, "create_twin_window() - Couldn't create window");
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    ri.Printf(PRINT_ALL, "...created twin window@%d,%d (%dx%d)\n", x, y, w, h);
    return hwnd;
}

#if id386 && !( (defined __linux__ || defined __FreeBSD__ ) && (defined __i386__ ) ) // rb010123

long myftol( float f ) {
	static int tmp;
	__asm fld f
	__asm fistp tmp
	__asm mov eax, tmp
}

#endif


static HWND create_twin_window(int width, int height, RenderApi render_api)
{
    //
    // register the window class if necessary
    //
    if (!s_twin_window_class_registered)
    {
        WNDCLASS wc;

        memset( &wc, 0, sizeof( wc ) );

        wc.style         = 0;
        wc.lpfnWndProc   = DefWindowProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = g_wv.hInstance;
        wc.hIcon         = LoadIcon( g_wv.hInstance, MAKEINTRESOURCE(IDI_ICON1));
        wc.hCursor       = LoadCursor (NULL,IDC_ARROW);
        wc.hbrBackground = (HBRUSH) (void *)COLOR_GRAYTEXT;
        wc.lpszMenuName  = 0;
        wc.lpszClassName = TWIN_WINDOW_CLASS_NAME;

        if ( !RegisterClass( &wc ) )
        {
            ri.Error( ERR_FATAL, "create_twin_window: could not register window class" );
        }
		s_twin_window_class_registered = true;
        ri.Printf( PRINT_ALL, "...registered twin window class\n" );
    }

    //
    // compute width and height
    //
    RECT r;
    r.left = 0;
    r.top = 0;
    r.right  = width;
    r.bottom = height;

    int stylebits = WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_SYSMENU;
    AdjustWindowRect (&r, stylebits, FALSE);

    int w = r.right - r.left;
    int h = r.bottom - r.top;

    cvar_t* vid_xpos = ri.Cvar_Get ("vid_xpos", "", 0);
    cvar_t* vid_ypos = ri.Cvar_Get ("vid_ypos", "", 0);
	int x, y;

	bool first_twin_window =
			(get_render_api() != RENDER_API_GL && render_api == RENDER_API_GL);

	if (first_twin_window) {
		x = vid_xpos->integer + width + 5;
		y = vid_ypos->integer;
	}  else {
		x = vid_xpos->integer + 2*width + 10;
		y = vid_ypos->integer;
	}

	int desktop_width = GetDesktopWidth();
    int desktop_height = GetDesktopHeight();

    if (x < 0)
        x = 0;
	else if (x >= desktop_width - 20)
		x = desktop_width - 20;

    if (y < 0)
        y = 0;
	else if (y >= desktop_height - 20)
		y = desktop_height - 20;
	
	char window_name[1024];
	const char* api_name = "invalid-render-api";
	if (render_api == RENDER_API_GL)
		api_name = "OpenGL";
	else if (render_api == RENDER_API_DX)
		api_name = "DX12";
	
	sprintf(window_name, "%s [%s]", MAIN_WINDOW_CLASS_NAME, api_name);

    HWND hwnd = CreateWindowEx(
        0, 
        TWIN_WINDOW_CLASS_NAME,
        window_name,
        stylebits,
        x, y, w, h,
        NULL,
        NULL,
        g_wv.hInstance,
        NULL);

    if (!hwnd)
    {
        ri.Error (ERR_FATAL, "create_twin_window() - Couldn't create window");
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    ri.Printf(PRINT_ALL, "...created twin window@%d,%d (%dx%d)\n", x, y, w, h);
    return hwnd;
}

