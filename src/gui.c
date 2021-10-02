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

static Config* cfg;
static size_t city_ptr = 0;
static int day_ptr = 0;
GtkLabel* labels[gui_id_num];
GtkButton* btn_prev_date = 0;
GtkButton* btn_next_date = 0;
GtkButton* btn_prev_city = 0;
GtkButton* btn_next_city = 0;
GtkDialog* dlg_calc_error = 0;
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
        sprint_prayer_time(prayer_times[i], dest_len, dest[pos]);
        pos++;
    }
    if(city.pr_time_provider == prov_calc) {
        prayer_times[0].hicri_date = calc_get_hijri_date(prayer_times[0].time_at);
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
    gtk_label_set_text(labels[gui_id_hijridate], hijri_date);
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
    gtk_label_set_text(labels[gui_id_hijridate], hijri_date);
    return ret;
}

static GtkWidget* find_child(GtkWidget* parent, const char*const name)
{
    if(!parent || !name) return NULL;
    if (!strcmp(gtk_widget_get_name(parent), name)) {
        return parent;
    }

    if (GTK_IS_BIN(parent)) {
        GtkWidget* child = gtk_bin_get_child(GTK_BIN(parent));
        return find_child(child, name);
    }

    if (GTK_IS_CONTAINER(parent)) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
        if(!children) return NULL;
        do {
            GtkWidget* widget = find_child(children->data, name);
            if (widget) {
                return widget;
            }
        } while((children = g_list_next(children)));
        //g_list_free_full(children);
        g_list_free(children);
    }
    return NULL;
}

static GtkListBox* gtk_listbox_clear(GtkListBox* listbox) {
    GtkListBox* box_return = listbox;
    if(!listbox) return box_return;

    GtkWidget* parent = gtk_widget_get_parent(GTK_WIDGET(listbox));
    if(!parent || !GTK_IS_CONTAINER(parent)) return box_return;

    GtkListBox* new_listbox = GTK_LIST_BOX(gtk_list_box_new());
    if(!new_listbox) return box_return;
    gtk_widget_set_name(GTK_WIDGET(new_listbox), "assistant_add_city_page1_listbox");

    gtk_widget_destroy(GTK_WIDGET(listbox));
    assert(GTK_IS_GRID(parent));
    GtkGrid* grid = GTK_GRID(parent);
    gtk_grid_attach(grid, GTK_WIDGET(new_listbox), 1, 1, 1, 1);
    //gtk_container_add(GTK_CONTAINER(parent), GTK_WIDGET(new_listbox));
    box_return = new_listbox;
    return box_return;
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

bool Callback_Seconds(gpointer data)
{
    prayer prayer_times[prayers_num];
    gui_calc_prayer(cfg->cities[city_ptr], prayer_times);
    if(cfg->cities[city_ptr].pr_time_provider == prov_empty) {
        gtk_label_set_text(GTK_LABEL(labels[gui_id_remainingtime]), "");
        return true;           // In case the calculation method failed, remaining time will also not be calculated
    }

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
            gtk_widget_show(GTK_WIDGET(dlg_calc_error));
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
        gtk_widget_hide(GTK_WIDGET(dialog_window));
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

void on_dlg_about_response(GtkWidget* dlg_about, gpointer data)
{
    gtk_widget_hide(dlg_about);
}

bool on_dlg_about_delete_event(GtkWidget* dlg_about, gpointer data) {
	puts("on_dlg_about_delete_event");
	gtk_widget_hide(dlg_about);
	return true;
}

void on_menuitm_load_activate(GtkWidget* widget, gpointer data)
{
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
#ifdef CHECK_RET
        char* new_mem = realloc(cfg->cfg_filename, strlen(old_filename + 1) * sizeof(char));        // use initial filename, not last filename
        cfg->cfg_filename = new_mem ? new_mem : cfg->cfg_filename;
#else
        cfg->cfg_filename = realloc(cfg->cfg_filename, strlen(old_filename + 1) * sizeof(char));        // use initial filename, not last filename
#endif
        strcpy(cfg->cfg_filename, old_filename);
        config_json_save(old_filename, cfg);
        city_ptr = 0;
        display_city(cfg->cities[city_ptr]);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), true);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_city), false);
    } else if(dialog_ret == GTK_RESPONSE_CANCEL) {
        // do nothing
    }
    gtk_widget_hide(GTK_WIDGET(dlg_filechooser));
}

