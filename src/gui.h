/*
   Desktop Prayer Times app
   Copyright (C) 2021 Ahmet Öztürk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

#ifdef OLD
void build_glade(Config* cfg_in, size_t num_strings, char* glade_filename, char* strings[num_strings]);
void on_btn_next_city_clicked(GtkWidget* widget, gpointer data);
void on_btn_prev_date_clicked(GtkWidget* widget, gpointer data);
void on_btn_next_date_clicked(GtkWidget* widget, gpointer data);
void on_dlg_calc_error_ok_clicked(GtkWidget* widget, gpointer data);
void dlg_calc_error_retry_btn_clicked(GtkWidget* widget, gpointer data);
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
#else

// General Functions
void label_append_text(GtkLabel* label, char const*const text);
GtkWidget* find_child(GtkWidget* parent, const char*const name);
void sprint_dates(prayer times, size_t buff_len, char dest_julian_date[buff_len], char dest_hijri_date[buff_len]);

// Main Window Functions
void display_city(City city);
void display_empty_city(void);
void build_glade(Config* cfg_in, size_t num_strings, char* glade_filename, char* strings[num_strings]);

// Menuitem
void on_menuitm_addcity_activate(GtkWidget* widget, gpointer data);
void on_menuitm_removecity_activate(GtkWidget* widget, gpointer data);
void on_menuitm_movecities_activate(GtkWidget* widget, gpointer data);
void on_menuitm_saveas_activate(GtkWidget* widget, gpointer data);
void on_menuitm_load_activate(GtkWidget* widget, gpointer data);
void on_menuitm_about_activate(GtkWidget* menuitm, gpointer data);

// Assistant
void on_assistant_addcity_diyanet_combobox_country_changed(GtkWidget* widget, gpointer data);
void on_assistant_addcity_diyanet_combobox_province_changed(GtkWidget* widget, gpointer data) ;
void on_assistant_addcity_diyanet_combobox_city_changed(GtkWidget* widget, gpointer data);
int assistant_addcity_nextpage_func(int current_page, gpointer data) ;
void on_assistant_addcity_prepare(GtkWidget* widget, gpointer data) ;
void on_assistant_addcity_cancel(GtkWidget* widget, gpointer data) ;
void on_assistant_add_city_page1_search_search_changed(GtkWidget* widget, gpointer data);
void on_dlg_add_city_close(GtkWidget* widget, gpointer data);

// Listbox functions
GtkListBox* gtk_listbox_clear_assistant_addcity(GtkListBox* listbox);
GtkListBox* gtk_listbox_clear_dialog_removeCity(GtkListBox* listbox);
GtkCheckButton* gui_create_and_add_check_button(GtkListBox* listbox, char const*const label, char const*const name);
GtkRadioButton* gui_create_and_add_radio_button(GtkListBox* listbox, GtkRadioButton* group, char const*const label, char const*const name);

// Settings
void on_menuitm_settings_activate(GtkWidget* widget, gpointer data);

#endif

#endif // GUI_H_INCLUDED
