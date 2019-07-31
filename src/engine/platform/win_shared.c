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

#include "win_public.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <conio.h>


#include "../../game/q_shared.h"
#include "../qcommon/qcommon.h"



int Sys_Milliseconds (void)
{
	static qboolean	initialized = qfalse;
	static int sys_timeBase;

	if (qfalse == initialized)
	{
		initialized = qtrue;
		
		sys_timeBase = timeGetTime();
	}

	return (timeGetTime() - sys_timeBase);
}


char* Sys_GetCurrentUser( void )
{
	static char s_userName[1024];
	unsigned long size = sizeof( s_userName );


	if ( !GetUserName( s_userName, &size ) )
		strcpy( s_userName, "player" );

	if ( !s_userName[0] )
	{
		strcpy( s_userName, "player" );
	}

	return s_userName;
}


char* Sys_DefaultHomePath(void)
{
	return NULL;
}


char* Sys_DefaultInstallPath(void)
{
	return Sys_Cwd();
}