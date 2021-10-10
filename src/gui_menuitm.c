/*
 * gui_menuitm.c
 *
 *  Created on: 10.10.2021
 *      Author: ahmet
 */

#include "gui_internal.h"


void on_menuitm_about_activate(GtkWidget* menuitm, gpointer data) {
	GtkWidget* dlg_about = data;

    char* current_version = update_get_current_version();
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dlg_about), current_version);
    free(current_version);

	gtk_widget_show(dlg_about);
	gtk_dialog_run(GTK_DIALOG(dlg_about)); // This Dialog can't do anything, ignore response
	gtk_widget_hide(dlg_about);
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

void on_menuitm_movecities_activate(GtkWidget* widget, gpointer data) {

}

void on_menuitm_removecity_activate(GtkWidget* widget, gpointer data) {
	GtkDialog* dlg_removecity = GTK_DIALOG(data);
	gtk_window_resize(GTK_WINDOW(dlg_removecity), -1, 200); // todo remove?
	char const*const listbox_name = "dlg_removecity_listbox";
	GtkListBox* listbox = GTK_LIST_BOX(find_child(GTK_WIDGET(dlg_removecity), listbox_name));
	if(!listbox) return;

ADD_CITIES:
	for(size_t i = 0; i < cfg->num_cities; i++) {
		gui_create_and_add_check_button(listbox, cfg->cities[i].name, NULL);
	}

	gtk_widget_show_all(GTK_WIDGET(dlg_removecity));
	int ret = 0;
RUN_DIALOG:
	ret = gtk_dialog_run(dlg_removecity);
	printf("ret: %d\n", ret);
	switch(ret) {
	case GTK_RESPONSE_APPLY:
	case GTK_RESPONSE_OK:
		(void) ret;

		GList* children = gtk_container_get_children(GTK_CONTAINER(listbox));
		size_t counter_deleted = 0;
		for(GList* child = children; child; child = child->next) {
			GtkListBoxRow* row = GTK_LIST_BOX_ROW(child->data);
			if(!row) return;
			GtkCheckButton* check_button = GTK_CHECK_BUTTON(gtk_bin_get_child(GTK_BIN(row)));
			if(!check_button) return;
			if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button))) {
				size_t i = gtk_list_box_row_get_index(row);
				config_remove_city(i - counter_deleted, cfg);
				if((i == city_ptr || i > cfg->num_cities) && cfg->num_cities) {
					city_ptr = 0;
				} else if(cfg->num_cities == 0) {
					ret = GTK_RESPONSE_OK;
					display_empty_city();
					gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), false);
					gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_city), false);
				}
				if(city_ptr && i < city_ptr) city_ptr--;
				counter_deleted++;
				i++;
			}
		}
		g_list_free(children);
		if(ret == GTK_RESPONSE_APPLY) {
			listbox = gtk_listbox_clear_dialog_removeCity(listbox);
			goto ADD_CITIES;
		}
		if(cfg->num_cities)
			display_city(cfg->cities[city_ptr]);
		break;

	default:
		puts("on_menuitm_removecity_activate, default-pfad");
		goto RUN_DIALOG;
		break;

	case GTK_RESPONSE_DELETE_EVENT:
	case GTK_RESPONSE_CANCEL:
		// do nothing, hide widget at end of function
		break;
	}
	if(cfg->num_cities)
		assert(city_ptr < cfg->num_cities);
	if(cfg->num_cities == 1 || city_ptr == 0) {
		gtk_widget_set_sensitive(GTK_WIDGET(btn_prev_city), false);
	}
	if(cfg->num_cities == 1 || city_ptr == cfg->num_cities - 1) {
		gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), false);
	} else if((cfg->num_cities > 1 && city_ptr == 0)) {
		gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), true);
	}
	gtk_listbox_clear_dialog_removeCity(listbox);
	gtk_widget_hide(GTK_WIDGET(dlg_removecity));
}

void on_menuitm_addcity_activate(GtkWidget* widget, gpointer data) {
	GtkAssistant* assistant_addcity = data;
	gtk_widget_show_all(GTK_WIDGET(assistant_addcity));
}
