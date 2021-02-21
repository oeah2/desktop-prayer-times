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
#include <math.h>
#include "config.h"
#include "city.h"


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

    prov_num
};

//extern enum ST_calculation_method;
//extern enum ST_juristic_method;
//extern enum ST_adjusting_method;


extern char const*const prayer_names[prayers_num]; /**< Referenced to prayer names */
extern char const*const provider_names[prov_num]; /**< Referencing to prayer times providers names */
extern char const*const ST_cm_names[ST_cm_num];

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

int diyanet_update_file(City* city, bool preserve_old_data);

/** \brief Calculate prayer time for given city using different calculation methods specified in cfg.
 *
 * \param
 * \param
 * \return
 *
 */

int calc_get_todays_prayers(City city, prayer prayer_times[prayers_num]);

int calc_get_preview_for_date(City city, prayer prayer_times[prayers_num], struct tm date);

int calc_get_preview_prayers(City city, size_t days, prayer prayer_times[days][prayers_num]);

typedef int calc_function(City, prayer[prayers_num]);/**< typedef for the calculation functions. This will be used in the further code to allow the usage of different methods. */

#endif // PRAYER_TIMES_H_INCLUDED
