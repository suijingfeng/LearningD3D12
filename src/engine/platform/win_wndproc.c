#include "win_public.h"
#include "../client/client.h"
#include "win_input.h"
#include "win_event.h"
#include "win_snd.h"
#include "win_mode.h"

WinVars_t g_wv;


// Console variables that we need to access from this module
cvar_t* in_forceCharset;// 


static void VID_AppActivate(BOOL fActive, BOOL minimize)
{
	g_wv.isMinimized = minimize;

	Com_DPrintf(" App Activate: %i\n", fActive );

	Key_ClearStates();	// FIXME!!!

	// we don't want to act like we're active if we're minimized
	if (fActive && !g_wv.isMinimized )
	{
		g_wv.activeApp = qtrue;
	}
	else
	{
		g_wv.activeApp = qfalse;
	}

	// minimize/restore mouse-capture on demand
	if (!g_wv.activeApp )
	{
		IN_Activate (qfalse);
	}
	else
	{
		IN_Activate (qtrue);
	}
}

//==========================================================================

const static byte s_scantokey[128] = 
{ 
//  0           1       2       3       4       5       6       7 
//  8           9       A       B       C       D       E       F 
	0  ,    27,     '1',    '2',    '3',    '4',    '5',    '6', 
	'7',    '8',    '9',    '0',    '-',    '=',    K_BACKSPACE, 9, // 0 
	'q',    'w',    'e',    'r',    't',    'y',    'u',    'i', 
	'o',    'p',    '[',    ']',    13 ,    K_CTRL,'a',  's',      // 1 
	'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';', 
	'\'' ,    '`',    K_SHIFT,'\\',  'z',    'x',    'c',    'v',      // 2 
	'b',    'n',    'm',    ',',    '.',    '/',    K_SHIFT,'*', 
	K_ALT,' ',   K_CAPSLOCK  ,    K_F1, K_F2, K_F3, K_F4, K_F5,   // 3 
	K_F6, K_F7, K_F8, K_F9, K_F10,  K_PAUSE,    0  , K_HOME, 
	K_UPARROW, K_PGUP, K_KP_MINUS, K_LEFTARROW, K_KP_5, K_RIGHTARROW, K_KP_PLUS, K_END, //4 
	K_DOWNARROW, K_PGDN, K_INS, K_DEL, 0, 0, 0, K_F11, 
	K_F12,  0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,     // 5
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0, 
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,     // 6 
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0, 
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0      // 7 
}; 

/*
==================
MapKey

Map from windows to quake keynums
==================
*/
static int MapKey( int nVirtKey, int key )
{
	qboolean is_extended;

	int modified = ( key >> 16 ) & 255;

	if ( modified > 127 )
		return 0;

	if ( key & ( 1 << 24 ) )
	{
		is_extended = qtrue;
	}
	else
	{
		is_extended = qfalse;
	}

	int result = s_scantokey[modified];

	//Com_Printf( "key: 0x%08x modified:%i extended:%i result:%i(%02x) vk=%i\n",
	//	key, modified, is_extended, result, result, nVirtKey );

	if ( !is_extended )
	{
		switch ( result )
		{
		case K_HOME:
			return K_KP_HOME;
		case K_UPARROW:
			if ( Key_GetCatcher() && nVirtKey == VK_NUMPAD8 )
				return 0;
			return K_KP_UPARROW;
		case K_DOWNARROW:
			if ( Key_GetCatcher() && nVirtKey == VK_NUMPAD2 )
				return 0;
			return K_KP_DOWNARROW;
		case K_PGUP:
			return K_KP_PGUP;
		case K_LEFTARROW:
			return K_KP_LEFTARROW;
		case K_RIGHTARROW:
			return K_KP_RIGHTARROW;
		case K_END:
			return K_KP_END;
		case K_PGDN:
			return K_KP_PGDN;
		case K_INS:
			return K_KP_INS;
		case K_DEL:
			return K_KP_DEL;
		default:
			return result;
		}
	}
	else
	{
		switch ( result )
		{
		case K_PAUSE:
			return K_KP_NUMLOCK;
		case K_ENTER:
			return K_KP_ENTER;
		case '/':
			return K_KP_SLASH;
		case 0xAF:
			return K_KP_PLUS;
		case '*':
			return K_KP_STAR;
		}
		return result;
	}
}


static qboolean directMap( const WPARAM chr )
{

	if ( !in_forceCharset->integer )
		return qtrue;

	switch ( chr ) // edit control sequences
	{
		case 'c'-'a'+1:
		case 'v'-'a'+1:
		case 'h'-'a'+1:
		case 'a'-'a'+1:
		case 'e'-'a'+1:
		case 'n'-'a'+1:
		case 'p'-'a'+1:
		case 'l'-'a'+1: // CTRL+L
			return qtrue;
	}

	if ( chr < ' ' || chr > 127 || in_forceCharset->integer > 1 )
		return qfalse;
	else
		return qtrue;
}