void on_menuitm_movecities_activate(GtkWidget* widget, gpointer data) {

}

void on_menuitm_removecity_activate(GtkWidget* widget, gpointer data) {

}

void on_menuitm_addcity_activate(GtkWidget* widget, gpointer data) {
#define ASSISTANT_ADDCITY
#ifdef ASSISTANT_ADDCITY
	GtkAssistant* assistant_addcity = data;
	gtk_widget_show_all(GTK_WIDGET(assistant_addcity));
#else
	GtkDialog* dlg_addcity = data;

    GtkListBox* listbox = GTK_LIST_BOX(find_child(GTK_WIDGET(dlg_addcity), "assistant_add_city_page1_listbox"));
    if(!listbox) return;

    gtk_widget_show_all(GTK_WIDGET(dlg_addcity));
    int dialog_ret = gtk_dialog_run(dlg_addcity);
    if(dialog_ret == GTK_RESPONSE_APPLY) {
        gtk_widget_hide(GTK_WIDGET(dlg_addcity));
    } else if(dialog_ret == GTK_RESPONSE_CANCEL) {
        gtk_widget_hide(GTK_WIDGET(dlg_addcity));
    } else {
        gtk_widget_hide(GTK_WIDGET(dlg_addcity));
    }

    // Clear Search
    GtkSearchEntry* entry = GTK_SEARCH_ENTRY(find_child(GTK_WIDGET(dlg_addcity), "assistant_add_city_page1_search"));
    if(entry) {
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
    GtkListBox* list = GTK_LIST_BOX(find_child(GTK_WIDGET(dlg_addcity), "assistant_add_city_page1_listbox"));
    if(list) {
       list =  gtk_listbox_clear(list);
    }
    gtk_window_resize(GTK_WINDOW(dlg_addcity), 350, 200);
#endif
}

static GtkRadioButton* gui_create_and_add_radio_button(GtkListBox* listbox, GtkRadioButton* group, char const*const label, char const*const name) {
    GtkRadioButton* ret = 0;
    if(!listbox || !label) return ret;

    GtkListBoxRow* list_element = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
    if(!list_element) goto ERR;

    ret = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label_from_widget(group, label));
    //ret = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label(list, label));
    if(!ret) goto ERR;
    if(name) gtk_widget_set_name(GTK_WIDGET(ret), name);

    gtk_container_add(GTK_CONTAINER(list_element), GTK_WIDGET(ret));
    gtk_list_box_insert(listbox, GTK_WIDGET(list_element), -1);

    return ret;
ERR:
	myperror("Error creating radio button");
    return ret;
}

void on_assistant_addcity_prepare(GtkWidget* widget, gpointer data) {
	GtkAssistant* assistant = GTK_ASSISTANT(widget);

	int current_page = gtk_assistant_get_current_page(assistant);
	if(current_page == 0) {
		GtkWidget* page0 = gtk_assistant_get_nth_page(assistant, current_page);
		gtk_assistant_set_page_complete(assistant, page0, true);
	}
	puts("drin");
}

void on_assistant_addcity_cancel(GtkWidget* widget, gpointer data) {
	GtkAssistant* assistant = GTK_ASSISTANT(widget);
	puts("on_assistant_addcity_cancel");
	gtk_widget_hide(GTK_WIDGET(assistant));
}

