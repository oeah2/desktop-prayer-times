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

#ifndef PRAYER_TIMES_H_INCLUDED
#define PRAYER_TIMES_H_INCLUDED

/*-------------------------------------------------
        User Interface:
    - diyanet_get_todays_prayers
    - diyanet_get_preview_for_date
    - diyanet_get_preview_prayers
    - calc_get_todays_prayers
    - calc_get_preview_for_date
    - calc_get_preview_prayers


--------------------------------------------------*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifndef _WIN32
#define ENOFILE ENOENT
#endif
#include <math.h>
#include "config.h"
#include "city.h"
#include "lang.h"


typedef struct Config Config; // This needs to be added here, because city.h also includes config. Due to include guards, this could otherwise not be used.


enum prayers {    /**< enum for indexing prayers */
    pr_fajr,
    pr_fajr_end,
    pr_sunrise,
    pr_dhuhr,
    pr_asr,
    pr_sunset,
    pr_maghreb,
    pr_ishaa,

    prayers_num
};

enum pr_time_providers { /**< enum for indexing prayer times providers */
    prov_diyanet,
    prov_calc,
    prov_empty,

    prov_num
};

extern char const*const prayer_names[prayers_num]; /**< Referenced to prayer names */
extern char const*const provider_names[prov_num]; /**< Referencing to prayer times providers names */
extern char const*const ST_cm_names[ST_cm_num];
extern char const*const diyanet_prayer_times_file_destination;

typedef struct prayer prayer;

struct prayer {
    char const* name;
    struct tm time_at;
    struct tm hicri_date;
};

/** \brief Prints prayer times. Output to command line. The prayer times need to be calculated before calling this function.
 *
 * \param city_name name of city
 * \param times array of struct prayer containing the times
 *
 */

void prayer_print_times(char const*const city_name, prayer times[prayers_num]);


/** Functions for Turkish Diyanet
**/

/** \brief Calculates todays prayer times
 *
 * \param city_id chosen id of city; this must match the id from initialization
 * \param prayer_times array of struct prayer. The array must contain at least @p prayers_num elements
 * \return EXIT_FAILURE upon failure, EXIT_SUCCESS otherwise
 *
 */
int diyanet_get_todays_prayers(City city, prayer prayer_times[prayers_num]);

/** \brief Calculates prayer times for given date
 *
 * \param city_id chosen id of city; this must match the id from initialization
 * \param prayer_times array of struct prayer. The array must contain at least @p prayers_num elements
 * \param date determines for which date the prayer times shall be calculated. Due to limitations in the provided files, date must not exceed 30 days from today
 * \return EXIT_FAILURE upon failure, EXIT_SUCCESS otherwise
 *
 */
int diyanet_get_preview_for_date(City city, prayer prayer_times[prayers_num], struct tm date);

/** \brief Calculates prayer times for given @p days days
 *
 * \param city_id chosen id of city; this must match the id from initialization
 * \param days the number of days, starting from tomorrow, for which the prayers shall be calculated
 * \param prayer_times two dimensional array of struct prayer. The array must contain at least @p days of columns of @p prayers_num elements
 * \return EXIT_FAILURE upon failure, EXIT_SUCCESS otherwise
 *
 */
int diyanet_get_preview_prayers(City city, size_t days, prayer prayer_times[days][prayers_num]);

/** \brief Updates prayer times file for given City
 *
 * \param city City* whose file shall be updated
 * \param preserve_old_data bool if true, prayer times of past are preserved.
 * \return int EXIT_FAILURE upon failure, EXIT_SUCCESS otherwise
 *
 */
int diyanet_update_file(City* city, bool preserve_old_data);

/** \brief Returns a string containing all countries and their codes. The user needs to free the returned string
 *
 * \param lang enum Languages language of the country names
 * \return char* String containing all countries and their codes in the format "country,code;"
 *
 */
char* diyanet_get_country_codes(enum Languages lang);

/** \brief Returns a string containing all provinces of given country
 *
 * \param country_code size_t country for which the provinces shall be given
 * \param lang enum Languages language of the province names
 * \return char* string containing all provinces and their codes in the format "province,code;"
 *
 */
