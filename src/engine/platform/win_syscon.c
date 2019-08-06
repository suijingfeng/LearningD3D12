/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// win_syscon.h
#include "../client/client.h"
#include "win_public.h"
#include "win_sysconsole.h"
#include "resource.h"
#include "win_event.h"



extern WinVars_t g_wv;

#define COPY_ID			1
#define QUIT_ID			2
#define CLEAR_ID		3

#define ERRORBOX_ID		10
#define ERRORTEXT_ID	11

#define EDIT_ID			100
#define INPUT_ID		101

#define EDIT_COLOR	RGB(0x00,0x00,0x10)
#define TEXT_COLOR	RGB(0x40,0xEE,0x20)

#define ERROR_BG_COLOR	RGB(0x90,0x80,0x80)

#define ERROR_COLOR_1   RGB(0xFF,0xFF,0x00)
#define ERROR_COLOR_2   RGB(0xF0,0x00,0x00)

field_t console;

typedef struct
{
	HWND		hWnd;
	HWND		hwndBuffer;

	HWND		hwndInputLine;

	HWND		hwndButtonClear;
	HWND		hwndButtonCopy;

	HWND		hwndErrorBox;

	HBRUSH		hbrEditBackground;
	HBRUSH		hbrErrorBackground;

	HFONT		hfBufferFont;
	HFONT		hfStatusFont;

	char		consoleText[512];
	char		returnedText[512];

	int		visLevel;
	qboolean	quitOnClose;
	int		windowWidth, windowHeight;

	WNDPROC		SysInputLineWndProc;

} WinConData;

static WinConData s_console_window;

static int maxConSize; // up to MAX_CONSIZE
static int curConSize; // up to MAX_CONSIZE

static void ConClear( void ) 
{
	//SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
	//SendMessage( s_wcd.hwndBuffer, EM_REPLACESEL, FALSE, ( LPARAM ) "" );
	SetWindowText( s_console_window.hwndBuffer, "" );
	UpdateWindow( s_console_window.hwndBuffer );
	// s_console_window.newline = qfalse;
	curConSize = 0;
	// conBufPos = 0;
}

