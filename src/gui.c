#include <stdlib.h>
#include <stdbool.h>
//#define NDEBUG 1
#include <assert.h>
#include "gui.h"
#include "geolocation.h"
#include "cJSON.h"
#include "config.h"

//#define USE_STATUSICON

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

#ifdef USE_STATUSICON
void hide_statusicon(GtkStatusIcon* statusicon)
{
    gtk_status_icon_set_visible(statusicon, false);
}
#endif // USE_STATUSICON

void label_set_text(enum GUI_IDS id, char* text)
{
    gtk_label_set_text(GTK_LABEL(labels[id]), text);
}

static void gui_calc_prayer_times(City city, char dest[prayers_num][6] )
{
    prayer prayer_times[prayers_num];
    calc_function* calc = calc_functions[city.pr_time_provider];
    if(calc(city, prayer_times) != EXIT_SUCCESS) {
        assert(0);
    }

    for(size_t i = 0, pos = 0; i < prayers_num; i++) {
        if(i == pr_sunrise || i == pr_sunset) continue;
        sprintf(dest[pos], "%2d:%2d", prayer_times[i].time_at.tm_hour, prayer_times[i].time_at.tm_min);
        pos++;
    }

    char date[11];
    sprintf(date, "%2d.%2d.%4d", prayer_times[0].time_at.tm_mday, prayer_times[0].time_at.tm_mon + 1, prayer_times[0].time_at.tm_year + 1900);
    gtk_label_set_text(labels[gui_id_datename], date);
}

static void display_city(City city)
{
    char times[prayers_num][6];
    gui_calc_prayer_times(city, times);

    for(size_t i = gui_id_fajr_time; i <= gui_id_isha_time; i++) {
        gtk_label_set_text(labels[i], times[i - gui_id_fajr_time]);
    }
    gtk_label_set_text(labels[gui_id_cityname], city.name);
}

static int gui_calcpreview_prayer(City city, char dest[prayers_num][6], char dest_date[11], int days)
{
    prayer prayer_times[prayers_num];
    preview_function* preview = preview_functions[city.pr_time_provider];

    double const SECS_PER_DAY = 60*60*24;
    time_t date_t = time(0) + SECS_PER_DAY * days;
    date_t -= date_t % 86400;
    struct tm date = *gmtime(&date_t);

    int ret = preview(city, prayer_times, date);
    if(ret != EXIT_SUCCESS) {
        return ret;
    }

    for(size_t i = 0, pos = 0; i < prayers_num; i++) {
        if(i == pr_sunrise || i == pr_sunset) continue;
        sprintf(dest[pos], "%2d:%2d", prayer_times[i].time_at.tm_hour, prayer_times[i].time_at.tm_min);
        pos++;
    }
    sprintf(dest_date, "%2d.%2d.%4d", prayer_times[0].time_at.tm_mday, prayer_times[0].time_at.tm_mon + 1, prayer_times[0].time_at.tm_year + 1900);
    return ret;
}

static int display_preview(City city, int days)
{
    char times[prayers_num][6];
    char date[11];

    int ret = gui_calcpreview_prayer(city, times, date, days);
    if(ret != EXIT_SUCCESS) {
        return ret;
    }

    for(size_t i = gui_id_fajr_time; i <= gui_id_isha_time; i++) {
        gtk_label_set_text(labels[i], times[i - gui_id_fajr_time]);
    }
    gtk_label_set_text(labels[gui_id_datename], date);
    return ret;
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
    GtkButton* btn_prev_city = GTK_BUTTON(gtk_builder_get_object(builder, "button_prev_city"));
    GtkButton* btn_next_city = GTK_BUTTON(gtk_builder_get_object(builder, "button_next_city"));
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

    g_object_unref(builder);

    gtk_widget_show(window_main);
    g_signal_connect(window_main, "destroy", G_CALLBACK(gtk_main_quit),
                     NULL);

#ifdef USE_STATUSICON
    /* Status Icon */
    GtkStatusIcon   *statusicon;
    statusicon = gtk_status_icon_new_from_file("Graphics/Logo_small.png");
    assert(statusicon);
    g_signal_connect_swapped(window_main, "destroy", G_CALLBACK(hide_statusicon), statusicon);
#endif // USE_STATUSICON
}
