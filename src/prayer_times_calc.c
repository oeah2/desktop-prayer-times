#include "prayer_times.h"

char const*const method_names[calculation_method_num] = {
    [jafari] = "jafari",
    [karachi] = "karachi",
    [ISNA] = "ISNA",
    [MWL] = "MWL",
    [makkah] = "makkah",
    [egypt] = "egypt",
    [custom] = "custom",
};

enum AdjustingMethod
{
    None,      	// No adjustment
    MidNight,  	// middle of night
    OneSeventh,	// 1/7th of night
    AngleBased,	// angle/60th of night
};


/*************************
* Ab hier muss ich alles integrieren
**************************/

/*
void get_prayer_times(int year, int month, int day, double _latitude, double _longitude, double _timezone, double times[])
	{
		latitude = _latitude;
		longitude = _longitude;
		timezone = _timezone;
		julian_date = get_julian_date(year, month, day) - longitude / (double) (15 * 24);
		compute_day_times(times);
	}
*/
typedef struct DoublePair DoublePair;

struct DoublePair {
    double val[2];
};

static DoublePair DoublePair_init(double a, double b) {
    DoublePair dp = {
        .val = {a, b},
    };
    return dp;
}

/* compute declination angle of sun and equation of time */
DoublePair sun_position(double jd)
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

    return DoublePair_init(dd, eq_t);
}
