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

int sprint_prayer_time(prayer time, size_t buff_len, char dest[buff_len])
{
    char* format = (time.time_at.tm_min < 10) ? "%d:0%d" : "%d:%d";
    return sprintf_s(dest, buff_len, format, time.time_at.tm_hour, time.time_at.tm_min);
}

int sprint_prayer_remaining(size_t buff_len, char dest[buff_len], int hours, int minutes, int seconds)
{
    char* format = 0;
    if(seconds < 10 && minutes < 10) format = "%d:0%d:0%d";
    else if(seconds < 10 && minutes >= 10) format = "%d:%d:0%d";
    else if(seconds >= 10 && minutes < 10) format = "%d:0%d:%d";
    else if(seconds >= 10 && minutes >= 10) format = "%d:%d:%d";
    assert(format);
    return sprintf_s(dest, buff_len, format, hours, minutes, seconds);
}

int sprint_prayer_date(prayer time, size_t buff_len, char dest[buff_len], bool hijri)
{
    if(!hijri)
        return sprintf_s(dest, buff_len, "%d.%d.%d", time.time_at.tm_mday, time.time_at.tm_mon + 1, time.time_at.tm_year + 1900);
    else
        return sprintf_s(dest, buff_len, "%d.%d.%d", time.hicri_date.tm_mday, time.hicri_date.tm_mon, time.hicri_date.tm_year);
}

int prayer_calc_remaining_time(prayer next, int* hours, int* minutes, int* seconds)
{
    time_t now = 0, remaining = 0;
    int ret = EXIT_FAILURE;
    now = time(0);

    time_t prayer = mktime(&next.time_at);
    remaining = difftime(prayer, now);
    if(remaining > 0) {
        *hours = remaining / (60*60);
        *minutes = (remaining - *hours * 60 * 60) / 60;
        *seconds = remaining - *hours * 60 * 60 - *minutes * 60;
        ret = EXIT_SUCCESS;
    }
    return ret;
}
