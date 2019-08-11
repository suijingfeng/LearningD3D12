#include "tr_cvar.h"

#include "VKimpl.h"
#include "vk_instance.h"
#include "ref_import.h" 

// have to use this crap for backward Compatibility.
// I want keep it locally, as it belong to OpenGL, not vulkan



void R_SetWinMode(int mode, unsigned int width, unsigned int height, unsigned int hz)
{
	if (mode == -2)
	{
        // use desktop video resolution
        glConfig.vidWidth = width;
        glConfig.vidHeight = height;
        glConfig.windowAspect = (float)width / (float)height;
        glConfig.displayFrequency = hz;
        glConfig.isFullscreen = 1;

        vk.renderArea.offset.x = 0;
        vk.renderArea.offset.y = 0;
        vk.renderArea.extent.width = width;
        vk.renderArea.extent.height = height;
    }
	else if ( mode == -1 )
    {
		glConfig.vidWidth = r_customwidth->integer;
		glConfig.vidHeight = r_customheight->integer;
        glConfig.windowAspect = (float)width / (float)height;
        glConfig.displayFrequency = hz;
        glConfig.isFullscreen = r_fullscreen->integer;

        vk.renderArea.offset.x = 0;
        vk.renderArea.offset.y = 0;
        vk.renderArea.extent.width = r_customwidth->integer;
        vk.renderArea.extent.height = r_customheight->integer;
	}
    else if ( (mode < s_numVidModes) && (mode > 3))
    {
        glConfig.vidWidth = r_vidModes[mode].width;
        glConfig.vidHeight = r_vidModes[mode].height;
        glConfig.windowAspect = (float)r_vidModes[mode].width / ( r_vidModes[mode].height * r_vidModes[mode].pixelAspect );
        glConfig.displayFrequency = hz;
        glConfig.isFullscreen = r_fullscreen->integer = 0;
        
        vk.renderArea.offset.x = 0;
        vk.renderArea.offset.y = 0;
        vk.renderArea.extent.width = glConfig.vidWidth;
        vk.renderArea.extent.height = glConfig.vidHeight;
    }
    

    if ( mode < -2 || mode >= s_numVidModes || glConfig.vidWidth <=0 || glConfig.vidHeight<=0)
    {
        r_mode->integer = 3;
        r_fullscreen->integer = 0;

        glConfig.vidWidth = 640;
        glConfig.vidHeight = 480;
        glConfig.displayFrequency = 60;
        glConfig.windowAspect = 1.333333f;

        vk.renderArea.offset.x = 0;
        vk.renderArea.offset.y = 0;
        vk.renderArea.extent.width = 640;
        vk.renderArea.extent.height = 480;
	}

	ri.Printf(PRINT_ALL,  "MODE: %d, %d x %d, refresh rate: %dhz\n",
        mode, glConfig.vidWidth, glConfig.vidHeight, glConfig.displayFrequency);
}


