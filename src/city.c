#include "city.h"

City* city_init_diyanet(City* c, char const*const name, size_t pr_time_provider, char const*const filename, size_t id)
{
    if(c && name && filename) {
        c->file_times = fopen(filename, "r");
        if(c->file_times) {
            c->filename = malloc(strlen(filename) + 1);
            if(c->filename) {
                c->name = malloc(strlen(name) + 1);
                if(c->name) {
                    strcpy(c->name, name);
                    strcpy(c->filename, filename);
                    *c = (City) {
                        .name = c->name,
                        .filename = c->filename,
                        .file_times = c->file_times,
                        .id = id,
                        .pr_time_provider = pr_time_provider,
                    };
                } else {
                    goto FREE_FILENAME;
                }
            } else {
                goto CLOSE_FILE;
            }
        }
    }
    return c;

FREE_FILENAME:
    free(c->filename);
CLOSE_FILE:
    fclose(c->file_times);
    c = 0;
    return c;
}

City* city_init_calc(City* c, char const*const name, size_t pr_time_provider, size_t method, size_t id, double longitude, double latitude)
{
    if(c && name) {
        c->name = malloc(strlen(name) + 1);
        if(c->name) {
            strcpy(c->name, name);
            *c = (City) {
                .name = c->name,
                .id = id,
                .pr_time_provider = pr_time_provider,
                .method = method,
                .latitude = latitude,
                .longitude = longitude,
            };
        } else {
            goto ERR_NAME;
        }
    }
    return c;

ERR_NAME:
    return c;
}

City* city_destroy(City* c)
{
    if(c) {
        free(c->filename);
        fclose(c->file_times);
        free(c->name);
    }
    return c;
}

City* city_new(char const*const name, size_t pr_time_provider, char const*const filename, size_t id)
{
    City* c = malloc(sizeof(City));
    if(c) {
        if(!city_init_diyanet(c, name, pr_time_provider, filename, id)) goto FREE_CITY;
    }
    return c;

FREE_CITY:
    free(c);
    c = 0;
    return c;
}

City* city_delete(City* c)
{
    if(c) {
        city_destroy(c);
        free(c);
        c = 0;
    }
    return c;
}

City* city_vnew(size_t num, char const (*name)[num], size_t pr_time_provider[num], char const (*filename)[num], size_t id[num])
{
    assert(num && name && filename && id);
    City* c = 0;
    if(num && name && filename && id) {
        c = malloc(num * sizeof(City));
        for(size_t i = 0; i < num; i++) {
            if(!city_init_diyanet(&c[i], name[i], pr_time_provider[i], filename[i], id[i])) goto FREE_CITY;
        }
    }
    return c;

FREE_CITY:
    for(size_t i = 0; &c[i] != 0; i++) {
        city_destroy(&c[i]);
    }
    free(c);
    c = 0;
    return c;
}

City* city_vdelete(size_t num, City* c)
{
    if(c) {
        for(size_t i = 0; i < num; i++) {
            city_destroy(&c[i]);
        }
        free(c);
        c = 0;
    }
    return c;
}
