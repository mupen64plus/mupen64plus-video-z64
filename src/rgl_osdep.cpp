/*
 * z64
 *
 * This program is free software; you can redistribute it and/
 * or modify it under the terms of the GNU General Public Li-
 * cence as published by the Free Software Foundation; either
 * version 2 of the Licence, or any later version.
 *
 * This program is distributed in the hope that it will be use-
 * ful, but WITHOUT ANY WARRANTY; without even the implied war-
 * ranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public Licence for more details.
 *
 * You should have received a copy of the GNU General Public
 * Licence along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 *
**/

#include "rdp.h"
#include "rgl.h"

#include <SDL/SDL.h>

SDL_Surface *sdl_Screen;
int viewportOffset;

/* definitions of pointers to Core video extension functions */
extern ptr_VidExt_Init                  CoreVideo_Init;
extern ptr_VidExt_Quit                  CoreVideo_Quit;
extern ptr_VidExt_ListFullscreenModes   CoreVideo_ListFullscreenModes;
extern ptr_VidExt_SetVideoMode          CoreVideo_SetVideoMode;
extern ptr_VidExt_SetCaption            CoreVideo_SetCaption;
extern ptr_VidExt_ToggleFullScreen      CoreVideo_ToggleFullScreen;
extern ptr_VidExt_GL_GetProcAddress     CoreVideo_GL_GetProcAddress;
extern ptr_VidExt_GL_SetAttribute       CoreVideo_GL_SetAttribute;
extern ptr_VidExt_GL_SwapBuffers        CoreVideo_GL_SwapBuffers;

//int screen_width = 640, screen_height = 480;
int screen_width = 1024, screen_height = 768;
//int screen_width = 320, screen_height = 240;

int viewport_offset;

void rglSwapBuffers()
{
    if (render_callback != NULL)
        render_callback();
    CoreVideo_GL_SwapBuffers();
    return;
}

int rglOpenScreen()
{
    if (rglStatus == RGL_STATUS_WINDOWED) {
        screen_width = rglSettings.resX;
        screen_height = rglSettings.resY;
    } else {
        screen_width = rglSettings.fsResX;
        screen_height = rglSettings.fsResY;
    }

    m64p_video_mode screen_mode = M64VIDEO_WINDOWED;
    if (rglSettings.fullscreen)
        screen_mode = M64VIDEO_FULLSCREEN;

    viewportOffset = 0;

    if (CoreVideo_GL_SetAttribute(M64P_GL_DOUBLEBUFFER, 1) != M64ERR_SUCCESS ||
        CoreVideo_GL_SetAttribute(M64P_GL_BUFFER_SIZE, 32) != M64ERR_SUCCESS ||
        CoreVideo_GL_SetAttribute(M64P_GL_DEPTH_SIZE, 24)  != M64ERR_SUCCESS)
    {
        rdp_log(M64MSG_ERROR, "Could not set video attributes.");
        return 0;
    }

    if (CoreVideo_SetVideoMode(screen_width, screen_height, 32, screen_mode) != M64ERR_SUCCESS)
    {
        rdp_log(M64MSG_ERROR, "Could not set video mode.");
        return 0;
    }

    CoreVideo_SetCaption("Z64gl");

    rdp_init();
    return 1;
}

void rglCloseScreen()
{
    rglClose();
    CoreVideo_Quit();
}

EXPORT void CALL ChangeWindow (void)
{
    if (rglNextStatus == RGL_STATUS_CLOSED || rglStatus == RGL_STATUS_CLOSED)
        return;
    switch (rglStatus) {
    case RGL_STATUS_WINDOWED:
        rglNextStatus = RGL_STATUS_FULLSCREEN;
        break;
    case RGL_STATUS_FULLSCREEN:
        rglNextStatus = RGL_STATUS_WINDOWED;
        break;
    }
    //  wanted_fullscreen = !fullscreen;
    //   rglCloseScreen();
    //   fullscreen = !fullscreen;
    //   rglOpenScreen();
}

