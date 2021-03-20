#include <stdlib.h>
#include <stdbool.h>
//#define NDEBUG 1
#include <assert.h>
#include "gui.h"
#include "geolocation.h"
#include "cJSON.h"
#include "config.h"
#include "socket.h"

#define USE_STATUSICON
//#define SHOW_MOON         // not really implemented

extern calc_function* calc_functions[];
extern preview_function* preview_functions[];

static char* gui_identifiers[gui_id_num] = {
    [gui_id_fajr_name]      = "label_name_fajr",
    [gui_id_sunrise_name]   = "label_name_sunrise",
    [gui_id_dhuhr_name]     = "label_name_dhuhr",
    [gui_id_asr_name]       = "label_name_asr",
    [gui_id_maghrib_name]   = "label_name_maghrib",
    [gui_id_isha_name]      = "label_name_isha",
    [gui_id_fajr_time]      = "label_time_fajr",
    [gui_id_sunrise_time]   = "label_time_sunrise",
    [gui_id_dhuhr_time]     = "label_time_dhuhr",
    [gui_id_asr_time]       = "label_time_asr",
    [gui_id_maghrib_time]   = "label_time_maghrib",
    [gui_id_isha_time]      = "label_time_isha",
    [gui_id_cityname]       = "label_cityname",
    [gui_id_datename]       = "label_date",
    [gui_id_hijridate]      = "label_hicridate",
    [gui_id_remainingtime]  = "label_remaining_time",
    [gui_id_randomhadith]   = "label_randomhadith",
};

GtkLabel* labels[gui_id_num];
static Config* cfg;
static size_t city_ptr = 0;
static int day_ptr = 0;
GtkButton* btn_prev_date = 0;
GtkButton* btn_next_date = 0;
GtkButton* btn_prev_city = 0;
GtkButton* btn_next_city = 0;
#ifdef SHOW_MOON
GtkImage* img_mondphase = 0;
#endif // SHOW_MOON
GtkDialog* dlg_calc_error = 0;
#ifdef USE_STATUSICON
GtkStatusIcon* statusicon;
#endif // USE_STATUSICON

#ifdef USE_STATUSICON
void hide_statusicon(GtkStatusIcon* statusicon)
{
    gtk_status_icon_set_visible(GTK_STATUS_ICON(statusicon), false);
}
#endif // USE_STATUSICON

void label_set_text(enum GUI_IDS id, char* text)
{
    gtk_label_set_text(GTK_LABEL(labels[id]), text);
}

static int gui_calc_prayer(City city, prayer prayer_times[prayers_num])
{
    calc_function* calc = calc_functions[city.pr_time_provider];
    int ret = calc(city, prayer_times);
    if(ret != EXIT_SUCCESS) {
        gtk_widget_show(GTK_WIDGET(dlg_calc_error));
    }
    return ret;
}

static void sprint_dates(prayer times, size_t buff_len, char dest_julian_date[buff_len], char dest_hijri_date[buff_len])
{
    sprint_prayer_date(times, buff_len, dest_julian_date, false);
    sprint_prayer_date(times, buff_len, dest_hijri_date, true);
}

static int gui_calc_prayer_times(City city, size_t dest_len, char dest[prayers_num][dest_len], size_t buff_len, char dest_julian_date[buff_len], char dest_hijri_date[buff_len])
{
    prayer prayer_times[prayers_num];
    int ret = gui_calc_prayer(city, prayer_times);

    for(size_t i = 0, pos = 0; i < prayers_num; i++) {
        if(i == pr_sunrise || i == pr_sunset) continue;
        sprint_prayer_time(prayer_times[i], dest_len, dest[pos]);
        pos++;
    }
    sprint_dates(prayer_times[0], buff_len, dest_julian_date, dest_hijri_date);
    return ret;
}

static void display_city(City city)
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
    gtk_label_set_text(labels[gui_id_hijridate], (city.pr_time_provider == prov_diyanet) ? hijri_date : "");
}

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

static int gui_calcpreview_prayer(City city, size_t dest_len, char dest[prayers_num][dest_len], size_t buff_len, char dest_julian_date[buff_len], char dest_hijri_date[buff_len], int days)
{
    prayer prayer_times[prayers_num];
    int ret = gui_calc_preview(city, prayer_times, days);
    if(ret != EXIT_SUCCESS) return ret;

    for(size_t i = 0, pos = 0; i < prayers_num; i++) {
        if(i == pr_sunrise || i == pr_sunset) continue;
        sprint_prayer_time(prayer_times[i], dest_len, dest[pos]);
        pos++;
    }
    sprint_dates(prayer_times[0], buff_len, dest_julian_date, dest_hijri_date);
    return ret;
}

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
    gtk_label_set_text(labels[gui_id_hijridate], (city.pr_time_provider == prov_diyanet) ? hijri_date : "");
    return ret;
}

