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
