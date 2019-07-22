#include "../renderer/tr_local.h"
#include "../qcommon/qcommon.h"
#include "win_local.h"

// only write information to glConfig, we dont use/read it.

void setWinConfig(int w, int h, bool isFullScreen, int mode, int hz)
{
	glConfig.vidWidth = w;
	glConfig.vidHeight = h;
	glConfig.windowAspect = (float)w / (float) h;
	glConfig.isFullscreen = isFullScreen ? qtrue : qfalse;
	glConfig.stereoEnabled = qfalse;
	glConfig.smpActive = qfalse;
	glConfig.UNUSED_displayFrequency = 120;
	glConfig.deviceSupportsGamma = qtrue;

	// allways enable stencil
	glConfig.stencilBits = 8;
	glConfig.depthBits = 24;
	glConfig.colorBits = 32;
}