#include "lang.h"
#include "gui.h"

char const*const lang_names[LANG_NUM] = {
    [LANG_EN] = "en",
    [LANG_DE] = "de",
};

char* gui_strings[gui_id_num] = {
    [gui_id_fajr_name]      = "Fajr",
    [gui_id_sunrise_name]   = "Sunrise",
    [gui_id_dhuhr_name]     = "Dhuhr",
    [gui_id_asr_name]       = "Asr",
    [gui_id_maghrib_name]   = "Maghrib",
    [gui_id_isha_name]      = "Isha",
    [gui_id_fajr_time]      = "label_time_fajr",
    [gui_id_sunrise_time]   = "label_time_sunrise",
    [gui_id_dhuhr_time]     = "label_time_dhuhr",
    [gui_id_asr_time]       = "Asr time",
    [gui_id_maghrib_time]   = "label_time_maghrib",
    [gui_id_isha_time]      = "label_time_isha",
    //[gui_id_cityname]       = config.cities[0].name,
    [gui_id_datename]       = "Heute",
    [gui_id_hijridate]      = "Heute Hijri",
    [gui_id_remainingtime]  = "Noch bisschen",
    //[gui_id_randomhadith]   = "Hadith",
};
