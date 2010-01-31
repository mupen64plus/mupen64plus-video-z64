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

extern ptr_ConfigOpenSection      ConfigOpenSection;
extern ptr_ConfigSetParameter     ConfigSetParameter;
extern ptr_ConfigGetParameter     ConfigGetParameter;
extern ptr_ConfigGetParameterHelp ConfigGetParameterHelp;
extern ptr_ConfigSetDefaultInt    ConfigSetDefaultInt;
extern ptr_ConfigSetDefaultFloat  ConfigSetDefaultFloat;
extern ptr_ConfigSetDefaultBool   ConfigSetDefaultBool;
extern ptr_ConfigSetDefaultString ConfigSetDefaultString;
extern ptr_ConfigGetParamInt      ConfigGetParamInt;
extern ptr_ConfigGetParamFloat    ConfigGetParamFloat;
extern ptr_ConfigGetParamBool     ConfigGetParamBool;
extern ptr_ConfigGetParamString   ConfigGetParamString;

char rgl_cwd[512];

int rglReadSettings()
{

    m64p_handle videoGeneralSection;
    m64p_handle videoZ64Section;
    if (ConfigOpenSection("Video-General", &videoGeneralSection) != M64ERR_SUCCESS ||
    ConfigOpenSection("Video-Z64", &videoZ64Section) != M64ERR_SUCCESS)
    {
        rdp_log(M64MSG_ERROR, "Could not open configuration");
        return false;
    }

    ConfigSetDefaultBool(videoGeneralSection, "Fullscreen", false, "Use fullscreen mode if True, or windowed mode if False");
    ConfigSetDefaultBool(videoZ64Section, "HiResFB", true, "High resolution framebuffer");
    ConfigSetDefaultBool(videoZ64Section, "FBInfo", true, "Use framebuffer info");
    ConfigSetDefaultBool(videoZ64Section, "Threaded", false, "Run RDP on thread");
    ConfigSetDefaultBool(videoZ64Section, "Async", false, "Run RDP asynchronously");
    ConfigSetDefaultBool(videoZ64Section, "NoNpotFbos", false, "Don't use NPOT FBOs (may be needed for older graphics cards)");

    rglSettings.resX = ConfigGetParamInt(videoGeneralSection, "ScreenWidth");
    rglSettings.resY = ConfigGetParamInt(videoGeneralSection, "ScreenHeight");
    rglSettings.fsResX = ConfigGetParamInt(videoGeneralSection, "ScreenWidth");
    rglSettings.fsResY = ConfigGetParamInt(videoGeneralSection, "ScreenHeight");
    rglSettings.fullscreen = ConfigGetParamBool(videoGeneralSection, "Fullscreen");
    rglSettings.hiresFb = ConfigGetParamBool(videoZ64Section, "HiResFB");
    rglSettings.fbInfo = ConfigGetParamBool(videoZ64Section, "FBInfo");
    rglSettings.threaded = ConfigGetParamBool(videoZ64Section, "Threaded");
    rglSettings.async = ConfigGetParamBool(videoZ64Section, "Async");
    rglSettings.noNpotFbos = ConfigGetParamBool(videoZ64Section, "NoNpotFbos");

    return true;
}
