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


#include <stdlib.h>
#include <stdbool.h>
//#define NDEBUG 1
#include <assert.h>
#include <pthread.h>
#include "gui.h"
#include "geolocation.h"
#include "cJSON.h"
#include "config.h"
#include "socket.h"
#include "update.h"
#include "error.h"
#include "hadith.h"

#define USE_STATUSICON
//#define SHOW_MOON         // not really implemented
#define JUMP_TO_NEXT_DAY    // At 00:00, the next day will be displayed

extern calc_function* calc_functions[];
extern preview_function* preview_functions[];

static char* gui_identifiers[gui_id_num] = {
#ifdef LOCALIZATION_THROUGH_CODE
    [gui_id_fajr_name]      = "label_name_fajr",
    [gui_id_sunrise_name]   = "label_name_sunrise",
    [gui_id_dhuhr_name]     = "label_name_dhuhr",
    [gui_id_asr_name]       = "label_name_asr",
    [gui_id_maghrib_name]   = "label_name_maghrib",
    [gui_id_isha_name]      = "label_name_isha",
#endif // LOCALIZATION_THROUGH_CODE
    [gui_id_fajr_time]      = "label_time_fajr",
    [gui_id_fajrend]        = "label_time_fajrend",
    [gui_id_dhuhr_time]     = "label_time_dhuhr",
    [gui_id_asr_time]       = "label_time_asr",
    [gui_id_maghrib_time]   = "label_time_maghrib",
    [gui_id_isha_time]      = "label_time_isha",
    [gui_id_sunrise_time]   = "label_time_sunrise",
    [gui_id_sunset_time]    = "label_time_sunset",
    [gui_id_cityname]       = "label_cityname",
    [gui_id_datename]       = "label_date",
    [gui_id_hijridate]      = "label_hicridate",
    [gui_id_remainingtime]  = "label_remaining_time",
    [gui_id_randomhadith]   = "label_randomhadith",
};


Config* cfg;
size_t city_ptr = 0;
int day_ptr = 0;
GtkLabel* labels[gui_id_num];
GtkButton* btn_prev_date = 0;
GtkButton* btn_next_date = 0;
GtkButton* btn_prev_city = 0;
GtkButton* btn_next_city = 0;
GtkDialog* dlg_calc_error = 0;
GtkDialog* dlg_hadith = 0;
#ifdef SHOW_MOON
GtkImage* img_mondphase = 0;
#endif // SHOW_MOON
#ifdef USE_STATUSICON
GtkStatusIcon* statusicon;
#endif // USE_STATUSICON

void hide_statusicon(GtkStatusIcon* statusicon);
void label_set_text(enum GUI_IDS id, char* text);
bool Callback_Minutes(gpointer data);
bool Callback_Seconds(gpointer data);
void on_btn_prev_city_clicked(GtkWidget* widget, gpointer data);
static void dlg_calc_error_main(GtkDialog* dlg_calc_error);

#ifdef USE_STATUSICON
void hide_statusicon(GtkStatusIcon* statusiconlcl)
{
    gtk_status_icon_set_visible(GTK_STATUS_ICON(statusiconlcl), false);
}
#endif // USE_STATUSICON

void label_set_text(enum GUI_IDS id, char* text)
{
    gtk_label_set_text(GTK_LABEL(labels[id]), text);
}

/** \brief Calc prayers for @p city and write into @p prayer_times
 *
 */
static int gui_calc_prayer(City city, prayer prayer_times[prayers_num])
{
    calc_function* calc = calc_functions[city.pr_time_provider];
    int ret = calc(city, prayer_times);
    if(ret != EXIT_SUCCESS) {
    	dlg_calc_error_main(dlg_calc_error);
    }
    return ret;
}

/** \brief Calc prayers for @p city and write into @p dest and @p dest_hijri_date and @p dest_julian_date
 *
 */
