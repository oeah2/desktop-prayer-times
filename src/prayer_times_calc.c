#include "config.h"
#include "Salah_times_calc.h"

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

static prayer convert_to_prayer(struct tm date, size_t pr, double prayer_double)
{
    intPair time = ST_float_time_decompose(prayer_double);
    date.tm_hour = time.val[0];
    date.tm_min = time.val[1];
    prayer ret = (prayer) {
        .name = prayer_names[pr],
        .time_at = date,
    };
    return ret;
}

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