char* diyanet_get_provinces(size_t country_code, enum Languages lang);

/** \brief Returns a string containing all cities of a given province.
 *  \details Call this function only after first calling diyanet_get_provinces.
 *  Otherwise there is no way to determine to which province or country the given cities belong.
 *
 * \param province_code size_t
 * \param lang enum Languages
 * \return char*
 *
 */
char* diyanet_get_cities(size_t province_code, enum Languages lang);

/** \brief Calculate prayer time for given city
 * \details The parameters used for calculation are given in city.
 *
 * \param city City for which the prayer times shall be calculated
 * \param prayer_times[prayers_num] prayer array of struct prayer. The array must contain at least @p prayers_num elements
 * \return int EXIT_FAILURE on failure, EXIT_SUCCESS otherwise
 *
 */
int calc_get_todays_prayers(City city, prayer prayer_times[prayers_num]);

/** \brief Calculates prayer times for a given date
 * \details The parameters used for calculation are given in city.
 *
 * \param city City for which the prayer times shall be calculated
 * \param prayer_times[prayers_num] prayer array of struct prayer. The array must contain at least @p prayers_num elements
 * \return int EXIT_FAILURE on failure, EXIT_SUCCESS otherwise
 *
 */
int calc_get_preview_for_date(City city, prayer prayer_times[prayers_num], struct tm date);

/** \brief Calculates prayer times for number of @p days starting from today
 * \details The parameters used for calculation are given in city.
 *
 * \param city City for which the prayer times shall be calculated
 * \param days size_t number of days starting from today, for which the prayer times shall be calculated
 * \param prayer_times[prayers_num] prayer array of struct prayer. The array must contain at least @p prayers_num elements
 * \return int EXIT_FAILURE on failure, EXIT_SUCCESS otherwise
 *
 */
int calc_get_preview_prayers(City city, size_t days, prayer prayer_times[days][prayers_num]);

/** \brief Determine hijri date for given gregorian date
 */
struct tm calc_get_hijri_date(struct tm today);

/** \brief Print prayer time in struct prayer into string
 *
 * \param time prayer
 * \param buff_len size_t
 * \param dest[buff_len] char
 * \return int return of sprintf_s
 *
 */
int sprint_prayer_time(prayer time, size_t buff_len, char dest[buff_len]);

/** \brief Print remaining time to prayer into string
 *
 * \param buff_len size_t
 * \param dest[buff_len] char
 * \param hours int
 * \param minutes int
 * \param seconds int
 * \return int return of sprintf_s
 *
 */
int sprint_prayer_remaining(size_t buff_len, char dest[buff_len], int hours, int minutes, int seconds);

/** \brief Print date in struct prayer into string
 *
 * \param time prayer
 * \param buff_len size_t
 * \param dest[buff_len] char
 * \param hijri bool true for hijri date, false for julian
 * \return int return of sprintf_s
 *
 */
int sprint_prayer_date(prayer time, size_t buff_len, char dest[buff_len], bool hijri);

/** \brief Calculate remaining time
 *
 * \param next prayer
 * \param hours int*
 * \param minutes int*
 * \param seconds int*
 * \return int
 *
 */
int prayer_calc_remaining_time(prayer next, int* hours, int* minutes, int* seconds);

/** \brief Return empty prayer times, no calculation
 *
 * \param city City
 * \param prayer_times[prayers_num] prayer
 * \return int
 *
 */
int empty_get_todays_prayers(City city, prayer prayer_times[prayers_num]);

/** \brief Return empty prayer times, no calculation
 *
 * \param city City
 * \param prayer_times[prayers_num] prayer
 * \param date struct tm
 * \return int
 *
 */
int empty_get_preview_for_date(City city, prayer prayer_times[prayers_num], struct tm date);

typedef int calc_function(City, prayer[prayers_num]);/**< typedef for the calculation functions. This will be used in the further code to allow the usage of different methods. */
typedef int preview_function(City, prayer[prayers_num], struct tm date); /**< typedef for preview functions */

#endif // PRAYER_TIMES_H_INCLUDED
