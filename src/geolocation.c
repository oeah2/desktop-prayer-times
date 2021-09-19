#include <stdio.h>
#include <locale.h>
#include "geolocation.h"
#include "cJSON.h"
#include "socket.h"
#include "config.h"

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
        perror("Error parsing JSON!");
        perror(cJSON_GetErrorPtr());
        return ret;
    }

    if(cJSON_IsArray(json)) {
        ret = calloc(10E3, sizeof(char));
        cJSON const* element = 0;

        cJSON_ArrayForEach(element, json) {
            char puffer[400];
            cJSON* name = cJSON_GetObjectItem(element, "display_name");
            cJSON* lat_str = cJSON_GetObjectItem(element, "lat");
            cJSON* long_str = cJSON_GetObjectItem(element, "lon");

            char* prev_locale = setlocale(LC_ALL, NULL);
            setlocale(LC_ALL, "C");     // needed to guarantee correct conversion independent of user locale
            double latitude = strtof(cJSON_GetStringValue(lat_str), NULL);
            double longitude = strtof(cJSON_GetStringValue(long_str), NULL);
            setlocale(LC_ALL, prev_locale);

            sprintf(puffer, "%s: %f\t%f\n", name->valuestring, latitude, longitude);
            strcat(ret, puffer);
        }
        ret = realloc(ret, strlen(ret) + 1);
    }
    cJSON_Delete(json);

    return ret;
}
