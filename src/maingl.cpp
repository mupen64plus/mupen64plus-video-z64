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
#include "rgl_glut.h"
#include "osal_dynamiclib.h"
#include <SDL/SDL.h>

#define THREADED
#ifdef WIN32
#define THREADED
#endif

GFX_INFO gfx;

char g_ConfigDir[PATH_MAX] = {0};

/* definitions of pointers to Core video extension functions */
ptr_VidExt_Init                  CoreVideo_Init = NULL;
ptr_VidExt_Quit                  CoreVideo_Quit = NULL;
ptr_VidExt_ListFullscreenModes   CoreVideo_ListFullscreenModes = NULL;
ptr_VidExt_SetVideoMode          CoreVideo_SetVideoMode = NULL;
ptr_VidExt_SetCaption            CoreVideo_SetCaption = NULL;
ptr_VidExt_ToggleFullScreen      CoreVideo_ToggleFullScreen = NULL;
ptr_VidExt_GL_GetProcAddress     CoreVideo_GL_GetProcAddress = NULL;
ptr_VidExt_GL_SetAttribute       CoreVideo_GL_SetAttribute = NULL;
ptr_VidExt_GL_SwapBuffers        CoreVideo_GL_SwapBuffers = NULL;

#ifdef THREADED
volatile static int waiting;
SDL_sem * rdpCommandSema;
SDL_sem * rdpCommandCompleteSema;
SDL_Thread * rdpThread;
int rdpThreadFunc(void * dummy)
{
  while (1) {
    SDL_SemWait(rdpCommandSema);
    waiting = 1;
    if (rglNextStatus == RGL_STATUS_CLOSED)
      rglUpdateStatus();
    else
      rdp_process_list();
    if (!rglSettings.async)
      SDL_SemPost(rdpCommandCompleteSema);
#ifndef WIN32
    if (rglStatus == RGL_STATUS_CLOSED) {
      rdpThread = NULL;
      return 0;
    }
#endif
  }
  return 0;
}

void rdpSignalFullSync()
{
  SDL_SemPost(rdpCommandCompleteSema);
}
void rdpWaitFullSync()
{
  SDL_SemWait(rdpCommandCompleteSema);
}

void rdpPostCommand()
{
  int sync = rdp_store_list();
  SDL_SemPost(rdpCommandSema);
  if (!rglSettings.async)
    SDL_SemWait(rdpCommandCompleteSema);
  else if (sync) {
    rdpWaitFullSync();
    *gfx.MI_INTR_REG |= 0x20;
    gfx.CheckInterrupts();
  }
  
  waiting = 0;
}

void rdpCreateThread()
{
  if (!rdpCommandSema) {
    rdpCommandSema = SDL_CreateSemaphore(0);
    rdpCommandCompleteSema = SDL_CreateSemaphore(0);
  }
  if (!rdpThread) {
    LOG("Creating rdp thread\n");
    rdpThread = SDL_CreateThread(rdpThreadFunc, 0);
  }
}
#endif

