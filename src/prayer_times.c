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

void prayer_print_times(char const*const city_name, prayer times[prayers_num])
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

calc_function* calc_functions[] = {
    [prov_diyanet] =  diyanet_get_todays_prayers,
    [prov_calc] = calc_get_todays_prayers,
}; /**< Array of function pointers. During calculation, one of these is being called, depending on the configuration of the city */

preview_function* preview_functions[] = {
    [prov_diyanet] = diyanet_get_preview_for_date,
    [prov_calc] = calc_get_preview_for_date,
};