static int gui_calc_prayer_times(City city, size_t dest_len, char dest[prayers_num][dest_len], size_t buff_len, char dest_julian_date[buff_len], char dest_hijri_date[buff_len])
{
    prayer prayer_times[prayers_num];
    int ret = gui_calc_prayer(city, prayer_times);

    for(size_t i = 0, pos = 0; i < prayers_num; i++) {
        sprint_prayer_time(prayer_times[i], dest_len, dest[pos]);
        pos++;
    }
    if(city.pr_time_provider == prov_calc) {
        prayer_times[0].hicri_date = calc_get_hijri_date(prayer_times[0].time_at);
    }
    sprint_dates(prayer_times[0], buff_len, dest_julian_date, dest_hijri_date);
    return ret;
}

/** \brief Calc prayers for @p city and display this city on main window
 *
 */
void display_city(City city)
{
    size_t buff_len = 20;
    char times[prayers_num][buff_len];
    char hijri_date[buff_len];
    char julian_date[buff_len];
    gui_calc_prayer_times(city, buff_len, times, buff_len, julian_date, hijri_date);

    for(size_t i = gui_id_fajr_time; i <= gui_id_isha_time; i++) {
        gtk_label_set_text(labels[i], times[i - gui_id_fajr_time]);
    }
    gtk_label_set_text(labels[gui_id_cityname], city.name);
    gtk_label_set_text(labels[gui_id_datename], julian_date);
    gtk_label_set_text(labels[gui_id_hijridate], hijri_date);
}

/** \brief Display empty city on main window
 *
 */
void display_empty_city(void) {
	City c = {0};
	c = *city_init_empty(&c);
	display_city(c);
}

/** \brief Calc prayer-previews (for @p days in future) for @p city and write into @p prayer_times.
 *
 * \param days can be negative for past days
 */
static int gui_calc_preview(City city, prayer prayer_times[prayers_num], int days)
{
    preview_function* preview = preview_functions[city.pr_time_provider];

    double const SECS_PER_DAY = 60*60*24;
    time_t date_t = time(0) + SECS_PER_DAY * days;
    date_t -= date_t % 86400;
    struct tm date = *gmtime(&date_t);

    int ret = preview(city, prayer_times, date);
    return ret;
}

/** \brief Calc prayer-previews (for @p days in future) for @p city and write into @p dest and @p dest_julian_date and @p dest_hijri_date.
 *
 * \param days can be negative for past days
 */
static int gui_calcpreview_prayer(City city, size_t dest_len, char dest[prayers_num][dest_len], size_t buff_len, char dest_julian_date[buff_len], char dest_hijri_date[buff_len], int days)
{
    prayer prayer_times[prayers_num];
    int ret = gui_calc_preview(city, prayer_times, days);
    if(ret != EXIT_SUCCESS) return ret;

    for(size_t i = 0, pos = 0; i < prayers_num; i++) {
        sprint_prayer_time(prayer_times[i], dest_len, dest[pos]);
        pos++;
    }
    sprint_dates(prayer_times[0], buff_len, dest_julian_date, dest_hijri_date);
    return ret;
}

/** \brief Display preview (for @p days in future) for @p city on main window
 *
 * \param days can be negative for past days
 */
static int display_preview(City city, int days)
{
    size_t buff_len = 20;
    char times[prayers_num][buff_len];
    char julian_date[buff_len];
    char hijri_date[buff_len];

    int ret = gui_calcpreview_prayer(city, buff_len, times, buff_len, julian_date, hijri_date, days);
    if(ret != EXIT_SUCCESS) {
        return ret;
    }

    for(size_t i = gui_id_fajr_time; i <= gui_id_isha_time; i++) {
        gtk_label_set_text(labels[i], times[i - gui_id_fajr_time]);
    }
    gtk_label_set_text(labels[gui_id_datename], julian_date);
    gtk_label_set_text(labels[gui_id_hijridate], hijri_date);
    return ret;
}

static bool gui_apply_language(size_t lang_id)
{
    bool ret = true;
    return ret;
}

bool Callback_Minutes(gpointer data)
{
    bool ret = true;
    return ret;
}

/** \brief Callback function for secondly update of main window
 *
 */
