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

#include "prayer_times.h"
#include <stdio.h>

char const*const prayer_names[prayers_num] = {
    [pr_fajr] = "Morgengebet",
    [pr_fajr_end] = "Ende des Morgengebetes",
    [pr_sunrise] = "Sonnenaufgang",
    [pr_dhuhr] = "Mittagsgebet",
    [pr_asr] = "Nachmittagsgebet",
    [pr_sunset] = "Sonnenuntergang",
    [pr_maghreb] = "Abendgebet",
    [pr_ishaa] = "Nachtgebet",
};

char const*const provider_names[prov_num] = {
    [prov_diyanet] = "diyanet",
    [prov_calc] = "calc",
    [prov_empty] = "",
};

void prayer_print_times(char const*const city_name, prayer times[prayers_num]) // Todo check if this function is depreciated
{
    size_t buffer_length = 200;
    char buffer[buffer_length];
    strftime(buffer, buffer_length, "%d.%m.%Y", &times[0].time_at);
    printf("%s Namaz Vakitleri: %s \n", city_name, buffer);
    for(size_t i = 0; i < prayers_num; i++) {
        strftime(buffer, buffer_length, "%H:%M", &times[i].time_at);
        printf("%s\t%s\n", prayer_names[i],buffer);
    }
}

int empty_get_todays_prayers(City city, prayer prayer_times[prayers_num])
{
    for(size_t i = 0; i < prayers_num; i++) {
    	prayer_times[i] = (prayer) {
    		.time_at = (struct tm) {0},
    		.hicri_date = (struct tm) {0},
			.name = city.name,
    	};
    }
    return EXIT_SUCCESS;
}

int empty_get_preview_for_date(City city, prayer prayer_times[prayers_num], struct tm date)
{
    return empty_get_todays_prayers(city, prayer_times);
}

calc_function* calc_functions[] = {
    [prov_diyanet] =  diyanet_get_todays_prayers,
    [prov_calc] = calc_get_todays_prayers,
    [prov_empty] = empty_get_todays_prayers,
}; /**< Array of function pointers. During calculation, one of these is being called, depending on the configuration of the city */

preview_function* preview_functions[] = {
    [prov_diyanet] = diyanet_get_preview_for_date,
    [prov_calc] = calc_get_preview_for_date,
    [prov_empty] = empty_get_preview_for_date,
};

int sprint_prayer_time(prayer time, size_t buff_len, char dest[buff_len])
{
    char* format = (time.time_at.tm_min < 10) ? "%d:0%d" : "%d:%d";
#ifdef _C11
    return sprintf_s(dest, buff_len, format, time.time_at.tm_hour, time.time_at.tm_min);
#else
    return sprintf(dest, format, time.time_at.tm_hour, time.time_at.tm_min);
#endif
}

int sprint_prayer_remaining(size_t buff_len, char dest[buff_len], int hours, int minutes, int seconds)
{
    char* format = 0;
    if(seconds < 10 && minutes < 10) format = "%d:0%d:0%d";
    else if(seconds < 10 && minutes >= 10) format = "%d:%d:0%d";
    else if(seconds >= 10 && minutes < 10) format = "%d:0%d:%d";
    else if(seconds >= 10 && minutes >= 10) format = "%d:%d:%d";
    assert(format);
#ifdef _C11
    return sprintf_s(dest, buff_len, format, hours, minutes, seconds);
#else
    return sprintf(dest, format, hours, minutes, seconds);
#endif
}

/** \brief Get print format for @p date
 *
 * \return char* format for printf
 */
static char const*const sprint_get_format(struct tm date, bool hijri) {
    char const* format = 0;
    if(date.tm_mday < 10 && date.tm_mon + (hijri ? 0 : 1) < 10) format = "0%d.0%d.%d";
    else if(date.tm_mday >= 10 && date.tm_mon + (hijri ? 0 : 1) < 10) format = "%d.0%d.%d";
    else if(date.tm_mday < 10 && date.tm_mon + (hijri ? 0 : 1) >= 10) format = "0%d.%d.%d";
    else format = "%d.%d.%d";

    return format;
}

int sprint_prayer_date(prayer time, size_t buff_len, char dest[buff_len], bool hijri)
{
    if(!hijri)
#ifdef _C11
        return sprintf_s(dest, buff_len, sprint_get_format(time.time_at, hijri), time.time_at.tm_mday, time.time_at.tm_mon + 1, time.time_at.tm_year + 1900);
#else
        //return sprintf(dest, "%d.%d.%d", time.time_at.tm_mday, time.time_at.tm_mon + 1, time.time_at.tm_year + 1900);
        return sprintf(dest, sprint_get_format(time.time_at, hijri), time.time_at.tm_mday, time.time_at.tm_mon + 1, time.time_at.tm_year + 1900);
#endif
    else
#ifdef _C11
        return sprintf_s(dest, buff_len, sprint_get_format(time.time_at, hijri), time.hicri_date.tm_mday, time.hicri_date.tm_mon, time.hicri_date.tm_year);
#else
        //return sprintf(dest, "%d.%d.%d", time.hicri_date.tm_mday, time.hicri_date.tm_mon, time.hicri_date.tm_year);
        return sprintf(dest, sprint_get_format(time.hicri_date, hijri), time.hicri_date.tm_mday, time.hicri_date.tm_mon, time.hicri_date.tm_year);
#endif
}

int prayer_calc_remaining_time(prayer next, int* hours, int* minutes, int* seconds)
{
    time_t now = 0, remaining = 0;
    int ret = EXIT_FAILURE;
    now = time(0);
    next.time_at.tm_isdst = -1;             // Determine if daylight saving is active

    time_t prayer_tm = mktime(&next.time_at);
    remaining = difftime(prayer_tm, now);
    if(remaining > 0) {
        *hours = remaining / (60*60);
        *minutes = (remaining - *hours * 60 * 60) / 60;
        *seconds = remaining - *hours * 60 * 60 - *minutes * 60;
        ret = EXIT_SUCCESS;
    }
    return ret;
}