bool Callback_Minutes(gpointer data)
{
    bool ret = true;
    return ret;
};

bool Callback_Seconds(gpointer data)
{
    prayer prayer_times[prayers_num];
    gui_calc_prayer(cfg->cities[city_ptr], prayer_times);

    int rem_hours = 0, rem_mins = 0, rem_secs = 0, ret_val = 0;
CALC_REMAINING:
    for(size_t i = 0; i < prayers_num; i++) {
        if(i == pr_sunrise || i == pr_sunset) continue;
        if((ret_val = prayer_calc_remaining_time(prayer_times[i], &rem_hours, &rem_mins, &rem_secs)) == EXIT_FAILURE) continue;
        size_t buff_len = 50;
        char buffer[buff_len];
        char buffer_prayertime[buff_len];
        sprint_prayer_time(prayer_times[i], buff_len, buffer_prayertime);
        sprintf(buffer, "Next prayer for %s: %s", cfg->cities[city_ptr].name, buffer_prayertime);
        gtk_status_icon_set_tooltip_text(GTK_STATUS_ICON(statusicon), buffer);
        break;
    }

    if(ret_val == EXIT_FAILURE) {
        gui_calc_preview(cfg->cities[city_ptr], prayer_times, 1);
        goto CALC_REMAINING;
    }

    size_t buff_len = 15;
    char remaining_time[buff_len];
    sprint_prayer_remaining(buff_len, remaining_time, rem_hours, rem_mins, rem_secs);

    gtk_label_set_text(GTK_LABEL(labels[gui_id_remainingtime]), remaining_time);

    //calc_get_hijri_date(prayer_times[0].time_at);     // not working currently. the website seems to be very slow.

    return true;
}

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
        GtkWidget* btn_next_city = data;
        gtk_widget_set_sensitive(btn_next_city, true);
        Callback_Seconds(NULL);
    }
}

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
        GtkWidget* btn_prev_city = data;
        gtk_widget_set_sensitive(btn_prev_city, true);
        Callback_Seconds(NULL);
    }
}

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

void on_dlg_calc_error_ok_clicked(GtkWidget* widget, gpointer data)
{
    if(cfg->num_cities > 1) {
        if(city_ptr == 1) {
            /*******
            Implement
            ********/
            puts("on_dlg_calc_error_ok_clicked: Next city");
        } else {
            /*******
            Implement
            ********/
            puts("on_dlg_calc_error_ok_clicked: Previous city");
        }
        GtkWidget* dialog_window = data;
        gtk_widget_destroy(GTK_WIDGET(dialog_window));
    } else {
        /*******
        Implement error reporting
        ********/
        exit(-1);
    }
}

void dlg_calc_error_retry_btn_clicked(GtkWidget* widget, gpointer data)
{
    City* city = &(cfg->cities[city_ptr]);
    if(city->pr_time_provider == prov_diyanet) {
        if(!socket_check_connection()) {
            puts("No internet connection");
        }
    }
    display_city(*city);
}

void on_dlg_about_response(GtkWidget* dlg_about, gpointer data) {
    gtk_widget_hide(dlg_about);
}

void on_menuitm_load_activate(GtkWidget* widget, gpointer data) {
    GtkFileChooser* dlg_filechooser = data;
    char* filename = 0;

    gtk_widget_show(GTK_WIDGET(dlg_filechooser));
    int dialog_ret = gtk_dialog_run(GTK_DIALOG(dlg_filechooser));
    if(dialog_ret == GTK_RESPONSE_OK) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg_filechooser));
        //config_save(cfg->cfg_filename, cfg);    // backup necessary?
        char old_filename[200];
        strcpy(old_filename, cfg->cfg_filename);
        size_t old_lang = cfg->lang;
        config_init(cfg);
        config_read(filename, cfg);
        cfg->config_changed = true;
        cfg->lang = old_lang;
        cfg->cfg_filename = realloc(cfg->cfg_filename, strlen(old_filename + 1) * sizeof(char));        // use initial filename, not last filename
        strcpy(cfg->cfg_filename, old_filename);
        config_save(old_filename, cfg);
        city_ptr = 0;
        display_city(cfg->cities[city_ptr]);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), true);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_city), false);
    } else if(dialog_ret == GTK_RESPONSE_CANCEL) {
        // do nothing
    }
    gtk_widget_hide(GTK_WIDGET(dlg_filechooser));
}

void on_menuitm_saveas_activate(GtkWidget* widget, gpointer data) {
    GtkFileChooser* dlg_filechooser = data;
    char* filename = 0;

    gtk_widget_show(GTK_WIDGET(dlg_filechooser));
    gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dlg_filechooser), GTK_FILE_CHOOSER_ACTION_SAVE);
    int dialog_ret = gtk_dialog_run(GTK_DIALOG(dlg_filechooser));
    if(dialog_ret == GTK_RESPONSE_OK) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg_filechooser));
        bool old_val = cfg->config_changed;
        cfg->config_changed = true;
        config_save(filename, cfg);
        cfg->config_changed = old_val;
        free(filename);
    } else if(dialog_ret == GTK_RESPONSE_CANCEL) {
        // do nothing
    }
    gtk_widget_hide(GTK_WIDGET(dlg_filechooser));
}

