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
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "../client/client.h"

//==========================================================================
extern void Sys_QueEvent(int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr);


static const byte s_scantokey[128] =
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
	K_F12,  0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,        // 5
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,        // 6 
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,
	0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0         // 7 
};



/*
=======
MapKey: Map from windows to quake keynums
=======
*/
int MapKey(int key)
{
	qboolean is_extended;

	//	Com_Printf( "0x%x\n", key);

	int modified = (key >> 16) & 255;

	if (modified > 127)
		return 0;

	if (key & (1 << 24))
	{
		is_extended = qtrue;
	}
	else
	{
		is_extended = qfalse;
	}

	int result = s_scantokey[modified];

	if (!is_extended)
	{
		switch (result)
		{
		case K_HOME:
			return K_KP_HOME;
		case K_UPARROW:
			return K_KP_UPARROW;
		case K_PGUP:
			return K_KP_PGUP;
		case K_LEFTARROW:
			return K_KP_LEFTARROW;
		case K_RIGHTARROW:
			return K_KP_RIGHTARROW;
		case K_END:
			return K_KP_END;
		case K_DOWNARROW:
			return K_KP_DOWNARROW;
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
		switch (result)
		{
		case K_PAUSE:
			return K_KP_NUMLOCK;
		case 0x0D:
			return K_KP_ENTER;
		case 0x2F:
			return K_KP_SLASH;
		case 0xAF:
			return K_KP_PLUS;
		}
		return result;
	}
}


void processMouseWheelMsg(unsigned int MsgTime, int zDelta)
{
	if (cls.keyCatchers & KEYCATCH_CONSOLE)
	{
		// 120 increments, might be 240 and multiples if wheel goes too fast
		// NOTE Logitech: logitech drivers are screwed and send the message twice?
		//   could add a cvar to interpret the message as successive press/release events

		int i;
		if (zDelta > 0)
		{
			for (i = 0; i < zDelta; ++i)
			{
				Sys_QueEvent(MsgTime, SE_KEY, K_MWHEELUP, qtrue, 0, NULL);
				Sys_QueEvent(MsgTime, SE_KEY, K_MWHEELUP, qfalse, 0, NULL);
			}
		}
		else
		{
			for (i = 0; i < -zDelta; ++i)
			{
				Sys_QueEvent(MsgTime, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL);
				Sys_QueEvent(MsgTime, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL);
			}
		}
		// when an application processes the WM_MOUSEWHEEL message, it must return zero
	}
}

void VID_AppActivate(void)
{
	Key_ClearStates();	// FIXME!!!
	// we don't want to act like we're active if we're minimized
}