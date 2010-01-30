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

char rgl_cwd[512];

int rglReadSettings()
{
    FILE * fp;
    char line[512];
    char section[256];
    char key[256];
    int value;
    int i, ignore = 0;
    char rom[21];

#ifdef WIN32
    if (!rgl_cwd[0]) {
        getcwd(rgl_cwd, 512-32);
        LOG("getcwd = '%s'\n", rgl_cwd);
    }
    sprintf(line, "%s/Plugin/z64gl.conf", rgl_cwd);
    fp = fopen(line, "r");
#else
    char configName[PATH_MAX];

    strcpy(rgl_cwd, ".");
    strcpy(configName, g_ConfigDir);
    strcat(configName, "z64gl.conf");
    fp = fopen(configName, "r");
#endif

    if (!fp) {
        LOGERROR("Couldn't open z64gl config file\n");
        return -1;
    }

    for (i=0; i<20; i++)
        rom[i] = gfx.HEADER[(32+i)^3];
    rom[20] = 0;

    i = strlen(rom);
    while (i > 0 && rom[i-1] == ' ')
        rom[--i] = 0;
    LOG("rom name '%s'\n", rom);

    section[0] = 0;
    while (fgets(line, 255, fp)) {
        int l = strlen(line);
        //char * e;
        if (l > 0 && line[l-1] == '\n') {
            line[l-1] = 0;
            l--;
        }

        if (line[0] == '#')
            continue;
        else if (sscanf(line, "[%21c]", section) == 1) {
            for (i=0; i<21; i++)
                if (section[i] == ']') {
                    section[i] = 0;
                    break;
                }
                LOG("section '%s'\n", section);
                ignore = strcmp(section, rom) && strcmp(section, "override");
        } else if (ignore)
            continue;

        if (sscanf(line , " %s = %d ", key, &value) == 2) {
            LOG("key '%s' value %d\n", key, value);
            if (!strcmp(key, "res_x"))
                rglSettings.resX = value;
            else if (!strcmp(key, "res_y"))
                rglSettings.resY = value;
            else if (!strcmp(key, "fs_res_x"))
                rglSettings.fsResX = value;
            else if (!strcmp(key, "fs_res_y"))
                rglSettings.fsResY = value;
            else if (!strcmp(key, "hires_fb"))
                rglSettings.hiresFb = value;
            else if (!strcmp(key, "fb_info"))
                rglSettings.fbInfo = value;
            else if (!strcmp(key, "force_swap"))
                rglSettings.forceSwap = value;
            else if (!strcmp(key, "threaded"))
                rglSettings.threaded = value;
            else if (!strcmp(key, "async"))
                rglSettings.async = value;
            else if (!strcmp(key, "no_npot_fbos"))
                rglSettings.noNpotFbos = value;
            else if (!strcmp(key, "lowres"))
                rglSettings.lowres = value;
            else
                LOGERROR("Unknown config key '%s'\n", key);
        }
    }

    fclose(fp);

    return 0;
}
