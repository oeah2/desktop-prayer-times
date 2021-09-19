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

