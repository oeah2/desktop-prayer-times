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


#include "Salah_times_calc.h"
#define M_PI		3.14159265358979323846


/***********************************************
        Structs
***********************************************/

/** Double Pair struct and initializer for EqT Function */
typedef struct doublePair doublePair;

struct doublePair {
    double val[2];
};

enum doublePair_identifier {
    dp_first = 0,
    dp_second = 1,
};

static doublePair doublePair_init(double a, double b)
{
    doublePair ret = {
        .val = {a, b},
    };
    return ret;
}

/** Method Config for different calculation methods */
typedef struct ST_method_config ST_method_config;

struct ST_method_config {
    double fajr_angle;
    double dhuhr_minutes;
    double maghrib_value;		// angle or minutes
    double isha_value;		// angle or minutes
    bool   maghrib_is_minutes;
    bool   isha_is_minutes;
    enum ST_adjusting_method adjust_high_lats;
};

/**********************************************************
        Static global variables;
        calculation parameters
**********************************************************/

static ST_method_config ST_method_params[ST_cm_num] = {
    [ST_cm_Jafari].fajr_angle = 16.0,
    [ST_cm_Jafari].maghrib_is_minutes = false,
    [ST_cm_Jafari].maghrib_value = 4.0,
    [ST_cm_Jafari].isha_is_minutes = false,
    [ST_cm_Jafari].isha_value = 14.0,

    [ST_cm_Karachi].fajr_angle = 18.0,
    [ST_cm_Karachi].maghrib_is_minutes = true,
    [ST_cm_Karachi].maghrib_value = 0.0,
    [ST_cm_Karachi].isha_is_minutes = false,
    [ST_cm_Karachi].isha_value = 18.0,

    [ST_cm_ISNA].fajr_angle = 15.0,
    [ST_cm_ISNA].maghrib_is_minutes = true,
    [ST_cm_ISNA].maghrib_value = 0.0,
    [ST_cm_ISNA].isha_is_minutes = false,
    [ST_cm_ISNA].isha_value = 15.0,

    [ST_cm_MWL].fajr_angle = 18.0,
    [ST_cm_MWL].maghrib_is_minutes = true,
    [ST_cm_MWL].maghrib_value = 0.0,
    [ST_cm_MWL].isha_is_minutes = false,
    [ST_cm_MWL].isha_value = 17.0,

    [ST_cm_Makkah].fajr_angle = 19.0,
    [ST_cm_Makkah].maghrib_is_minutes = true,
    [ST_cm_Makkah].maghrib_value = 0.0,
    [ST_cm_Makkah].isha_is_minutes = true,
    [ST_cm_Makkah].isha_value = 90.0,

    [ST_cm_Egypt].fajr_angle = 19.5,
    [ST_cm_Egypt].maghrib_is_minutes = true,
    [ST_cm_Egypt].maghrib_value = 0.0,
    [ST_cm_Egypt].isha_is_minutes = false,
    [ST_cm_Egypt].isha_value = 17.5,

    [ST_cm_Custom].fajr_angle = 18.0,
    [ST_cm_Custom].maghrib_is_minutes = true,
    [ST_cm_Custom].maghrib_value = 0.0,
    [ST_cm_Custom].isha_is_minutes = false,
    [ST_cm_Custom].isha_value = 17.0,
};

ST_method_config ST_method_custom_init(double fajr_angle, bool maghrib_is_minutes, double maghrib_value, bool isha_is_minutes, double isha_value)
{
    ST_method_config ret = (ST_method_config) {
        .fajr_angle = fajr_angle,
        .maghrib_is_minutes = maghrib_is_minutes,
        .maghrib_value = maghrib_value,
        .isha_is_minutes = isha_is_minutes,
        .isha_value = isha_value,
    };
    return ret;
}


static void ST_method_set_high_lats(enum ST_calculation_method calc_method, enum ST_adjusting_method adjust_high_lats)
{
    ST_method_params[calc_method].adjust_high_lats = adjust_high_lats;
}

/**********************************************************
        Trigonometric Functions
**********************************************************/

/* degree to radian */
static double deg2rad(double d)
{
    return d * M_PI / 180.0;
}

/* radian to degree */
static double rad2deg(double r)
{
    return r * 180.0 / M_PI;
}

/* degree sin */
static double dsin(double d)
{
    return sin(deg2rad(d));
}

