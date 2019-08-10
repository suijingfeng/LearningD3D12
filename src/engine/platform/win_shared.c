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
	// The only difference between this function and the timeGetSystemTime 
	// function is that timeGetSystemTime uses the MMTIME structure to return
	// the system time. The timeGetTime function has less overhead than 
	// timeGetSystemTime.
	// Note that the value returned by the timeGetTime function is a DWORD value.
	// The return value wraps around to 0 every 2^32 milliseconds, which is 
	// about 49.71 days. This can cause problems in code that directly uses the 
	// timeGetTime return value in computations, particularly where the value is 
	// used to control code execution. You should always use the difference 
	// between two timeGetTime return values in computations.
	return (timeGetTime() - sys_timeBase);
}



char* Sys_DefaultHomePath(void)
{
	return NULL;
}


char* Sys_DefaultInstallPath(void)
{
	return Sys_Cwd();
}