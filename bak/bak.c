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
