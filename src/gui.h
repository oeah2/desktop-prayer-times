#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED
#include <gtk/gtk.h>
#include "config.h"

enum GUI_IDS {
    gui_id_fajr_name,
    gui_id_sunrise_name,
    gui_id_dhuhr_name,
    gui_id_asr_name,
    gui_id_maghrib_name,
    gui_id_isha_name,
    gui_id_fajr_time,
    gui_id_sunrise_time,
    gui_id_dhuhr_time,
    gui_id_asr_time,
    gui_id_maghrib_time,
    gui_id_isha_time,
    gui_id_cityname,
    gui_id_datename,
    gui_id_hijridate,
    gui_id_remainingtime,
    gui_id_randomhadith,

    gui_id_num,
};

void build_glade(Config* cfg_in, size_t num_strings, char* strings[num_strings]);

#endif // GUI_H_INCLUDED