static LRESULT WINAPI ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	char *cmdString;
	static int s_timePolarity;

	switch ( uMsg )
	{
	// Sent to a window after it has gained the keyboard focus.
	case WM_SETFOCUS:
		if ( s_console_window.hwndInputLine ) 
		{
			SetFocus( s_console_window.hwndInputLine );
		}
		break;

	case WM_ACTIVATE:
	{
		if ( com_viewlog && ( com_dedicated && !com_dedicated->integer ) )
		{
			// if the viewlog is open, check to see if it's being minimized
			if ( com_viewlog->integer == 1 )
			{
				if ( HIWORD( wParam ) )		// minimized flag
				{
					Cvar_Set( "viewlog", "2" );
				}
			}
			else if ( com_viewlog->integer == 2 )
			{
				if ( !HIWORD( wParam ) )		// minimized flag
				{
					Cvar_Set( "viewlog", "1" );
				}
			}
		}
	} break;

	case WM_CLOSE:
		if ( com_dedicated && com_dedicated->integer && !com_errorEntered )
		{
			cmdString = CopyString( "quit" );
			Sys_QueEvent( 0, SE_CONSOLE, 0, 0, (int)strlen( cmdString ) + 1, cmdString );
		}
		else if ( s_console_window.quitOnClose )
		{
			PostQuitMessage( 0 );
		}
		else
		{
			Sys_ShowConsole( 0, qfalse );
			Cvar_Set( "viewlog", "0" );
		}
		return 0;
	// A static control, or an edit control that is read-only or disabled, 
	// sends the WM_CTLCOLORSTATIC message to its parent window when the 
	// control is about to be drawn. By responding to this message, the 
	// parent window can use the specified device context handle to set 
	// the text foreground and background colors of the static control.
	case WM_CTLCOLORSTATIC:
		if ( ( HWND ) lParam == s_console_window.hwndBuffer )
		{
			SetBkColor( ( HDC ) wParam, EDIT_COLOR );
			SetTextColor( ( HDC ) wParam, TEXT_COLOR );
			return ( LRESULT ) s_console_window.hbrEditBackground;
		}
		else if ( ( HWND ) lParam == s_console_window.hwndErrorBox )
		{
			if ( s_timePolarity )
			{
				SetBkColor( ( HDC ) wParam, ERROR_BG_COLOR );
				SetTextColor( ( HDC ) wParam, ERROR_COLOR_1 );
			}
			else
			{
				SetBkColor( ( HDC ) wParam, ERROR_BG_COLOR );
				SetTextColor( ( HDC ) wParam, ERROR_COLOR_2 );
			}
			return ( LRESULT ) s_console_window.hbrErrorBackground;
		}
		break;

	case WM_CREATE:
		s_console_window.hbrEditBackground = CreateSolidBrush( EDIT_COLOR );
		s_console_window.hbrErrorBackground = CreateSolidBrush( RGB( 0x80, 0x80, 0x80 ) );
		// Creates a timer with the specified time-out value.
		// 
		SetTimer( hWnd, 1, 1000, NULL );
		break;
	case WM_COMMAND:
		if ( wParam == COPY_ID )
		{
			SendMessage( s_console_window.hwndBuffer, EM_SETSEL, 0, -1 );
			SendMessage( s_console_window.hwndBuffer, WM_COPY, 0, 0 );
		}
		else if ( wParam == QUIT_ID )
		{
			if ( s_console_window.quitOnClose )
			{
				PostQuitMessage( 0 );
			}
			else
			{
				cmdString = CopyString( "quit" );
				Sys_QueEvent( 0, SE_CONSOLE, 0, 0, (int)strlen( cmdString ) + 1, cmdString );
			}
		}
		else if ( wParam == CLEAR_ID )
		{
			// EM_SETSEL: Selects a range of characters in an edit control. 
			// You can send this message to either an edit control
			// or a rich edit control.
			// If the start is 0 and the end is -1, all the text 
			// in the edit control is selected.
			SendMessage( s_console_window.hwndBuffer, EM_SETSEL, 0, -1 );
			// EM_REPLACESEL: Replaces the selected text in an edit control
			// or a rich edit control with the specified text.
			// wParam: Specifies whether the replacement operation can be undone.
			// If this is TRUE, the operation can be undone.If this is FALSE, 
			// the operation cannot be undone.
			// lParam: A pointer to a null - terminated string containing the
			// replacement text.
			SendMessage( s_console_window.hwndBuffer, EM_REPLACESEL, FALSE, ( LPARAM ) "" );
			UpdateWindow( s_console_window.hwndBuffer );
		}
		break;

	case WM_TIMER:
		if ( wParam == 1 )
		{
			s_timePolarity = !s_timePolarity;
			if ( s_console_window.hwndErrorBox )
			{
				InvalidateRect( s_console_window.hwndErrorBox, NULL, FALSE );
			}
		}
		break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}