bool Callback_Seconds(gpointer data)
{
    prayer prayer_times[prayers_num];
    if(cfg->num_cities == 0 || cfg->cities[city_ptr].pr_time_provider == prov_empty) {
        gtk_label_set_text(GTK_LABEL(labels[gui_id_remainingtime]), "");
    	display_empty_city();
    	return true;
    } else if(city_ptr >= cfg->num_cities) {
    	city_ptr = 0;
    }
    gui_calc_prayer(cfg->cities[city_ptr], prayer_times);

    int rem_hours = 0, rem_mins = 0, rem_secs = 0, ret_val = 0;
CALC_REMAINING:
    for(size_t i = 0; i < prayers_num; i++) {
        if(i == pr_sunrise || i == pr_sunset) continue;
        ret_val = prayer_calc_remaining_time(prayer_times[i], &rem_hours, &rem_mins, &rem_secs);
        if(ret_val == ENETDOWN) cfg->cities[city_ptr].pr_time_provider = prov_empty;
        else if(ret_val == EXIT_FAILURE) continue;
        size_t buff_len = 50;
        char buffer[buff_len];
        char buffer_prayertime[buff_len];
        sprint_prayer_time(prayer_times[i], buff_len, buffer_prayertime);
        sprintf(buffer, "Next prayer for %s: %s", cfg->cities[city_ptr].name, buffer_prayertime);
        gtk_status_icon_set_tooltip_text(statusicon, buffer);
        break;
    }

    if(ret_val == EXIT_FAILURE) {
        int ret = gui_calc_preview(cfg->cities[city_ptr], prayer_times, 1);
        if(ret == EXIT_SUCCESS)
            goto CALC_REMAINING;
        else {
        	dlg_calc_error_main(dlg_calc_error);
            cfg->cities[city_ptr].pr_time_provider = prov_empty;
        }
    }

    static time_t last_calc_day = 0;
    if(time(0) - last_calc_day > 24 * 60 * 60) {         // new day
        last_calc_day = time(0);
        last_calc_day -= last_calc_day % (24 * 60 * 60);    // auf tag abrunden
#ifdef JUMP_TO_NEXT_DAY
        static bool initial_run = true;
        if(!initial_run) {
            day_ptr++;
            display_preview(cfg->cities[city_ptr], day_ptr);
        }
        initial_run = false;
#endif // JUMP_TO_NEXT_DAY
        // Update times and days
        //goto CALC_REMAINING;
    }

    size_t buff_len = 15;
    char remaining_time[buff_len];
    sprint_prayer_remaining(buff_len, remaining_time, rem_hours, rem_mins, rem_secs);

    gtk_label_set_text(GTK_LABEL(labels[gui_id_remainingtime]), remaining_time);

    return true;
}

/** \brief Handler for previous city button.
 *
 */
void on_btn_prev_city_clicked(GtkWidget* widget, gpointer data)
{
    if(city_ptr > 0) {
        day_ptr = 0;    /* Reset to be safe for Diyanet? */
        city_ptr--;
        if(city_ptr == 0) gtk_widget_set_sensitive(widget, false);

        /* Reset Date Buttons */
        gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_date), cfg->cities[city_ptr].pr_time_provider != prov_diyanet);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_next_date), true);

        display_city(cfg->cities[city_ptr]);
        GtkWidget* btn_next_city_lcl = data;
        gtk_widget_set_sensitive(btn_next_city_lcl, true);
        Callback_Seconds(NULL);
    }
}

/** \brief Handler for next city button.
 *
 */
void on_btn_next_city_clicked(GtkWidget* widget, gpointer data)
{
    if(city_ptr < cfg->num_cities - 1) {
        day_ptr = 0;    /* Reset to be safe for Diyanet? */
        city_ptr++;
        if(city_ptr == cfg->num_cities - 1) {
            gtk_widget_set_sensitive(widget, false);
        }

        /* Reset Date Buttons */
        gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_date), cfg->cities[city_ptr].pr_time_provider != prov_diyanet);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_next_date), true);

        display_city(cfg->cities[city_ptr]);
        GtkWidget* btn_prev_city_lcl = data;
        gtk_widget_set_sensitive(btn_prev_city_lcl, true);
        Callback_Seconds(NULL);
        if(cfg->cities[city_ptr].pr_time_provider == prov_calc) {
            //calc_get_hijri_date(prayer_times[0].time_at);     // not working currently. the website seems to be very slow.
        }
    }
}

/** \brief Handler for previous date button.
 *
 */
