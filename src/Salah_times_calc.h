/*-------------------------- In the name of God ----------------------------*\

    libprayertimes 1.0
    Islamic prayer times calculator library
    Based on PrayTimes 1.0 C++ library

----------------------------- Copyright Block --------------------------------

Copyright (C) 2007-2010 PrayTimes.org

Ported to C by: Ahmet Ozturk <sivasli-ahmet at gmx dot de>
Developed By: Mohammad Ebrahim Mohammadi Panah <ebrahim at mohammadi dot ir>
Based on a JavaScript Code By: Hamid Zarrabi-Zadeh

License: GNU LGPL v3.0

TERMS OF USE:
    Permission is granted to use this code, with or
    without modification, in any website or application
    provided that credit is given to the original work
    with a link back to PrayTimes.org.

This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY.

PLEASE DO NOT REMOVE THIS COPYRIGHT BLOCK.

------------------------------------------------------------------------------

User's Manual:
http://praytimes.org/manual

Calculating Formulas:
http://praytimes.org/calculation

Code Repository:
https://github.com/oeah2/Salah_times_lib

\*--------------------------------------------------------------------------*/

#ifndef SALAH_TIMES_INCLUDED
#define SALAH_TIMES_INCLUDED

#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>


/* --------------------- User Interface ----------------------- */
/*


	ST_get_prayer_times_custom(custom_config, year, month, day, latitude, longitude, asr_juristic, adjust_high_lats, times[])
    ST_get_prayer_times_t(date, latitude, longitude, calc_method, asr_juristic, adjust_high_lats, times[]);
    ST_get_prayer_times(year, month, day, latitude, longitude, calc_method, asr_juristic, adjust_high_lats, times[])

	ST_method_custom_init(fajr_angle, maghrib_is_minutes, maghrib_value, isha_is_minutes, isha_value)

	ST_float_time_to_str24(dest[], time)
	ST_float_time_to_str12(dest[], time, no_suffix)
	ST_float_time_decompose(double time);


	enum ST_prayers             :   Identifiers for the prayers
	enum ST_calculation_method  :   Identifiers for calculation methods. Pass one of these to calc functions
	enum ST_juristic_method     :   Identifiers for asr juristic method. Pass one of these to calc functions
	enum ST_adjusting_method    :   Identifiers for High latitude adjustments. Pass one of these to set_high_lats_adjust_method
*/




/************************************************
        Enums
************************************************/

enum {
    VERSION_MAJOR = 1,
    VERSION_MINOR = 0,
};

enum ST_prayers {                           /**< Used to identify the prayers */
    ST_prayer_fajr,
    ST_prayer_sunrise,
    ST_prayer_dhuhr,
    ST_prayer_asr,
    ST_prayer_sunset,
    ST_prayer_maghrib,
    ST_prayer_isha,

    ST_prayer_num
};

enum ST_calculation_method {                /**< Used to identify the calculation methods, including custom method */
    ST_cm_Jafari, 	// Ithna Ashari
    ST_cm_Karachi,	// University of Islamic Sciences, Karachi
    ST_cm_ISNA,   	// Islamic Society of North America (ISNA)
    ST_cm_MWL,    	// Muslim World League (MWL)
    ST_cm_Makkah, 	// Umm al-Qura, Makkah
    ST_cm_Egypt,  	// Egyptian General Authority of Survey
    ST_cm_Custom, 	// Custom Setting

    ST_cm_num,
};

// Juristic Methods
enum ST_juristic_method {                   /**< Used to identify the Asr Juristic method */
    ST_jm_Shafii,    // Shafii (standard)
    ST_jm_Hanafi,    // Hanafi
};

// Adjusting Methods for Higher Latitudes
enum ST_adjusting_method {                  /**< Used to identify the High latitude adjusting method */
    ST_am_None,      	// No adjustment
    ST_am_MidNight,  	// middle of night
    ST_am_OneSeventh,	// 1/7th of night
    ST_am_AngleBased,	// angle/60th of night
};


typedef struct intPair intPair;             /**< Used to return double formatted time as two integers */
struct intPair {
    int val[2];
};


/******************************************************
        Structs
******************************************************/
typedef struct ST_method_config ST_method_config;

/* Interface Functions */

/** \brief Create struct ST_method_config for setting custom calculation parameters. This struct can be stored for further use in function @f ST_get_prayer_times_custom
 *
 * \param fajr_angle
 * \param maghrib_is_minutes
 * \param maghrib_value
 * \param isha_is_minutes
 * \param isha_value
 * \return returns a struct ST_method_config containing the parameters for calculation
 *
 */
ST_method_config ST_method_custom_init(double fajr_angle, bool maghrib_is_minutes, double maghrib_value, bool isha_is_minutes, double isha_value);

/** \brief Converts @p time to string in 24 hour format
 *
 * \param dest destination array
 * \param time
 *
 */
void ST_float_time_to_str24(char dest[static 6], double time);

/** \brief Converts @p time to string in 12 hour format
 *
 * \param dest destination array
 * \param time
 * \param no_suffix true for no suffix, false for suffix
 *
 */
void ST_float_time_to_str12(char dest[static 9], double time, bool no_suffix);

/** \brief Decompose double formatted time into an pair in integers
 *
 * \param time to be decomposed
 * \return intPair. First element in array is hour, second is minutes.
 *
 */

intPair ST_float_time_decompose(double time);

/** \brief Calculate prayer time for given date and location
 *
 * \param year of interest
 * \param month of interest
 * \param day of interest
 * \param latitude of location
 * \param longitude of location
 * \param calc_method chosen calculation method. needs to be element of enum ST_calculation_method
 * \param asr_juristic chosen method for asr juristic. needs to be element of enum ST_juristic_method
 * \param adjust_high_lats chosen method for high latitude adjustments for fajr and isha
 * \param times destination array
 *
 */

void ST_get_prayer_times(int year, int month, int day, double latitude, double longitude, enum ST_calculation_method calc_method, enum ST_juristic_method asr_juristic, enum ST_adjusting_method adjust_method, double times[static ST_prayer_num]);

/** \brief Calculate prayer times for given date and location
 *
 * \param date of interest
 * \param latitude of location
 * \param longitude of location
 * \param calc_method chosen calculation method. needs to be element of enum oe_calculation_method
 * \param asr_juristic chosen method for asr juristic. needs to be element of enum oe_juristic_method
 * \param adjust_high_lats chosen method for high latitude adjustments for fajr and isha
 * \param times destination array
 *
 */

void ST_get_prayer_times_t(time_t date, double latitude, double longitude, enum ST_calculation_method calc_method, enum ST_juristic_method asr_juristic, enum ST_adjusting_method adjust_method, double times[static ST_prayer_num]);

/** \brief Calculate prayer times for given date, location and custom configuration
 *
 * \param custom_config struct ST_method_config, containing custom calculation parameters
 * \param year of interest
 * \param month of interest
 * \param day of interest
 * \param latitude of location
 * \param longitude of location
 * \param asr_juristic chosen method for asr juristic. needs to be element of enum ST_juristic_method
 * \param adjust_high_lats chosen method for high latitude adjustments for fajr and isha
 * \param times destination array
 *
 */

void ST_get_prayer_times_custom(ST_method_config custom_config, int year, int month, int day, double latitude, double longitude, enum ST_juristic_method asr_juristic, enum ST_adjusting_method adjust_method, double times[static ST_prayer_num]);

#endif // SALAH_TIMES_INCLUDED