void on_assistant_add_city_page1_search_search_changed(GtkWidget* widget, gpointer data) {
    GtkGrid* grid = data;
    GtkListBox* listbox = GTK_LIST_BOX(find_child(GTK_WIDGET(grid), "assistant_add_city_page1_listbox")); // Todo statt listbox die grid �bertragen; dann kann ich die listbox l�schen. Denn wenn ich die listbox l�sche, kann der gpointer nicht mehr ordnungsgem�� arbeitne.
    if(!listbox)
        return;
    char* geolocation_string = 0;

    char const*const input_string = gtk_entry_get_text(GTK_ENTRY(widget));
    if(!input_string) goto ERR;

    if(strlen(input_string) > 3) {
        geolocation_string = geolocation_get(input_string);
        if(!geolocation_string) goto ERR;
        char* split = strtok(geolocation_string, "\n");
        GtkRadioButton* first_radio_button = GTK_RADIO_BUTTON(find_child(GTK_WIDGET(data), "radio_button_first"));
        if(first_radio_button) { // Destroy remaining radio buttons
            assert(listbox);
            GtkListBox* newList = gtk_listbox_clear(listbox);
            if(newList) listbox = newList;
            //gtk_listbox_remove_all_children(listbox);
            //destroy_all_children(GTK_WIDGET(listbox));
            //gtk_widget_show_all(GTK_WIDGET(listbox));
        }


        if(!(first_radio_button = gui_create_and_add_radio_button(listbox, NULL, split, "radio_button_first"))) // create new radio buttons
                goto ERR;
        while((split = strtok(0, "\n"))) {
            GtkRadioButton* radio_button = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label_from_widget(first_radio_button, split));
            gtk_container_add(GTK_CONTAINER(listbox), GTK_WIDGET(radio_button));
        }
        gtk_widget_show_all(GTK_WIDGET(listbox));
        free(geolocation_string);
    }
    return;

ERR:
	myperror("Error on_assistant_add_city_page1_search_search_changed");
    free(geolocation_string);
    return;
}

void on_dlg_add_city_close(GtkWidget* widget, gpointer data) {
    GtkSearchEntry* entry = GTK_SEARCH_ENTRY(find_child(widget, "assistant_add_city_page1_search"));
    if(entry) {
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
    GtkListBox* list = GTK_LIST_BOX(find_child(widget, "assistant_add_city_page1_listbox"));
    if(list) {
       list =  gtk_listbox_clear(list);
    }
    gtk_widget_hide(widget);
}

void on_menuitm_saveas_activate(GtkWidget* widget, gpointer data)
{
    GtkFileChooser* dlg_filechooser = data;
    char* filename = 0;

    gtk_widget_show(GTK_WIDGET(dlg_filechooser));
    gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dlg_filechooser), GTK_FILE_CHOOSER_ACTION_SAVE);
    int dialog_ret = gtk_dialog_run(GTK_DIALOG(dlg_filechooser));
    if(dialog_ret == GTK_RESPONSE_OK) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg_filechooser));
        char* filename_utf8 = g_filename_to_utf8(filename, -1, NULL, NULL, NULL);
        free(filename);
        filename = filename_utf8;

        if(!strstr(filename, ".cfg")) {
            filename = realloc(filename, strlen(filename) + strlen(".cfg") + 1);
            strcat(filename, ".cfg");
        }
        bool old_val = cfg->config_changed;
        cfg->config_changed = true;
        config_json_save(filename, cfg);
        cfg->config_changed = old_val;
        free(filename);
    } else if(dialog_ret == GTK_RESPONSE_CANCEL) {
        // do nothing
    }
    gtk_widget_hide(GTK_WIDGET(dlg_filechooser));
}