static LRESULT WINAPI InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char inputBuffer[MAX_EDIT_LINE];

	switch ( uMsg )
	{
	// Sent to a window immediately before it loses the keyboard focus.
	case WM_KILLFOCUS:
		if ( (HWND)wParam == s_console_window.hwndBuffer )
		{
			SetFocus( s_console_window.hwndInputLine );
			return 0;
		}
		break;

	case WM_MOUSEWHEEL:
	{
		int zDelta = (short) HIWORD( wParam ) / WHEEL_DELTA;
		if ( zDelta )
		{
			WPARAM scrollMsg;
			int fwKeys = LOWORD( wParam );
			if ( zDelta > 0 )
			{
				if ( fwKeys & MK_CONTROL )
					scrollMsg = SB_PAGEUP;
				else
					scrollMsg = SB_LINEUP;
			}
			else
			{
				zDelta = -zDelta;
				if ( fwKeys & MK_CONTROL )
					scrollMsg = SB_PAGEDOWN;
				else
					scrollMsg = SB_LINEDOWN;
			}
			for (int i = 0; i < zDelta; i++ ) {
				SendMessage( s_console_window.hwndBuffer, EM_SCROLL, scrollMsg, 0 );
			}
			return 0;
		}
	} break;

	case WM_KEYDOWN:
	{
		if ( wParam == 'L' && ( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 || GetAsyncKeyState( VK_RCONTROL ) & 0x8000 ) ) {
			ConClear();
			return 0;
		}

		if ( wParam == VK_PRIOR ) {
			if ( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 || GetAsyncKeyState( VK_RCONTROL ) & 0x8000 )
				SendMessage( s_console_window.hwndBuffer, EM_SCROLL, (WPARAM)SB_PAGEUP, 0 );
			else
				SendMessage( s_console_window.hwndBuffer, EM_SCROLL, (WPARAM)SB_LINEUP, 0 );
			return 0;
		}

		if ( wParam == VK_NEXT ) {
			if ( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 || GetAsyncKeyState( VK_RCONTROL ) & 0x8000 )
				SendMessage( s_console_window.hwndBuffer, EM_SCROLL, (WPARAM)SB_PAGEDOWN, 0 );
			else
				SendMessage( s_console_window.hwndBuffer, EM_SCROLL, (WPARAM)SB_LINEDOWN, 0 );
			return 0;
		}

		if ( wParam == VK_UP ) {
			// Con_HistoryGetPrev( &console );
			SetWindowText( hWnd, AtoW( console.buffer ) );
			SendMessage( hWnd, EM_SETSEL, (WPARAM) console.cursor, console.cursor );
			return 0;
		}

		if ( wParam == VK_DOWN ) {
			// Con_HistoryGetNext( &console );
			SetWindowText( hWnd, AtoW( console.buffer ) );
			SendMessage( hWnd, EM_SETSEL, (WPARAM) console.cursor, console.cursor );
			return 0;
		}

		break;
	}

	case WM_CHAR:
		if ( wParam > 255 )
			return 0;
		if ( wParam == VK_RETURN )
		{
			GetWindowText( s_console_window.hwndInputLine, inputBuffer, sizeof( inputBuffer ) );

			strncat( s_console_window.consoleText, inputBuffer, 
				sizeof( s_console_window.consoleText ) - (int)strlen( s_console_window.consoleText ) - 5 );
			strcat( s_console_window.consoleText, "\n" );

			SetWindowText( s_console_window.hwndInputLine, "" );
			Field_Clear( &console );

			Sys_Print( va( "]%s\n", WtoA( inputBuffer ) ) );

			return 0;
		}

		if ( wParam == VK_TAB )
		{
			DWORD pos;

			GetWindowText( hWnd, inputBuffer, sizeof( inputBuffer ) );
			Q_strncpyz( console.buffer, WtoA( inputBuffer ), sizeof( console.buffer ) );
			SendMessage( hWnd, EM_GETSEL, (WPARAM) &pos, (LPARAM) 0 );
			console.cursor = pos;
			
			Field_CompleteCommand( &console );

			SetWindowText( hWnd, AtoW( console.buffer ) );
			SendMessage( hWnd, EM_SETSEL, console.cursor, console.cursor );
			return 0;
		}
		break;

	case WM_CONTEXTMENU:
		return 0;
	}

	// Passes message information to the specified window procedure.
	return CallWindowProc( s_console_window.SysInputLineWndProc, hWnd, uMsg, wParam, lParam );
}