/*
==================
MapChar

Map input to ASCII charset
==================
*/
static int MapChar( WPARAM wParam, byte scancode ) 
{
	static const int s_scantochar[ 128 ] = 
	{ 
//	0        1       2       3       4       5       6       7 
//	8        9       A       B       C       D       E       F 
 	 0,      0,     '1',    '2',    '3',    '4',    '5',    '6', 
	'7',    '8',    '9',    '0',    '-',    '=',    0x8,    0x9,	// 0
	'q',    'w',    'e',    'r',    't',    'y',    'u',    'i', 
	'o',    'p',    '[',    ']',    0xD,     0,     'a',    's',	// 1
	'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';', 
	'\'',    0,      0,     '\\',   'z',    'x',    'c',    'v',	// 2
	'b',    'n',    'm',    ',',    '.',    '/',     0,     '*', 
	 0,     ' ',     0,      0,      0,      0,      0,      0,     // 3

	 0,      0,     '!',    '@',    '#',    '$',    '%',    '^', 
	'&',    '*',    '(',    ')',    '_',    '+',    0x8,    0x9,	// 4
	'Q',    'W',    'E',    'R',    'T',    'Y',    'U',    'I', 
	'O',    'P',    '{',    '}',    0xD,     0,     'A',    'S',	// 5
	'D',    'F',    'G',    'H',    'J',    'K',    'L',    ':',
	'"',     0,      0,     '|',    'Z',    'X',    'C',    'V',	// 6
	'B',    'N',    'M',    '<',    '>',    '?',     0,     '*', 
 	 0,     ' ',     0,      0,      0,      0,      0,      0,     // 7
	}; 

	if ( scancode == 0x53 )
		return '.';

	if ( directMap( wParam ) || scancode > 0x39 )
	{
		return wParam;
	}
	else 
	{
		char ch = s_scantochar[ scancode ];
		int shift = (GetKeyState( VK_SHIFT ) >> 15) & 1;
		if ( ch >= 'a' && ch <= 'z' ) 
		{
			int  capital = GetKeyState( VK_CAPITAL ) & 1;
			if ( capital ^ shift ) 
			{
				ch = ch - 'a' + 'A';
			}
		} 
		else 
		{
			ch = s_scantochar[ scancode | (shift<<6) ];
		}

		return ch;
	}
}



void ToggleFullscreenWindow(void)
{
	// make sure r_mode left unchanged ? 
	// Use r_mode to save the old window rect so 
	// we can restore it when exiting fullscreen mode.
	
	// Make the window borderless so that the client
	// area can fill the screen.

	if (g_wv.isFullScreen)
	{
		// FullScreen to windowed

		// Restore the window's attributes and size.
		SetWindowLong(g_wv.hWnd, GWL_STYLE, g_wv.m_windowStyle);
		// If you have changed certain window data using SetWindowLong,
		// you must call SetWindowPos for the changes to take effect. 
		// Use the following combination for uFlags: 
		// SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED.


		int width;
		int height;

		cvar_t* win_mode = Cvar_Get("r_mode", "12", 0);

		int mode = R_GetModeInfo(&width, &height, win_mode->integer, g_wv.desktopWidth, g_wv.desktopHeight);
		
		RECT m_windowRect;
		m_windowRect.left = 60;
		m_windowRect.top = 80;
		m_windowRect.right = 60 + width;
		m_windowRect.bottom = 80 + height;

		AdjustWindowRect(&m_windowRect, g_wv.m_windowStyle, FALSE);

		SetWindowPos(
			g_wv.hWnd,
			HWND_NOTOPMOST,
			m_windowRect.left,
			m_windowRect.top,
			m_windowRect.right - m_windowRect.left,
			m_windowRect.bottom - m_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(g_wv.hWnd, SW_NORMAL);


		g_wv.isFullScreen = 0;
	}
	else
	{
		SetWindowLongPtr(g_wv.hWnd, GWL_STYLE,
			g_wv.m_windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));


		// fullscreenWindowRect.left,
		// fullscreenWindowRect.top,
		// fullscreenWindowRect.right,
		// fullscreenWindowRect.bottom,

		// A window can be made a topmost window either 
		// by setting the hWndInsertAfter parameter to HWND_TOPMOST
		// and ensuring that the SWP_NOZORDER flag is not set, 
		// or by setting a window's position in the Z order so that
		// it is above any existing topmost windows. 
		// 
		// When a non-topmost window is made topmost, its owned windows
		// are also made topmost. Its owners, however, are not changed.

		int res = SetWindowPos(g_wv.hWnd, HWND_TOPMOST,
			0, 0, g_wv.desktopWidth, g_wv.desktopHeight,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);


		g_wv.isFullScreen = 1;
		// try the long way if failed
		if (res == 0)
		{
			Cvar_Set("r_fullscreen", "1");
			Cbuf_AddText("vid_restart\n");
		}
		else
		{
			ShowWindow(g_wv.hWnd, SW_MAXIMIZE);
		}
	}
}