static bool dlg_settings_apply_config(GtkDialog* dlg_settings) {
    char buffer[200];
    char* widget_name = "combobox_lang";
    GtkWidget* pwidget = find_child(GTK_WIDGET(dlg_settings), widget_name);
    if(!pwidget) goto ERR;
    gtk_combo_box_set_active(GTK_COMBO_BOX(pwidget), cfg->lang);

    widget_name = "checkbutton_saveposition";
    pwidget = find_child(GTK_WIDGET(dlg_settings), widget_name);
    if(!pwidget) goto ERR;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pwidget), cfg->save_position);

    widget_name = "checkbutton_checkforupdates";
    pwidget = find_child(GTK_WIDGET(dlg_settings), widget_name);
    if(!pwidget) goto ERR;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pwidget), cfg->check_for_updates);

    widget_name = "checkbutton_enable_notification";
    pwidget = find_child(GTK_WIDGET(dlg_settings), widget_name);
    if(!pwidget) goto ERR;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pwidget), cfg->enable_notification);

    return true;

ERR:
    sprintf(buffer, "Error in function %s looking for widget %s", __func__, widget_name);
	myperror(buffer);
    return false;
}

void on_menuitm_settings_activate(GtkWidget* widget, gpointer data)
{
    GtkDialog* dlg_settings = data;
    int dialog_ret = 0;
    bool end = false;
    GtkWidget* pwidget;

    if(!dlg_settings_apply_config(dlg_settings)) return;
    gtk_widget_show(GTK_WIDGET(dlg_settings));

RUN_DIALOG:
    dialog_ret = gtk_dialog_run(dlg_settings);
    const char* lang = 0;
    switch(dialog_ret) {
    case GTK_RESPONSE_OK:
        end = true;
    case GTK_RESPONSE_APPLY:
        pwidget = find_child(GTK_WIDGET(dlg_settings), "combobox_lang");
        if(!pwidget) goto ERR_WIDGET;
        lang = gtk_combo_box_get_active_id(GTK_COMBO_BOX(pwidget));
        if(!strcmp(lang, "de")) {
            cfg->lang = LANG_DE;
        } else if(!strcmp(lang, "en")) {
            cfg->lang = LANG_EN;
        } else if(!strcmp(lang, "tr")) {
            cfg->lang = LANG_TR;
        }

        pwidget = find_child(GTK_WIDGET(dlg_settings), "checkbutton_saveposition");
        if(!pwidget) goto ERR_WIDGET;
        cfg->save_position = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pwidget));

        pwidget = find_child(GTK_WIDGET(dlg_settings), "checkbutton_checkforupdates");
        if(!pwidget) goto ERR_WIDGET;
        cfg->check_for_updates = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pwidget));

        pwidget = find_child(GTK_WIDGET(dlg_settings), "checkbutton_enable_notification");
        if(!pwidget) goto ERR_WIDGET;
        cfg->enable_notification = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pwidget));
        cfg->config_changed = true;

        if(!end) goto RUN_DIALOG;
        break;
    case GTK_RESPONSE_CANCEL:
        dlg_settings_apply_config(dlg_settings);
        break;
    }
    gtk_widget_hide(GTK_WIDGET(dlg_settings));
    return;

ERR_WIDGET:
	myperror("Error finding widget!");
    gtk_widget_hide(GTK_WIDGET(dlg_settings));
}

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

    /* Callback functions */
    gtk_builder_add_callback_symbol(builder, "on_dlg_calc_error_ok_clicked", G_CALLBACK(on_dlg_calc_error_ok_clicked));
    gtk_builder_add_callback_symbol(builder, "dlg_calc_error_retry_btn_clicked", G_CALLBACK(dlg_calc_error_retry_btn_clicked));
    gtk_builder_add_callback_symbol(builder, "on_dlg_about_response", G_CALLBACK(on_dlg_about_response));
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
    gtk_builder_add_callback_symbol(builder, "on_dlg_about_delete_event", G_CALLBACK(on_dlg_about_delete_event));


    /* About dialog */
    GtkAboutDialog* dlg_about = GTK_ABOUT_DIALOG(gtk_builder_get_object(builder, "dlg_about"));
    CHECK_OBJ(dlg_about);
    char* current_version = update_get_current_version();
    gtk_about_dialog_set_version(dlg_about, current_version);
    free(current_version);

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
