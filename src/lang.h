#ifndef LANG_H_INCLUDED
#define LANG_H_INCLUDED

#include <stdbool.h>

enum Languages {
    LANG_EN = 0,
    LANG_DE,
    LANG_TR,

    LANG_NUM,
};

bool lang_is_available(enum Languages ID);
char* lang_get_filename(enum Languages ID);

extern char const*const lang_names[LANG_NUM];


#endif // LANG_H_INCLUDED
