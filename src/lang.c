#include "lang.h"
#include <stdlib.h>
#include <stdio.h>

char const*const lang_names[LANG_NUM] = {
    [LANG_EN] = "en",
    [LANG_DE] = "de",
};


bool lang_is_available(enum Languages ID)
{
    bool ret = true;
    return ret;
}

char* lang_get_filename(enum Languages ID)
{
    char* ret = malloc(50 * sizeof(char));
    sprintf(ret, "./gui/%s.glade", lang_names[ID]);
    return ret;
}