/* degree cos */
static double dcos(double d)
{
    return cos(deg2rad(d));
}

/* degree tan */
static double dtan(double d)
{
    return tan(deg2rad(d));
}

/* degree arcsin */
static double darcsin(double x)
{
    return rad2deg(asin(x));
}

/* degree arccos */
static double darccos(double x)
{
    return rad2deg(acos(x));
}

/* degree arctan2 */
static double darctan2(double y, double x)
{
    return rad2deg(atan2(y, x));
}

/* degree arccot */
static double darccot(double x)
{
    return rad2deg(atan(1.0 / x));
}


/* range reduce angle in degrees. */
static double fix_angle(double a)
{
    a = a - 360.0 * floor(a / 360.0);
    a = a < 0.0 ? a + 360.0 : a;
    return a;
}

/* range reduce hours to 0..23 */
static double fix_hour(double a)
{
    a = a - 24.0 * floor(a / 24.0);
    a = a < 0.0 ? a + 24.0 : a;
    return a;
}

/*********************************************
        Julianic Date Functions
*********************************************/

/* calculate julian date from a calendar date */
static double get_julian_date(int year, int month, int day)
{
    if (month <= 2) {
        year -= 1;
        month += 12;
    }

    double a = floor(year / 100.0);
    double b = 2 - a + floor(a / 4.0);

    return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + b - 1524.5;
}

/* ---------------------- Time-Zone Functions ----------------------- */

/* compute local time-zone for a specific date */
static double get_effective_timezone_t(time_t local_time)
{
    struct tm* tmp = localtime(&local_time);
    tmp->tm_isdst = 0;
    time_t local = mktime(tmp);
    tmp = gmtime(&local_time);
    tmp->tm_isdst = 0;
    time_t gmt = mktime(tmp);
    return (local - gmt) / 3600.0;
}

/* compute local time-zone for a specific date */
static double get_effective_timezone(int year, int month, int day)
{
    struct tm date = { 0 };
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    date.tm_isdst = -1;		// determine it yourself from system
    time_t local = mktime(&date);		// seconds since midnight Jan 1, 1970
    return get_effective_timezone_t(local);
}

/* ----------------------------- Hour format change ----------------- */
/* get hours and minutes parts of a float time */
static void get_float_time_parts(double time, int* hours, int* minutes)
{
    time = fix_hour(time + 0.5 / 60);		// add 0.5 minutes to round
    *hours = floor(time);
    *minutes = floor((time - *hours) * 60);
}

/* convert float hours to 24h format */
void ST_float_time_to_str24(char dest[static 6], double time)
{
    if (isnan(time))
        return;
    int hours, minutes;
    get_float_time_parts(time, &hours, &minutes);
    sprintf(dest, "%2d:%2d", hours, minutes);
}

/* convert float hours to 12h format */
void ST_float_time_to_str12(char dest[static 9], double time, bool no_suffix)
{
    if (isnan(time))
        return;
    int hours, minutes;
    get_float_time_parts(time, &hours, &minutes);
    const char* suffix = hours >= 12 ? " PM" : " AM";
    hours = (hours + 12 - 1) % 12 + 1;
    if(no_suffix)
        sprintf(dest, "%2d:%2d", hours, minutes);
    else
        sprintf(dest, "%2d:%2d%s", hours, minutes, suffix);
}

intPair ST_float_time_decompose(double time)
{
    intPair ret = {0};
    get_float_time_parts(time, &ret.val[dp_first], &ret.val[dp_second]);
    return ret;
}

/**********************************************
        Sun Calculation Functions
**********************************************/

/* compute declination angle of sun and equation of time */
static doublePair sun_position(double jd)
{
    double d = jd - 2451545.0;
    double g = fix_angle(357.529 + 0.98560028 * d);
    double q = fix_angle(280.459 + 0.98564736 * d);
    double l = fix_angle(q + 1.915 * dsin(g) + 0.020 * dsin(2 * g));

    // double r = 1.00014 - 0.01671 * dcos(g) - 0.00014 * dcos(2 * g);
    double e = 23.439 - 0.00000036 * d;

    double dd = darcsin(dsin(e) * dsin(l));
    double ra = darctan2(dcos(e) * dsin(l), dcos(l)) / 15.0;
    ra = fix_hour(ra);
    double eq_t = q / 15.0 - ra;

    return doublePair_init(dd, eq_t);
}

