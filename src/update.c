/*
   Desktop Prayer Times app
   Copyright (C) 2021 Ahmet Öztürk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "update.h"
#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define DEBUG

char* update_get_current_version(void)
{
    char* version = malloc(30*sizeof(char));
    if(version) {
        char const*const format = VERSION_MAIN_MINOR < 10 ? "%d.0%d" : "%d.%d";
        sprintf(version, format, VERSION_MAIN_MAJOR, VERSION_MAIN_MINOR);
        version = realloc(version, (strlen(version) + 1)*sizeof(char));
    }
    return version;
}

char* update_get_available_version(void)
{
    char* version_available = NULL;
    char const*const host = "raw.githubusercontent.com";
    char const*const file = "/oeah2/desktop-prayer-times/main/version";

    version_available = https_get(host, file, NULL);
#ifdef DEBUG
    if(version_available)
        puts(version_available);
#endif // DEBUG
    if(version_available && strchr(version_available, '\n')) {
        *strchr(version_available, '\n') = '\0';
    }

    return version_available;
}

bool update_check_is_available(void)
{
    bool ret = false;

    char* version_this = update_get_current_version();
    char* version_available = update_get_available_version();
    if(version_this && version_available && strcmp(version_this, version_available) < 0) {
        // Update available
        ret = true;
    }
    if(version_this) free(version_this);
    if(version_available) free(version_available);
    return ret;
}