void on_btn_prev_date_clicked(GtkWidget* widget, gpointer data)
{
    if(day_ptr <= 0 && cfg->cities[city_ptr].pr_time_provider == prov_calc) {
        day_ptr--;
    } else if(day_ptr > 0) {
        day_ptr--;
    }
    if(day_ptr == 0 && cfg->cities[city_ptr].pr_time_provider == prov_diyanet) {
        gtk_widget_set_sensitive(widget, false);
    }
    GtkWidget* button_next_date = data;
    gtk_widget_set_sensitive(button_next_date, true);

    int ret = display_preview(cfg->cities[city_ptr], day_ptr);
    if(ret != EXIT_SUCCESS) {
        assert(0);  // Sollte nicht passieren
    }
}

/** \brief Handler for next city button.
 *
 */
void on_btn_next_date_clicked(GtkWidget* widget, gpointer data)
{
    day_ptr++;
    int ret = display_preview(cfg->cities[city_ptr], day_ptr);
    if(ret != EXIT_SUCCESS) {
        day_ptr--;
        assert(cfg->cities[city_ptr].pr_time_provider == prov_diyanet);
        gtk_widget_set_sensitive(widget, false);

    }

    GtkWidget* button_prev_date = data;
    gtk_widget_set_sensitive(button_prev_date, true);
}

/** \brief Main function for Calculation error dialog.
 *
 */
static void dlg_calc_error_main(GtkDialog* dlg_calc_error) {
	assert(dlg_calc_error);

	gtk_widget_show(GTK_WIDGET(dlg_calc_error));
	int ret_dlg = 0;
	City* city = 0;
RUN:
	ret_dlg = gtk_dialog_run(dlg_calc_error);
	switch(ret_dlg) {
	case GTK_RESPONSE_YES:
		// Retry
	    city = &(cfg->cities[city_ptr]);
	    if(city->pr_time_provider == prov_diyanet) {
	        if(!socket_check_connection()) {
	            myperror("dlg_calc_error_retry_btn_clicked: No internet connection");
	    	    GtkLabel* label = GTK_LABEL(find_child(GTK_WIDGET(dlg_calc_error), "dlg_calc_error_label"));
	    	    label_append_text(label, language_specific_strings[LangStrings_CalcError_NoConnection]);
	        }
	    } else if(city->pr_time_provider == prov_calc) {
	    	myperror("dlg_calc_error_retry_btn_clicked, Error for prov_calc");
    	    GtkLabel* label = GTK_LABEL(find_child(GTK_WIDGET(dlg_calc_error), "dlg_calc_error_label"));
    	    label_append_text(label, language_specific_strings[LangStrings_CalcError_Unknown]);
	    }
		goto RUN;

	case GTK_RESPONSE_OK:
	default:
		if(cfg->num_cities == 1 || cfg->num_cities == 0) {
			display_empty_city();
		} else {
			city_ptr = city_ptr == 0 ? city_ptr + 1 : city_ptr - 1;
			display_city(cfg->cities[city_ptr]);
		}
		break;
	}
	gtk_widget_hide(GTK_WIDGET(dlg_calc_error));
}

/** \brief Handler for Hadith-Update
 *
 */
void on_label_randomhadith_clicked(GtkWidget* widget, gpointer data) {
	GtkLabel* random_hadith = GTK_LABEL(widget);
	assert(GTK_IS_LABEL(random_hadith));
	char* hadith = hadith_get_random();

	if(hadith) {
		if(strlen(hadith) < 240) {
			gtk_label_set_text(random_hadith, hadith);
			free(hadith);
		} else {
			GtkLabel* dlg_hadith_label = GTK_LABEL(find_child(GTK_WIDGET(dlg_hadith), "dlg_hadith_label"));
			gtk_label_set_text(dlg_hadith_label, hadith);

		    gtk_widget_show(GTK_WIDGET(dlg_hadith));
		    gtk_dialog_run(GTK_DIALOG(dlg_hadith)); // This Dialog can't do anything, ignore response
		    gtk_widget_hide(GTK_WIDGET(dlg_hadith));
		}
	}
}

/** \brief Handler for closing of main window.
 *
 */