/* compute equation of time */
static double equation_of_time(double jd)
{
    return sun_position(jd).val[dp_second];
}

/* compute declination angle of sun */
static double sun_declination(double jd)
{
    return sun_position(jd).val[dp_first];
}

/********************************************
        Prayer Calculation Functions
********************************************/

/* compute mid-day (Dhuhr, Zawal) time */
static double compute_mid_day(double julian_date, double _t)
{
    double t = equation_of_time(julian_date + _t);
    double z = fix_hour(12 - t);
    return z;
}

/* compute time for a given angle G */
static double compute_time(double julian_date, double latitude, double g, double t)
{
    double d = sun_declination(julian_date + t);
    double z = compute_mid_day(julian_date, t);
    double v = 1.0 / 15.0 * darccos((-dsin(g) - dsin(d) * dsin(latitude)) / (dcos(d) * dcos(latitude)));
    return z + (g > 90.0 ? - v :  v);
}

/* compute the time of Asr */
static double compute_asr(double julian_date, double latitude, int step, double t)  // Shafii: step=1, Hanafi: step=2
{
    double d = sun_declination(julian_date + t);
    double g = -darccot(step + dtan(fabs(latitude - d)));
    return compute_time(julian_date, latitude, g, t);
}

/* compute the difference between two times  */
static double time_diff(double time1, double time2)
{
    return fix_hour(time2 - time1);
}

/* the night portion used for adjusting times in higher latitudes */
static double night_portion(enum ST_adjusting_method adjust_high_lats, double angle)
{
    switch (adjust_high_lats) {
    case ST_am_AngleBased:
        return angle / 60.0;
    case ST_am_MidNight:
        return 1.0 / 2.0;
    case ST_am_OneSeventh:
        return 1.0 / 7.0;
    case ST_am_None:
    default:
        // Just to return something!
        // In original library nothing was returned
        // Maybe I should throw an exception
        // It must be impossible to reach here
        return 0;
    }
}

/***************************************************
        Compute prayer times
***************************************************/

/* convert hours to day portions  */
static void day_portion(double times[static ST_prayer_num])
{
    for (int i = 0; i < ST_prayer_num; ++i)
        times[i] /= 24.0;
}

/* compute prayer times at given julian date */
static void compute_times(double julian_date, double latitude, enum ST_calculation_method calc_method, enum ST_juristic_method asr_juristic, double times[])
{
    day_portion(times);

    times[ST_prayer_fajr]    = compute_time(julian_date, latitude, 180.0 - ST_method_params[calc_method].fajr_angle, times[ST_prayer_fajr]);
    times[ST_prayer_sunrise] = compute_time(julian_date, latitude, 180.0 - 0.833, times[ST_prayer_sunrise]);
    times[ST_prayer_dhuhr]   = compute_mid_day(julian_date, times[ST_prayer_dhuhr]);
    times[ST_prayer_asr]     = compute_asr(julian_date, latitude, 1 + asr_juristic, times[ST_prayer_asr]);
    times[ST_prayer_sunset]  = compute_time(julian_date, latitude, 0.833, times[ST_prayer_sunset]);
    times[ST_prayer_maghrib] = compute_time(julian_date, latitude, ST_method_params[calc_method].maghrib_value, times[ST_prayer_maghrib]); // Todo Check if this is fine. how is maghrib angle adjusted.
    times[ST_prayer_isha]    = compute_time(julian_date, latitude, ST_method_params[calc_method].isha_value, times[ST_prayer_isha]);
}

