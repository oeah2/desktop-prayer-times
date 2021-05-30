#include "update.h"
#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* update_get_current_version(void) {
    char* version = malloc(30*sizeof(char));
    if(version) {
        sprintf(version, "%d.%d", VERSION_MAIN_MAJOR, VERSION_MAIN_MINOR);
        version = realloc(version, (strlen(version) + 1)*sizeof(char));
    }
    return version;
}

char* update_get_available_version(void) {
    char* version_available = NULL;
    if(version_available) {
        char const*const host = "raw.githubusercontent.com";
        char const*const file = "/oeah2/desktop-prayer-times/main/version?token=AL2O77C4ZEI2TPBFSCUO4H3AGDA2W";

        version_available = http_get(host, file, 0);
    }
    return version_available;
}

bool update_check_is_available(void) {
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