void Sys_CreateConsole( void )
{	
	const char * const pDEDCLASS = "Console";
	int widths[2] = { 140, -1 };
	int borders[3];
	const int DEDSTYLE = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	
	WNDCLASS wc;

	wc.style         = 0;
	wc.lpfnWndProc   = ConWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = g_wv.hInstance;
	wc.hIcon         = LoadIcon( g_wv.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor       = LoadCursor (NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(LRESULT)COLOR_WINDOW;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = pDEDCLASS;

	if ( !RegisterClass (&wc) )
		return;

	RECT rect;
	rect.left = 0;
	rect.right = 800;
	rect.top = 0;
	rect.bottom = 600;

	// Calculates the required size of the window rectangle, 
	// based on the desired client rectangle size.
	// The window rectangle can then be passed to the CreateWindow function
	// to create a window whose client area is the desired size.
	AdjustWindowRect( &rect, DEDSTYLE, FALSE );
/*
	// Retrieves a handle to the desktop window. The desktop window covers the entire screen. 
	// The desktop window is the area on top of which other windows are painted.

	HDC hDC = GetDC( GetDesktopWindow() );
	int swidth = GetDeviceCaps( hDC, HORZRES );
	int sheight = GetDeviceCaps( hDC, VERTRES );
	ReleaseDC( GetDesktopWindow(), hDC );
*/
	// well, g_wv.hInstance get initialed before 
	s_console_window.hWnd = CreateWindow(
		pDEDCLASS, "Console Arena", DEDSTYLE,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
		NULL, NULL, g_wv.hInstance, NULL );

	if ( s_console_window.hWnd == NULL )
	{
		return;
	}

	//
	// create fonts
	//
	// The GetDC function retrieves a handle to a device context (DC) 
	// for the client area of a specified window or for the entire screen. 
	// You can use the returned handle in subsequent GDI functions to draw in the DC.
	// The device context is an opaque data structure, whose values are used internally by GDI.
	HDC hDC = GetDC( s_console_window.hWnd );

	// Multiplies two 32-bit values and then divides the 64-bit result by a third 32-bit value.
	// The final result is rounded to the nearest integer.
	// The GetDeviceCaps function retrieves device-specific information for the specified device.
	int nHeight = -MulDiv( 8, GetDeviceCaps( hDC, LOGPIXELSY), 72);

	s_console_window.hfBufferFont = CreateFont( nHeight, 0,
		0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_MODERN | FIXED_PITCH,
		"Courier New" );

	rect.left += borders[1];
	rect.right -= borders[1];
	//
	// create the input line
	//
	s_console_window.hwndInputLine = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_LEFT | ES_AUTOHSCROLL,
		10, 515, 730, 20,
		s_console_window.hWnd,
		(HMENU)INPUT_ID,	// child window ID
		g_wv.hInstance, NULL);

	SendMessage(s_console_window.hwndInputLine, WM_SETFONT, (WPARAM)s_console_window.hfBufferFont, 0);

	//
	// create the scrollbuffer
	//
	s_console_window.hwndBuffer = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
		ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
		10, 10, 775, 500,
		s_console_window.hWnd,
		(HMENU)EDIT_ID,	// child window ID
		g_wv.hInstance, NULL);
	SendMessage(s_console_window.hwndBuffer, WM_SETFONT, (WPARAM)s_console_window.hfBufferFont, 0);


	//
	// create the buttons
	//
	s_console_window.hwndButtonCopy = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
												10, 540, 72, 24,
												s_console_window.hWnd, 
												( HMENU ) COPY_ID,	// child window ID
												g_wv.hInstance, NULL );
	SendMessage( s_console_window.hwndButtonCopy, WM_SETTEXT, 0, ( LPARAM ) "copy" );

	s_console_window.hwndButtonClear = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
												90, 540, 72, 24,
												s_console_window.hWnd, 
												( HMENU ) CLEAR_ID,	// child window ID
												g_wv.hInstance, NULL );
	SendMessage( s_console_window.hwndButtonClear, WM_SETTEXT, 0, ( LPARAM ) "clear" );



	// To write code that is compatible with both 32 bit and 64 bit 
	// versions of Windows, use SetWindowLongPtr. When compiling for
	// 32 bit Windows, SetWindowLongPtr is defined as a call to the 
	// SetWindowLong function.
	//
	// Changes an attribute of the specified window. The function also
	// sets a value at the specified offset in the extra window memory.
	// 
	// A handle to the window and, indirectly, the class to which the 
	// window belongs. The SetWindowLongPtr function fails if the process
	// that owns the window specified by the hWnd parameter is at a higher
	// process privilege in the UIPI hierarchy than the process the calling
	// thread resides in.

	// GWLP_WNDPROC: Sets a new address for the window procedure.
	// dwNewLong: The replacement value.
	s_console_window.SysInputLineWndProc = (WNDPROC)SetWindowLongPtr( 
		s_console_window.hwndInputLine, GWLP_WNDPROC, (LONG_PTR)InputLineWndProc);


	ShowWindow( s_console_window.hWnd, SW_SHOWDEFAULT);
	UpdateWindow( s_console_window.hWnd );
	
	SetForegroundWindow( s_console_window.hWnd );
	
	SetFocus( s_console_window.hwndInputLine );

	s_console_window.visLevel = 1;
}


void Sys_DestroyConsole( void )
{
	if ( s_console_window.hWnd )
	{
		ShowWindow( s_console_window.hWnd, SW_HIDE );
		CloseWindow( s_console_window.hWnd );
		DestroyWindow( s_console_window.hWnd );
		s_console_window.hWnd = NULL;
	}
}