/*
====================
MainWndProc

main window procedure
====================
*/
LRESULT WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		// Windows 98/Me, Windows NT 4.0 and later - uses WM_MOUSEWHEEL
		// only relevant for non-DI input and when console is toggled in window mode
		// if console is toggled in window mode (KEYCATCH_CONSOLE) then mouse is released 
		// and DI doesn't see any mouse wheel.
		// comments this allow pull-down console receive mousewheel message,
		// but I don't know does this doing the right thing.
		// !r_fullscreen->integer &&
		if ( ( Key_GetCatcher() & KEYCATCH_CONSOLE))
		{
			// 120 increments, might be 240 and multiples if wheel goes too fast
			// NOTE Logitech: logitech drivers are screwed and send the message twice?
			//   could add a cvar to interpret the message as successive press/release events
			int zDelta = ( short ) HIWORD( wParam ) / WHEEL_DELTA;
	
			if ( zDelta > 0 )
			{
				for(int i=0; i<zDelta; ++i)
				{
					Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
					Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
				}
			}
			else
			{
				for(int i=0; i<-zDelta; ++i)
				{
					Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
					Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
				}
			}
			// when an application processes the WM_MOUSEWHEEL message, it must return zero
			return 0;
		}
		break;

	case WM_CREATE:
	{
		CREATESTRUCT * pCreate = (CREATESTRUCT *)lParam;
		WinVars_t * pWinState = (WinVars_t *)pCreate->lpCreateParams;

		pWinState->hWnd = hWnd;

		in_forceCharset = Cvar_Get("in_forceCharset", "1", CVAR_ARCHIVE);

	} break;

	case WM_DESTROY:
		// let sound and input know about this?
		g_wv.hWnd = NULL;
		break;

	case WM_CLOSE:
		Cbuf_ExecuteText( EXEC_APPEND, "quit" );
		break;

	case WM_ACTIVATE:
		{
			int fActive = LOWORD(wParam);
			int fMinimized = (BOOL) HIWORD(wParam);

			VID_AppActivate( fActive != WA_INACTIVE, fMinimized);
			SNDDMA_Activate();
		}
		break;

	case WM_MOVE:
	{
		if (!g_wv.isFullScreen)
		{
			int xPos = (short) LOWORD(lParam);    // horizontal position 
			int yPos = (short) HIWORD(lParam);    // vertical position 
			RECT r;
			r.left   = 0;
			r.top    = 0;
			r.right  = 1;
			r.bottom = 1;

			int style = GetWindowLong( hWnd, GWL_STYLE );
			AdjustWindowRect( &r, style, FALSE );
			
			if ( g_wv.activeApp )
			{
				IN_Activate (qtrue);
			}
		}
	} break;

// this is complicated because Win32 seems to pack multiple mouse events into
// one update sometimes, so we always check all states and look for events
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	{
		int temp = 0;

		if (wParam & MK_LBUTTON)
			temp |= 1;

		if (wParam & MK_RBUTTON)
			temp |= 2;

		if (wParam & MK_MBUTTON)
			temp |= 4;

		IN_MouseEvent (temp);
	} break;

	case WM_SYSCOMMAND:
	{
		if (wParam == SC_SCREENSAVE)
			return 0;
	} break;

	case WM_SYSKEYDOWN: 
	{
		// Handle ALT+ENTER:
		if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
		{
			ToggleFullscreenWindow();
			return 0;
		}
		// Send all other WM_SYSKEYDOWN messages to the client.
		Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, MapKey( wParam, lParam ), qtrue, 0, NULL );
	} break;

	case WM_KEYDOWN:
		Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, MapKey( wParam, lParam ), qtrue, 0, NULL );
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		//Com_Printf( "^5k-^7 wParam:%08x lParam:%08x\n", wParam, lParam );
		Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, MapKey( wParam, lParam ), qfalse, 0, NULL );
		break;

	case WM_CHAR:
	{
		// Sys_QueEvent( g_wv.sysMsgTime, SE_CHAR, wParam, 0, 0, NULL );
		byte scancode = ((lParam >> 16) & 0xFF);
		if ( wParam != VK_NUMPAD0 && scancode != 0x29 ) {
			Sys_QueEvent( g_wv.sysMsgTime, SE_CHAR, MapChar( wParam, scancode ), 0, 0, NULL );
		}
	} break;
	
	} // end of switch

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