/* adjust Fajr, Isha and Maghrib for locations in higher latitudes */
static void adjust_high_lat_times(enum ST_calculation_method calc_method, double times[])
{
    double night_time = time_diff(times[ST_prayer_sunset], times[ST_prayer_sunrise]);		// sunset to sunrise

    // Adjust Fajr
    double fajr_diff = night_portion(ST_method_params[calc_method].adjust_high_lats, ST_method_params[calc_method].fajr_angle) * night_time;
    if (isnan(times[ST_prayer_fajr]) || time_diff(times[ST_prayer_fajr], times[ST_prayer_sunrise]) > fajr_diff)
        times[ST_prayer_fajr] = times[ST_prayer_sunrise] - fajr_diff;

    // Adjust Isha
    double isha_angle = ST_method_params[calc_method].isha_is_minutes ? 18.0 : ST_method_params[calc_method].isha_value;
    double isha_diff = night_portion(ST_method_params[calc_method].adjust_high_lats, isha_angle) * night_time;
    if (isnan(times[ST_prayer_isha]) || time_diff(times[ST_prayer_sunset], times[ST_prayer_isha]) > isha_diff)
        times[ST_prayer_isha] = times[ST_prayer_sunset] + isha_diff;

    // Adjust Maghrib
    double maghrib_angle = ST_method_params[calc_method].maghrib_is_minutes ? 4.0 : ST_method_params[calc_method].maghrib_value;
    double maghrib_diff = night_portion(ST_method_params[calc_method].adjust_high_lats, maghrib_angle) * night_time;
    if (isnan(times[ST_prayer_maghrib]) || time_diff(times[ST_prayer_sunset], times[ST_prayer_maghrib]) > maghrib_diff)
        times[ST_prayer_maghrib] = times[ST_prayer_sunset] + maghrib_diff;
}

/* adjust times in a prayer time array */
static void adjust_times(double julian_date, double longitude, enum ST_calculation_method calc_method, enum ST_juristic_method asr_juristic, double timezone, double times[])
{
    for (int i = 0; i < ST_prayer_num; ++i)
        times[i] += timezone - longitude / 15.0;
    times[ST_prayer_dhuhr] += ST_method_params[calc_method].dhuhr_minutes / 60.0;		// Dhuhr
    if (ST_method_params[calc_method].maghrib_is_minutes)		// Maghrib
        times[ST_prayer_maghrib] = times[ST_prayer_sunset] + ST_method_params[calc_method].maghrib_value / 60.0;
    if (ST_method_params[calc_method].isha_is_minutes)		// Isha
        times[ST_prayer_isha] = times[ST_prayer_maghrib] + ST_method_params[calc_method].isha_value / 60.0;

    if (ST_method_params[calc_method].adjust_high_lats != ST_am_None)
        adjust_high_lat_times(calc_method, times);
}

/* compute prayer times at given julian date */
static void compute_day_times(double julian_date, double longitude, double latitude, enum ST_calculation_method calc_method, enum ST_juristic_method asr_juristic, double timezone, double times[])
{
    static size_t NUM_ITERATIONS = 1;
    double default_times[] = { 5, 6, 12, 13, 18, 18, 18 };		// default times
    for (int i = 0; i < ST_prayer_num; ++i)
        times[i] = default_times[i];

    for (int i = 0; i < NUM_ITERATIONS; ++i)
        compute_times(julian_date, latitude, calc_method, asr_juristic,times);

    adjust_times(julian_date, longitude, calc_method, asr_juristic, timezone, times);
}


/****************************************************
        Interface functions
****************************************************/

/* return prayer times for a given date */
void ST_get_prayer_times(int year, int month, int day, double latitude, double longitude, enum ST_calculation_method calc_method, enum ST_juristic_method asr_juristic, enum ST_adjusting_method adjust_method, double times[static ST_prayer_num])
{
    double julian_date = get_julian_date(year, month, day) - longitude / (double) (15 * 24);
    double timezone = get_effective_timezone(year, month, day);
    ST_method_set_high_lats(calc_method, adjust_method);
    compute_day_times(julian_date, longitude, latitude, calc_method, asr_juristic, timezone, times);
}

/* return prayer times for a given date */
void ST_get_prayer_times_t(time_t date, double latitude, double longitude, enum ST_calculation_method calc_method, enum ST_juristic_method asr_juristic, enum ST_adjusting_method adjust_method, double times[static ST_prayer_num])
{
    struct tm* t = localtime(&date);
    ST_get_prayer_times(1900 + t->tm_year, t->tm_mon + 1, t->tm_mday, latitude, longitude, calc_method, asr_juristic, adjust_method, times);
}

void ST_get_prayer_times_custom(ST_method_config custom_config, int year, int month, int day, double latitude, double longitude, enum ST_juristic_method asr_juristic, enum ST_adjusting_method adjust_method, double times[static ST_prayer_num])
{
    ST_method_params[ST_cm_Custom] = custom_config;
    ST_get_prayer_times(year, month, day, latitude, longitude, ST_cm_Custom, asr_juristic, adjust_method, times);
}
