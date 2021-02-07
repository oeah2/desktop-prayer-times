#include "prayer_times.h"

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
};
/*
char const*const prayer_names[prayers_num] = {
    [pr_fajr] = &prayer_names_int[pr_fajr],
    [pr_fajr_end] = &prayer_names_int[pr_fajr_end],
    [pr_sunrise] = &prayer_names_int[pr_sunrise],
    [pr_dhuhr] = &prayer_names_int[pr_dhuhr],
    [pr_asr] = &prayer_names_int[pr_asr],
    [pr_sunset] = &prayer_names_int[pr_sunset],
    [pr_maghreb] = &prayer_names_int[pr_maghreb],
    [pr_ishaa] = &prayer_names_int[pr_ishaa]
};*/

/*
char const*const prayer_names[prayers_num] = {
    [pr_fajr] = "Fajr",
    [pr_fajr_end] = "End of Fajr",
    [pr_sunrise] = "Sunrise",
    [pr_dhuhr] = "Dhuhr",
    [pr_asr] = "Asr",
    [pr_sunset] = "Sunset",
    [pr_maghreb] = "Maghreb",
    [pr_ishaa] = "Ishaa"
};*/

void prayer_print_times(char const*const city_name, prayer times[prayers_num])
{
    size_t buffer_length = 100;
    char buffer[buffer_length];
    strftime(buffer, buffer_length, "%d.%m.%Y", &times[0].time_at);
    printf("%s Namaz Vakitleri: %s \n", city_name, buffer);
    for(size_t i = 0; i < prayers_num; i++) {
        strftime(buffer, buffer_length, "%H:%M", &times[i].time_at);
        printf("%s\t%s\n", prayer_names[i],buffer);
    }
}

calc_function* calc_functions[] = {
    [prov_diyanet] =  diyanet_get_todays_prayers,
    [prov_calc] = 0,
};