void on_window_close(GtkWidget* window, gpointer data)
{
    if(cfg->save_position) {
        int posX = 0, posY = 0;
        gtk_window_get_position(GTK_WINDOW(window), &posX, &posY);
        if(posX != cfg->last_window_posX || posY != cfg->last_window_posY || cfg->config_changed) {
            cfg->config_changed = true;
            cfg->last_window_posX = posX;
            cfg->last_window_posY = posY;
            config_json_save(cfg->cfg_filename, cfg);
        }
    }

    gtk_widget_destroy(window);
    gtk_main_quit();
}

/** \brief Assistant for glade
 *
 */
void build_assistant_glade(void)
{
    GtkBuilder          *builder;
    GtkAssistant        *window_assistant;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "Prayer_times_GTK.glade", NULL);

    window_assistant = GTK_ASSISTANT(gtk_builder_get_object(builder, "window_assistant"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show(GTK_WIDGET(window_assistant));
    g_signal_connect(window_assistant, "destroy", G_CALLBACK(gtk_main_quit),
                     NULL);

}

#ifdef SHOW_MOON
void show_moonphase(unsigned char* data)
{
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_stream(data, NULL, NULL);
    if(pixbuf) {

    }
}
#endif // SHOW_MOON

/** \brief Handler for click of status icon
 *
 */
void statusicon_clicked(GtkWidget* widget, gpointer data)
{
    gtk_window_present(GTK_WINDOW(widget));
}

#define CHECK_OBJ(x)        \
 do {                       \
    if(!(x)) {              \
        err_msg = "" #x "";\
        goto ERR_OBJ;       \
    }                       \
 } while(false)

/** \brief Load glade GUI file, read buttons and connect signals.
 *
 */
void build_glade(Config* cfg_in, size_t num_strings, char* glade_filename, char* strings[num_strings])
{
    GtkBuilder      *builder;
    GtkWidget       *window_main;
    cfg = cfg_in;
    char* err_msg = NULL;

    builder = gtk_builder_new();
    {
        int gtk_builder_ret = gtk_builder_add_from_file(builder, glade_filename, NULL);
        if(!gtk_builder_ret) {
        	myperror("Error parsing GUI file!");
            assert(gtk_builder_ret);
        }
    }

    window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main")); CHECK_OBJ(window_main);
    /* Connect Button signals */
    /* Buttons city */
    btn_prev_city = GTK_BUTTON(gtk_builder_get_object(builder, "button_prev_city")); CHECK_OBJ(btn_prev_city);
    btn_next_city = GTK_BUTTON(gtk_builder_get_object(builder, "button_next_city")); CHECK_OBJ(btn_next_city);
    g_signal_connect(btn_prev_city, "clicked", G_CALLBACK(on_btn_prev_city_clicked), btn_next_city);
    g_signal_connect(btn_next_city, "clicked", G_CALLBACK(on_btn_next_city_clicked), btn_prev_city);
    /* Buttons date */
    btn_prev_date = GTK_BUTTON(gtk_builder_get_object(builder, "button_prev_date")); CHECK_OBJ(btn_prev_date);
    btn_next_date = GTK_BUTTON(gtk_builder_get_object(builder, "button_next_date")); CHECK_OBJ(btn_next_date);
    g_signal_connect(btn_prev_date, "clicked", G_CALLBACK(on_btn_prev_date_clicked), btn_next_date);
    g_signal_connect(btn_next_date, "clicked", G_CALLBACK(on_btn_next_date_clicked), btn_prev_date);

    /* Get Label IDs */
    if(strings) {
        for(size_t i = 0; i < gui_id_num; i++) {
            GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, gui_identifiers[i]));
            CHECK_OBJ(label);
            labels[i] = label;
            if(strings[i]) gtk_label_set_text(labels[i], strings[i]);
        }
    }

    /* Disable buttons, if num cities is below 2 */
    gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_city), false);     // Always disabled, because initially first city is displayed
    if(cfg->num_cities < 2) {
        gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), false);
    }

    if(cfg->cities[city_ptr].pr_time_provider == prov_diyanet) {
        gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_date), false);
    }

#ifdef SHOW_MOON
    /* Mondphasen */
    img_mondphase = gtk_builder_get_object(builder, "image-mond");
    CHECK_OBJ(img_mondphase);
