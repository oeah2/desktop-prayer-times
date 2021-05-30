#ifndef UPDATE_H_INCLUDED
#define UPDATE_H_INCLUDED

#include <stdbool.h>

enum Version {
    VERSION_MAIN_MAJOR = 0,
    VERSION_MAIN_MINOR = 1,
};

bool update_check_is_available(void);
char* update_get_current_version(void);
char* update_get_available_version(void);

#endif // UPDATE_H_INCLUDED