#ifdef __cplusplus
extern "C" {
#endif


//EXPORT void CALL CaptureScreen ( char * Directory )
//{
//}

//EXPORT void CALL CloseDLL (void)
//{
//}
//
//EXPORT void CALL DllAbout ( HWND hParent )
//{
//}
//
//EXPORT void CALL DllConfig ( HWND hParent )
//{
//}
//
//EXPORT void CALL DllTest ( HWND hParent )
//{
//}

EXPORT m64p_error CALL PluginStartup(m64p_dynlib_handle CoreLibHandle, void *Context,
                                   void (*DebugCallback)(void *, int, const char *))
{
    /* Get the core Video Extension function pointers from the library handle */
    CoreVideo_Init = (ptr_VidExt_Init) osal_dynlib_getproc(CoreLibHandle, "VidExt_Init");
    CoreVideo_Quit = (ptr_VidExt_Quit) osal_dynlib_getproc(CoreLibHandle, "VidExt_Quit");
    CoreVideo_ListFullscreenModes = (ptr_VidExt_ListFullscreenModes) osal_dynlib_getproc(CoreLibHandle, "VidExt_ListFullscreenModes");
    CoreVideo_SetVideoMode = (ptr_VidExt_SetVideoMode) osal_dynlib_getproc(CoreLibHandle, "VidExt_SetVideoMode");
    CoreVideo_SetCaption = (ptr_VidExt_SetCaption) osal_dynlib_getproc(CoreLibHandle, "VidExt_SetCaption");
    CoreVideo_ToggleFullScreen = (ptr_VidExt_ToggleFullScreen) osal_dynlib_getproc(CoreLibHandle, "VidExt_ToggleFullScreen");
    CoreVideo_GL_GetProcAddress = (ptr_VidExt_GL_GetProcAddress) osal_dynlib_getproc(CoreLibHandle, "VidExt_GL_GetProcAddress");
    CoreVideo_GL_SetAttribute = (ptr_VidExt_GL_SetAttribute) osal_dynlib_getproc(CoreLibHandle, "VidExt_GL_SetAttribute");
    CoreVideo_GL_SwapBuffers = (ptr_VidExt_GL_SwapBuffers) osal_dynlib_getproc(CoreLibHandle, "VidExt_GL_SwapBuffers");

    if (!CoreVideo_Init || !CoreVideo_Quit || !CoreVideo_ListFullscreenModes || !CoreVideo_SetVideoMode ||
        !CoreVideo_SetCaption || !CoreVideo_ToggleFullScreen || !CoreVideo_GL_GetProcAddress ||
        !CoreVideo_GL_SetAttribute || !CoreVideo_GL_SwapBuffers)
    {
        printf("Couldn't connect to Core video functions");
        return M64ERR_INCOMPATIBLE;
    }
    CoreVideo_Init();
    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginShutdown(void)
{
    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion, int *APIVersion, const char **PluginNamePtr, int *Capabilities)
{
    /* set version info */
    if (PluginType != NULL)
        *PluginType = M64PLUGIN_GFX;

    if (PluginVersion != NULL)
        *PluginVersion = 0x016300;

    if (APIVersion != NULL)
        *APIVersion = PLUGIN_API_VERSION;
    
    if (PluginNamePtr != NULL)
        *PluginNamePtr = "Z64gl";

    if (Capabilities != NULL)
    {
        *Capabilities = 0;
    }
                    
    return M64ERR_SUCCESS;
}

EXPORT void CALL SetRenderingCallback(void (*callback)())
{
}

EXPORT void CALL ReadScreen(void **dest, long *width, long *height)
{
  LOG("ReadScreen\n");

  *width = rglSettings.resX;
  *height = rglSettings.resY;
}

EXPORT void CALL DrawScreen (void)
{
}

//EXPORT void CALL GetDllInfo ( PLUGIN_INFO * PluginInfo )
//{
//  PluginInfo->Version = 0x0103;
//  PluginInfo->Type  = PLUGIN_TYPE_GFX;
//  sprintf (PluginInfo->Name, "z64gl");
//  
//  // If DLL supports memory these memory options then set them to TRUE or FALSE
//  //  if it does not support it
//  PluginInfo->NormalMemory = TRUE;  // a normal BYTE array
//  PluginInfo->MemoryBswaped = TRUE; // a normal BYTE array where the memory has been pre
//}


EXPORT int CALL InitiateGFX (GFX_INFO Gfx_Info)
{
  LOG("InitiateGFX\n");
  gfx = Gfx_Info;
  memset(rdpTiles, 0, sizeof(rdpTiles));
  memset(rdpTmem, 0, 0x1000);
  memset(&rdpState, 0, sizeof(rdpState));
#ifdef THREADED
  rglSettings.threaded = 0;
  rglSettings.async = 0;
  rglReadSettings();
  if (rglSettings.threaded)
    rdpCreateThread();
#endif
  return true;
}

EXPORT void CALL MoveScreen (int xpos, int ypos)
{
}

EXPORT void CALL ChangeWindow()
{
}

EXPORT void CALL ProcessDList(void)
{
}

static void glut_rdp_process_list()
{
  rdp_process_list();
}

EXPORT void CALL ProcessRDPList(void)
{
#ifdef RGL_USE_GLUT
  rglGlutPostCommand(glut_rdp_process_list);
#else
#ifdef THREADED
  if (rglSettings.threaded) {
    rdpCreateThread();
    rdpPostCommand();
  } else
#endif
  {
    rdp_process_list();
  }
#endif
  
  return;
}

EXPORT void CALL RomClosed (void)
{
#ifdef THREADED
  if (rglSettings.threaded) {
#ifdef WIN32
    fflush(stdout); fflush(stderr);
    //while (waiting); // temporary hack
#endif
    rglNextStatus = RGL_STATUS_CLOSED;
#ifndef WIN32
    do
      rdpPostCommand();
    while (rglStatus != RGL_STATUS_CLOSED);
#else
    void rglWin32Windowed();
    rglWin32Windowed();
#endif
  } else
#endif
  {
    asm("int $3");
    rglNextStatus = rglStatus = RGL_STATUS_CLOSED;
    rglCloseScreen();
  }
}

EXPORT int CALL RomOpen()
{
#ifdef THREADED
  if (rglSettings.threaded) {
    rdpCreateThread();
    //while (rglStatus != RGL_STATUS_CLOSED);
    rglNextStatus = RGL_STATUS_WINDOWED;
  }
  else
#endif
  {
    rglNextStatus = rglStatus = RGL_STATUS_WINDOWED;
    rglOpenScreen();
  }
  return true;
}

EXPORT void CALL ShowCFB (void)
{
}

EXPORT void CALL UpdateScreen (void)
{
#ifdef THREADED
  if (rglSettings.threaded) {
    rdpPostCommand();
  } else
#endif
  {
    rglUpdate();
  }
}

/******************************************************************
   NOTE: THIS HAS BEEN ADDED FOR MUPEN64PLUS AND IS NOT PART OF THE
         ORIGINAL SPEC
  Function: SetConfigDir
  Purpose:  To pass the location where config files should be read/
            written to.
  input:    path to config directory
  output:   none
*******************************************************************/
EXPORT void CALL SetConfigDir(char *configDir)
{
    strncpy(g_ConfigDir, configDir, PATH_MAX);
}


EXPORT void CALL ViStatusChanged (void)
{
}

EXPORT void CALL ViWidthChanged (void)
{
}

#ifdef __cplusplus
}
#endif