void Sys_ShowConsole( int visLevel, qboolean quitOnClose )
{
	s_console_window.quitOnClose = quitOnClose;

	if ( visLevel == s_console_window.visLevel )
	{
		return;
	}

	s_console_window.visLevel = visLevel;

	if ( !s_console_window.hWnd )
		return;

	switch ( visLevel )
	{
	case 0:
		ShowWindow( s_console_window.hWnd, SW_HIDE );
		break;
	case 1:
		ShowWindow( s_console_window.hWnd, SW_SHOWNORMAL );
		curConSize = GetWindowTextLength( s_console_window.hwndBuffer );
		// Sends the specified message to a window or windows. 
		// The SendMessage function calls the window procedure
		// for the specified window and does not return until 
		// the window procedure has processed the message.
		SendMessage( s_console_window.hwndBuffer, EM_SETSEL, curConSize, curConSize );
		SendMessage( s_console_window.hwndBuffer, EM_SCROLLCARET, 0, 0 );
		//SendMessage( s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
		break;
	case 2:
		ShowWindow( s_console_window.hWnd, SW_MINIMIZE );
		break;
	default:
		Sys_Error( "Invalid visLevel %d sent to Sys_ShowConsole\n", visLevel );
		break;
	}
}




char* Sys_ConsoleInput( void )
{
	if ( s_console_window.consoleText[0] == '\0' )
	{
		return NULL;
	}

	strcpy( s_console_window.returnedText, s_console_window.consoleText );
	s_console_window.consoleText[0] = '\0';

	return s_console_window.returnedText;
}



void Sys_Print( const char *pMsg )
{
#define CONSOLE_BUFFER_SIZE		16384

	char buffer[CONSOLE_BUFFER_SIZE*2];
	char *b = buffer;
	const char *msg;
	int bufLen;
	int i = 0;
	static unsigned long s_totalChars;

	//
	// if the message is REALLY long, use just the last portion of it
	//
	if( (int)strlen( pMsg ) > CONSOLE_BUFFER_SIZE - 1 )
	{
		msg = pMsg + (int)strlen( pMsg ) - CONSOLE_BUFFER_SIZE + 1;
	}
	else
	{
		msg = pMsg;
	}

	//
	// copy into an intermediate buffer
	//
	while( msg[i] && ( ( b - buffer ) < sizeof( buffer ) - 1 ) )
	{
		if( msg[i] == '\n' && msg[i+1] == '\r' )
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
			i++;
		}
		else if( msg[i] == '\r' )
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		}
		else if( msg[i] == '\n' )
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		}
		else if( Q_IsColorString( &msg[i] ) )
		{
			i++;
		}
		else
		{
			*b= msg[i];
			b++;
		}
		++i;
	}
	*b = 0;
	bufLen = b - buffer;

	s_totalChars += bufLen;

	//
	// replace selection instead of appending if we're overflowing
	//
	if ( s_totalChars > 0x7fff )
	{
		SendMessage( s_console_window.hwndBuffer, EM_SETSEL, 0, -1 );
		s_totalChars = bufLen;
	}

	//
	// put this text into the windows console
	//
	SendMessage( s_console_window.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
	SendMessage( s_console_window.hwndBuffer, EM_SCROLLCARET, 0, 0 );
	SendMessage( s_console_window.hwndBuffer, EM_REPLACESEL, 0, (LPARAM) buffer );

#undef CONSOLE_BUFFER_SIZE
}




void Sys_SetErrorText( const char *buf )
{
	char errorString[80];
	Q_strncpyz( errorString, buf, sizeof( errorString ) );

	if ( !s_console_window.hwndErrorBox )
	{
		s_console_window.hwndErrorBox = CreateWindow( "static", NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN,
													6, 5, 526, 30,
													s_console_window.hWnd, 
													( HMENU ) ERRORBOX_ID,	// child window ID
													g_wv.hInstance, NULL );
		SendMessage( s_console_window.hwndErrorBox, WM_SETFONT, ( WPARAM ) s_console_window.hfBufferFont, 0 );
		SetWindowText( s_console_window.hwndErrorBox, errorString );

		DestroyWindow( s_console_window.hwndInputLine );
		s_console_window.hwndInputLine = NULL;
	}
}