#endif // SHOW_MOON

    gtk_widget_show(window_main);
    if(cfg->save_position)
        gtk_window_move(GTK_WINDOW(window_main), cfg->last_window_posX, cfg->last_window_posY);
    g_signal_connect(window_main, "delete-event", G_CALLBACK(on_window_close), NULL);

    g_timeout_add_seconds(1, G_SOURCE_FUNC(Callback_Seconds), NULL);
    Callback_Seconds(NULL);

    /* Dialog window for calculation error */
    dlg_calc_error = GTK_DIALOG(gtk_builder_get_object(builder, "dlg_calc_error"));
    CHECK_OBJ(dlg_calc_error);

    /* Dialog for showing long hadithes */
    dlg_hadith = GTK_DIALOG(gtk_builder_get_object(builder, "dlg_hadith"));
    CHECK_OBJ(dlg_hadith);

    /* Callback functions */
    gtk_builder_add_callback_symbol(builder, "on_menuitm_load_activate", G_CALLBACK(on_menuitm_load_activate));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_saveas_activate", G_CALLBACK(on_menuitm_saveas_activate));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_settings_activate", G_CALLBACK(on_menuitm_settings_activate));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_movecities_activate", G_CALLBACK(on_menuitm_movecities_activate));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_removecity_activate", G_CALLBACK(on_menuitm_removecity_activate));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_addcity_activate", G_CALLBACK(on_menuitm_addcity_activate));
    gtk_builder_add_callback_symbol(builder, "on_assistant_add_city_page1_search_search_changed", G_CALLBACK(on_assistant_add_city_page1_search_search_changed));
    gtk_builder_add_callback_symbol(builder, "on_dlg_add_city_close", G_CALLBACK(on_dlg_add_city_close));
    gtk_builder_add_callback_symbol(builder, "on_assistant_addcity_prepare", G_CALLBACK(on_assistant_addcity_prepare));
    gtk_builder_add_callback_symbol(builder, "on_assistant_addcity_cancel", G_CALLBACK(on_assistant_addcity_cancel));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_about_activate", G_CALLBACK(on_menuitm_about_activate));
    gtk_builder_add_callback_symbol(builder, "on_assistant_addcity_diyanet_combobox_country_changed", G_CALLBACK(on_assistant_addcity_diyanet_combobox_country_changed));
    gtk_builder_add_callback_symbol(builder, "on_assistant_addcity_diyanet_combobox_province_changed", G_CALLBACK(on_assistant_addcity_diyanet_combobox_province_changed));
    gtk_builder_add_callback_symbol(builder, "on_assistant_addcity_diyanet_combobox_city_changed", G_CALLBACK(on_assistant_addcity_diyanet_combobox_city_changed));
    gtk_builder_add_callback_symbol(builder, "on_label_randomhadith_clicked", G_CALLBACK(on_label_randomhadith_clicked));


    // Todo check
    GtkAssistant* assistant_addcity = GTK_ASSISTANT(gtk_builder_get_object(builder, "assistant_addcity")); CHECK_OBJ(assistant_addcity);
	gtk_assistant_set_forward_page_func(assistant_addcity, assistant_addcity_nextpage_func, assistant_addcity, NULL);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

//#define SHOW_NOTIFICATIONS
#ifdef SHOW_NOTIFICATIONS   // to be implemented
    //GApplication* application = g_application_new("app.desktop_prayer_times", 0);
    GApplication* application = g_application_new(NULL, 0);
    g_application_activate(application);
    g_application_run(application, 0, NULL);
    GNotification* notification_test = g_notification_new("Test");
    g_application_send_notification(application, "desktop_prayer_time:prayer_time", notification_test);
#endif // SHOW_NOTIFICATIONS

#ifdef USE_STATUSICON
    /* Status Icon */
    statusicon = gtk_status_icon_new_from_file("Graphics/Logo_small.png");
    assert(statusicon);
    g_signal_connect_swapped(statusicon, "activate", G_CALLBACK(statusicon_clicked), window_main);
    g_signal_connect_swapped(window_main, "destroy", G_CALLBACK(hide_statusicon), statusicon);
#endif // USE_STATUSICON
    return;

ERR_OBJ:
	myperror("Error finding object with gtk_builder_get_object");
	myperror(err_msg);
    return;
}
