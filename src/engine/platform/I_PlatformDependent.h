#pragma once

/*
====================================================================

IMPLEMENTATION SPECIFIC FUNCTIONS

====================================================================
*/

void GLimp_Init( glconfig_t * const pConfig, void ** pContext );
void GLimp_Shutdown( void );
void GLimp_EndFrame( void );
void GLimp_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
void FNimp_Log(char * const pComment);

// NOTE TTimo linux works with float gamma value, not the gamma table
// the params won't be used, getting the r_gamma cvar directly
