#include "../client/client.h"

/*
** R_GetModeInfo
*/
typedef struct vidmode_s
{
	const char * description;
	int         width, height;
	float		pixelAspect;		// pixel width / height
} vidmode_t;

static const vidmode_t r_vidModes[] =
{
	{ "Mode  0: 320x240",		320,	240,	1 },
	{ "Mode  1: 400x300",		400,	300,	1 },
	{ "Mode  2: 512x384",		512,	384,	1 },
	{ "Mode  3: 640x480",		640,	480,	1 },
	{ "Mode  4: 800x600",		800,	600,	1 },
	{ "Mode  5: 960x720",		960,	720,	1 },
	{ "Mode  6: 1024x768",		1024,	768,	1 },
	{ "Mode  7: 1152x864",		1152,	864,	1 },
	{ "Mode  8: 1280x1024",		1280,	1024,	1 },
	{ "Mode  9: 1600x1200",		1600,	1200,	1 },
	{ "Mode 10: 2048x1536",		2048,	1536,	1 },
	{ "Mode 11: 856x480 (wide)",856,	480,	1 },
	{ "Mode 12: 1280x720",		1280,	720,	1 },
	{ "Mode 13: 1280x768",		1280,	768,	1 },
	{ "Mode 14: 1280x800",		1280,	800,	1 },
	{ "Mode 15: 1280x960",		1280,	960,	1 },
	{ "Mode 16: 1360x768",		1360,	768,	1 },
	{ "Mode 17: 1366x768",		1366,	768,	1 }, // yes there are some out there on that extra 6
	{ "Mode 18: 1360x1024",		1360,	1024,	1 },
	{ "Mode 19: 1400x1050",		1400,	1050,	1 },
	{ "Mode 20: 1400x900",		1400,	900,	1 },
	{ "Mode 21: 1600x900",		1600,	900,	1 },
	{ "Mode 22: 1680x1050",		1680,	1050,	1 },
	{ "Mode 23: 1920x1080",		1920,	1080,	1 },
	{ "Mode 24: 1920x1200",		1920,	1200,	1 },
	{ "Mode 25: 1920x1440",		1920,	1440,	1 },
	{ "Mode 26: 2560x1080",		2560,	1080,	1 },
	{ "Mode 27: 2560x1600",		2560,	1600,	1 },
	{ "Mode 28: 3840x2160 (4K)",3840,	2160,	1 }
};

const static int s_numVidModes = (sizeof(r_vidModes) / sizeof(r_vidModes[0]));


// always returu a valid mode ...
void R_GetModeInfo(int *width, int *height, int mode)
{
	// corse error handle,
	if (mode < 0 || mode >= s_numVidModes)
	{
		// just 640 * 480;
		mode = 3;
	}

	const vidmode_t * const vm = &r_vidModes[mode];

	*width = vm->width;
	*height = vm->height;
}

// ri.Cmd_AddCommand("modelist", R_ModeList_f);
void R_ModeList_f(void)
{
	Com_Printf( "\n" );
	for (int i = 0; i < s_numVidModes; ++i)
	{
		Com_Printf( "%s \n", r_vidModes[i].description);
	}
	Com_Printf( "\n" );
}