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

#include <stdio.h>
#include <locale.h>
#include "geolocation.h"
#include "cJSON.h"
#include "socket.h"
#include "config.h"
#include "error.h"

char* geolocation_get(char const*const city_name)
{
    char* ret = 0;
    char const*const host = "nominatim.openstreetmap.org";
    char file[100];
    sprintf(file, "/search?city=%s&format=json", city_name);
#ifdef OLDCODE
    char user_agent[100];
    sprintf(user_agent, "%s %d.%d", "User-Agent: desktop_prayer_time/", VERSION_MAIN_MAJOR, VERSION_MAIN_MINOR);
    char* http_response = https_get(host, file, user_agent);
#else
    char* http_response = https_get_with_useragent(host, file, NULL);
#endif // OLDCODE
    if(!http_response) return 0;
    char* http_resp_json_start = strchr(http_response, '[');
    cJSON* json = cJSON_Parse(http_resp_json_start);
    free(http_response);
    http_response = 0;

    if(!json) {
    	myperror("Error parsing JSON!");
    	myperror(cJSON_GetErrorPtr());
        return ret;
    }

    if(cJSON_IsArray(json)) {
        ret = calloc(10E3, sizeof(char));
        cJSON const* element = 0;

        cJSON_ArrayForEach(element, json) {
            const size_t length_same = 50;
            char puffer[400];
            char puffer2[length_same];
            cJSON* name = cJSON_GetObjectItem(element, "display_name");
            cJSON* lat_str = cJSON_GetObjectItem(element, "lat");
            cJSON* long_str = cJSON_GetObjectItem(element, "lon");

            char prev_locale[30];
            strcpy(prev_locale, setlocale(LC_ALL, NULL));
            setlocale(LC_ALL, "C");     // needed to guarantee correct conversion independent of user locale
            double latitude = strtof(cJSON_GetStringValue(lat_str), NULL);
            double longitude = strtof(cJSON_GetStringValue(long_str), NULL);
            setlocale(LC_ALL, prev_locale);

            sprintf(puffer, "%s: %f;%f\n", name->valuestring, latitude, longitude);
            memcpy(puffer2, puffer, length_same - 1);
            puffer2[length_same - 1] = '\0';
            if(!strstr(ret, puffer2))
                strcat(ret, puffer);
        }
        ret = realloc(ret, strlen(ret) + 1);
    }
    cJSON_Delete(json);

    return ret;
}
