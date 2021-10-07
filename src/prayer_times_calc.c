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

#include "config.h"
#include "Salah_times_calc.h"
#include "cJSON.h"
#include "socket.h"
#include "error.h"
#include <math.h>

char const*const ST_cm_names[ST_cm_num] = {
    [ST_cm_Jafari] = "Jafari",
    [ST_cm_Karachi] = "Karachi",
    [ST_cm_ISNA] = "ISNA",
    [ST_cm_MWL] = "Muslim World League",
    [ST_cm_Makkah] = "Makkah",
    [ST_cm_Egypt] = "Egypt",
    [ST_cm_Custom] = "Custom",
};

/*****************************
        Wrapper functions
*****************************/

/** \brief Convert prayer times from double to struct prayer
 *
 * \param date struct tm date of prayer
 * \param pr size_t element of enum prayers, defining which prayer is being converted
 * \param prayer_double double prayer time in double format
 * \return prayer
 *
 */
static prayer convert_to_prayer(struct tm date, size_t pr, double prayer_double)
{
    intPair time = {0};
    if(!isnan(prayer_double)) time = ST_float_time_decompose(prayer_double);
    date.tm_sec = 0;
    date.tm_hour = time.val[0];
    date.tm_min = time.val[1];
    prayer ret = (prayer) {
        .name = prayer_names[pr],
        .time_at = date,
    };
    return ret;
}

/** \brief Convert prayer times from double to struct prayer
 * \details Fajr_end and Sunrise is identical for calculation. Therefore an individual assignment has been done here.
 *
 * \param date struct tm date of prayer
 * \param prayer_times[prayers_num] prayer destination array
 * \param prayer_double[ST_prayer_num] double source array
 * \return void
 *
 */
static void convert_double_to_prayer(struct tm date, prayer prayer_times[prayers_num], double prayer_double[ST_prayer_num])
{
    prayer_times[pr_fajr]       = convert_to_prayer(date, pr_fajr,      prayer_double[ST_prayer_fajr]);
    prayer_times[pr_fajr_end]   = convert_to_prayer(date, pr_fajr_end,  prayer_double[ST_prayer_sunrise]);
    prayer_times[pr_sunrise]    = convert_to_prayer(date, pr_sunrise,   prayer_double[ST_prayer_sunrise]);
    prayer_times[pr_dhuhr]      = convert_to_prayer(date, pr_dhuhr,     prayer_double[ST_prayer_dhuhr]);
    prayer_times[pr_asr]        = convert_to_prayer(date, pr_asr,       prayer_double[ST_prayer_asr]);
    prayer_times[pr_sunset]     = convert_to_prayer(date, pr_sunset,    prayer_double[ST_prayer_sunset]);
    prayer_times[pr_maghreb]    = convert_to_prayer(date, pr_maghreb,   prayer_double[ST_prayer_maghrib]);
    prayer_times[pr_ishaa]      = convert_to_prayer(date, pr_ishaa,     prayer_double[ST_prayer_isha]);
}

int calc_get_todays_prayers(City city, prayer prayer_times[prayers_num])
{
    if(!prayer_times) {
        return EXIT_FAILURE;
    }

    double times_double[prayers_num] = {0};
    ST_get_prayer_times_t(time(0), city.latitude, city.longitude, city.method, city.asr_juristic, city.adjust_high_lats, times_double);
    time_t now = time(0);
    struct tm tm = *localtime(&now);
    convert_double_to_prayer(tm, prayer_times, times_double);
    return EXIT_SUCCESS;
}

int calc_get_preview_for_date(City city, prayer prayer_times[prayers_num], struct tm date)
{
    if(!prayer_times) {
        return EXIT_FAILURE;
    }

    double times_double[prayers_num] = {0};
    time_t time_date = mktime(&date);
    ST_get_prayer_times_t(time_date, city.latitude, city.longitude, city.method, city.asr_juristic, city.adjust_high_lats, times_double);
    convert_double_to_prayer(date, prayer_times, times_double);
    prayer_times[0].hicri_date = calc_get_hijri_date(date);

    return EXIT_SUCCESS;
}

int calc_get_preview_prayers(City city, size_t days, prayer prayer_times[days][prayers_num])
{
    if(!prayer_times) {
        return EXIT_FAILURE;
    }
    time_t date_calc = time(0);

    for(size_t i = 0; i < days; i++) {
        date_calc += 60*60*24;
        if(calc_get_preview_for_date(city, prayer_times[i], *localtime(&date_calc)) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

struct tm calc_get_hijri_date(struct tm today)
{
    struct tm ret_hijri = {0};
    char req_file[50];
    sprintf(req_file, "/v1/gToH?date=%d-%d-%d", today.tm_mday, today.tm_mon + 1, today.tm_year + 1900);
    char* http_response = http_get("api.aladhan.com", req_file, NULL);
    if(http_response) {
        cJSON* json = cJSON_Parse(http_response);
        if(!json) {
        	free(http_response);
        	myperror("Error parsing JSON.");
            return (struct tm) {
                0
            };
        }
        cJSON* data = cJSON_GetObjectItem(json, "data");
        cJSON* hijri = cJSON_GetObjectItem(data, "hijri");
        cJSON* day = cJSON_GetObjectItem(hijri, "day");
        cJSON* month = cJSON_GetObjectItem(hijri, "month");
        cJSON* month_num = cJSON_GetObjectItem(month, "number");
        cJSON* year = cJSON_GetObjectItem(hijri, "year");
        if(day->valuestring && year->valuestring && !month_num->valuestring) {
            int day_val = atoi(day->valuestring);
            int month_val = month_num->valueint;
            int year_val = atoi(year->valuestring);

            assert(day_val);
            assert(month_val);
            assert(year_val);

            ret_hijri.tm_mday = day_val;
            ret_hijri.tm_mon = month_val;
            ret_hijri.tm_year = year_val;
        }
        free(year);
        free(month_num);
        free(month);
        free(day);
        free(hijri);
        free(data);
        free(json);
        free(http_response);
    }
    return ret_hijri;
}
