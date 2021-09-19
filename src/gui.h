#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED
#include <gtk/gtk.h>
#include "config.h"

enum GUI_IDS {
#ifdef LOCALIZATION_THROUGH_CODE
    gui_id_fajr_name,
    gui_id_sunrise_name,
    gui_id_dhuhr_name,
    gui_id_asr_name,
    gui_id_maghrib_name,
    gui_id_isha_name,
#endif // LOCALIZATION_THROUGH_CODE
    gui_id_fajr_time,
    gui_id_fajrend,
    gui_id_sunrise_time,
    gui_id_dhuhr_time,
    gui_id_asr_time,
    gui_id_sunset_time,
    gui_id_maghrib_time,
    gui_id_isha_time,
    gui_id_cityname,
    gui_id_datename,
    gui_id_hijridate,
    gui_id_remainingtime,
    gui_id_randomhadith,

    gui_id_num,
};

void build_glade(Config* cfg_in, size_t num_strings, char* glade_filename, char* strings[num_strings]);
void on_btn_next_city_clicked(GtkWidget* widget, gpointer data);
void on_btn_prev_date_clicked(GtkWidget* widget, gpointer data);
void on_btn_next_date_clicked(GtkWidget* widget, gpointer data);
void on_dlg_calc_error_ok_clicked(GtkWidget* widget, gpointer data);
void dlg_calc_error_retry_btn_clicked(GtkWidget* widget, gpointer data);
void on_dlg_about_response(GtkWidget* dlg_about, gpointer data);
void on_menuitm_load_activate(GtkWidget* widget, gpointer data);
void on_menuitm_movecities_activate(GtkWidget* widget, gpointer data);
void on_menuitm_removecity_activate(GtkWidget* widget, gpointer data);
void on_menuitm_addcity_activate(GtkWidget* widget, gpointer data);
void on_dlg_add_city_search_search_changed(GtkWidget* widget, gpointer data);
void on_menuitm_saveas_activate(GtkWidget* widget, gpointer data);
void on_menuitm_settings_activate(GtkWidget* widget, gpointer data);
void on_window_close(GtkWidget* window, gpointer data);
void build_assistant_glade(void);
void statusicon_clicked(GtkWidget* widget, gpointer data);
void hide_statusicon(GtkStatusIcon* statusicon);
void label_set_text(enum GUI_IDS id, char* text);
bool Callback_Minutes(gpointer data);
bool Callback_Seconds(gpointer data);
void on_btn_prev_city_clicked(GtkWidget* widget, gpointer data);
void on_dlg_add_city_close(GtkWidget* widget, gpointer data);

#endif // GUI_H_INCLUDED
