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


#include "city.h"
#include "prayer_times.h"

City* city_init_diyanet(City* c, char const*const name, size_t pr_time_provider, char const*const filename, size_t id)
{
    if(c && name && filename) {
        static size_t retry;
        retry = 0;  // Reset for next city
        c->filename = malloc(strlen(filename) + 1);
        if(c->filename) {
            c->name = malloc(strlen(name) + 1);
            if(c->name) {
                strcpy(c->name, name);
                strcpy(c->filename, filename);
                c->file_times = fopen(filename, "r");
INIT_STRUCT:
                if(c->file_times) {
                    *c = (City) {
                        .name = c->name,
                        .filename = c->filename,
                        .file_times = c->file_times,
                        .id = id,
                        .pr_time_provider = pr_time_provider,
                    };
                } else {
                    if(retry <= 3) {
                        retry++;
                        diyanet_update_file(c, false);
                        goto INIT_STRUCT;
                    } else {
                        goto ERR_FREE_NAME;
                    }
                }
            } else {
                goto ERR_FREE_FILENAME;
            }
        }
    }
    return c;
ERR_FREE_NAME:
    free(c->name);
ERR_FREE_FILENAME:
    free(c->filename);
    c = 0;
    return c;
}

City* city_init_calc(City* c, char const*const name, size_t pr_time_provider, size_t method, size_t id, double longitude, double latitude, size_t asr_juristic, size_t adjust_high_lats)
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
                .asr_juristic = asr_juristic,
                .adjust_high_lats = adjust_high_lats,
            };
        } else {
            goto ERR_NAME;
        }
    }
    return c;

ERR_NAME:
    return c;
}

City* city_set_name(City* c, char const*const name) {
	City* ret = 0;
	if(c && name) {
		c->name = malloc(strlen(name) + 1);
		if(c->name) {
			strcpy(c->name, name);
			ret = c;
		}
	}
	return ret;
}

City* city_init_empty(City* c) {
	if(c) {
		*c = (City) {
			.pr_time_provider = prov_empty,
		};
	}
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
        if(pr_time_provider == prov_diyanet && !city_init_diyanet(c, name, pr_time_provider, filename, id)) goto FREE_CITY;
    }
    return c;

FREE_CITY:
    free(c);
    c = 0;
    return c;
}

City* city_new_calc(char const*const name, size_t pr_time_provider, size_t method, size_t id, double longitude, double latitude, size_t asr_juristic, size_t adjust_high_lats) {
    City* c = malloc(sizeof(City));
    if(c) {
        if(pr_time_provider == prov_calc && !city_init_calc(c, name, pr_time_provider, method, id, longitude, latitude, asr_juristic, adjust_high_lats)) goto FREE_CITY;
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

#define EPS             (1.0E-6)
#define CMP_DBL(x,y)     (fabs((x)-(y)) < EPS)

bool city_is_equal(City a, City b)
{
    return  a.adjust_high_lats == b.adjust_high_lats &&
            a.asr_juristic == b.asr_juristic &&
#ifdef ARE_THESE_TESTS_OF_ANY_BENEFIT
            !strcmp(a.filename, b.filename) &&
            a.file_times == b.file_times &&
            a.id == b.id &&
#endif // ARE_THESE_TESTS_OF_ANY_BENEFIT
            CMP_DBL(a.latitude, b.latitude) &&    //a.latitude == b.latitude &&
            CMP_DBL(a.longitude, b.longitude) &&
            a.method == b.method &&
            a.pr_time_provider == b.pr_time_provider;
}