void on_menuitm_settings_activate(GtkWidget* widget, gpointer data) {
    GtkDialog* dlg_settings = data;

    gtk_widget_show(GTK_WIDGET(dlg_settings));
    int dialog_ret = 0;
RUN_DIALOG:
    dialog_ret = gtk_dialog_run(dlg_settings);
    if(dialog_ret == GTK_RESPONSE_APPLY) {
        // implement and goto run dialog
        goto RUN_DIALOG;
    } else if(dialog_ret == GTK_RESPONSE_OK) {
        // implement
    } else if(dialog_ret == GTK_RESPONSE_CANCEL) {
        // do nothing
    }
    gtk_widget_hide(GTK_WIDGET(dlg_settings));

}

void on_window_close(GtkWidget* window, gpointer data)
{
    int posX = 0, posY = 0;
    gtk_window_get_position(GTK_WINDOW(window), &posX, &posY);
    if(posX != cfg->last_window_posX || posY != cfg->last_window_posY || cfg->config_changed) {
        cfg->config_changed = true;
        cfg->last_window_posX = posX;
        cfg->last_window_posY = posY;
        config_save(cfg->cfg_filename, cfg);
    }

    gtk_widget_destroy(window);

    gtk_main_quit();
}

void build_assistant_glade()
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
void show_moodphase(unsigned char* data)
{
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_stream(data, NULL, NULL);
    if(pixbuf) {

    }
}
#endif // SHOW_MOON

void statusicon_clicked(GtkWidget* widget, gpointer data)
{
    gtk_window_present(GTK_WINDOW(widget));
}

void build_glade(Config* cfg_in, size_t num_strings, char* strings[num_strings])
{
    GtkBuilder      *builder;
    GtkWidget       *window_main;
    cfg = cfg_in;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "Prayer_times_GTK.glade", NULL);

    window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    /* Connect Button signals */
    /* Buttons city */
    btn_prev_city = GTK_BUTTON(gtk_builder_get_object(builder, "button_prev_city"));
    btn_next_city = GTK_BUTTON(gtk_builder_get_object(builder, "button_next_city"));
    g_signal_connect(btn_prev_city, "clicked", G_CALLBACK(on_btn_prev_city_clicked), btn_next_city);
    g_signal_connect(btn_next_city, "clicked", G_CALLBACK(on_btn_next_city_clicked), btn_prev_city);
    /* Buttons date */
    btn_prev_date = GTK_BUTTON(gtk_builder_get_object(builder, "button_prev_date"));
    btn_next_date = GTK_BUTTON(gtk_builder_get_object(builder, "button_next_date"));
    g_signal_connect(btn_prev_date, "clicked", G_CALLBACK(on_btn_prev_date_clicked), btn_next_date);
    g_signal_connect(btn_next_date, "clicked", G_CALLBACK(on_btn_next_date_clicked), btn_prev_date);

    /* Get Label IDs */
    if(strings) {
        for(size_t i = 0; i < gui_id_num; i++) {
            labels[i] = GTK_LABEL(gtk_builder_get_object(builder, gui_identifiers[i]));
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
#endif // SHOW_MOON

    gtk_widget_show(window_main);
    gtk_window_move(GTK_WINDOW(window_main), cfg->last_window_posX, cfg->last_window_posY);
    g_signal_connect(window_main, "delete-event", G_CALLBACK(on_window_close), NULL);

    g_timeout_add_seconds(1, G_SOURCE_FUNC(Callback_Seconds), NULL);
    Callback_Seconds(NULL);


    /* Dialog window for calculation error */
    dlg_calc_error = GTK_DIALOG(gtk_builder_get_object(builder, "dlg_calc_error"));
    gtk_builder_add_callback_symbol(builder, "on_dlg_calc_error_ok_clicked", G_CALLBACK(on_dlg_calc_error_ok_clicked));
    gtk_builder_add_callback_symbol(builder, "dlg_calc_error_retry_btn_clicked", G_CALLBACK(dlg_calc_error_retry_btn_clicked));
    gtk_builder_add_callback_symbol(builder, "on_dlg_about_response", G_CALLBACK(on_dlg_about_response));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_load_activate", G_CALLBACK(on_menuitm_load_activate));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_saveas_activate", G_CALLBACK(on_menuitm_saveas_activate));
    gtk_builder_add_callback_symbol(builder, "on_menuitm_settings_activate", G_CALLBACK(on_menuitm_settings_activate));

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
}
