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


#include "gui_internal.h"

typedef enum AssistantAddcityPages {
	AssistantPages_Intro,
	AssistantPages_Calc_Cityname,
	AssistantPages_Calc_Method,
	AssistantPages_Diyanet,
} AssistantAddcityPages;

City city_buffer;


// Fucntion prototype declarations
static void assistant_set_current_page_complete(GtkAssistant* assistant);
static void assistant_set_page_incomplete(GtkAssistant* assistant, int page);
static void assistant_set_current_page_incomplete(GtkAssistant* assistant);
static void assistant_clearCityname_page(GtkAssistant* assistant);
static City assistant_read_city_name(GtkAssistant* assistant, City c);
static City* assistant_read_calc_params(City* c, GtkAssistant* assistant, GtkComboBox* method, GtkComboBox* asr, GtkComboBox* highlats);
static void assistant_apply_diyanet_to_combobox(GtkComboBoxText* combobox_dest, char* str);
static void assistant_diyanet_display_countries(GtkAssistant* assistant);
static void on_assistant_addcity_diyanet_combobox_changed_func(GtkComboBoxText* combobox_src, GtkComboBoxText* combobox_dest, bool set_dest_sensitive, char* (*f)(size_t, enum Languages));
static void assistant_addcity_diyanet_parse_and_add_city(char const*const combobox_city_str);

void on_dlg_add_city_close(GtkWidget* widget, gpointer data) {
    GtkSearchEntry* entry = GTK_SEARCH_ENTRY(find_child(widget, "assistant_add_city_page1_search"));
    if(entry) {
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
    GtkListBox* list = GTK_LIST_BOX(find_child(widget, "assistant_add_city_page1_listbox"));
    if(list) {
       list =  gtk_listbox_clear_assistant_addcity(list);
    }
    gtk_widget_hide(widget);
}

/** \brief Set current page of GtkAssistant complete, so that the next page button can be activated
 *
 */
static void assistant_set_current_page_complete(GtkAssistant* assistant) {
	if(assistant) {
		int current_page = gtk_assistant_get_current_page(assistant);

		GtkWidget* assistant_current_page = gtk_assistant_get_nth_page(assistant, current_page);
		gtk_assistant_set_page_complete(assistant, assistant_current_page, true);
	}
}

/** \brief Set page of GtkAssistant incomplete, so that the next page button can not be activated
 *
 */
static void assistant_set_page_incomplete(GtkAssistant* assistant, int page) {
	if(assistant) {
		GtkWidget* assistant_page = gtk_assistant_get_nth_page(assistant, page);
		gtk_assistant_set_page_complete(assistant, assistant_page, false);
	}
}

/** \brief Set current page of GtkAssistant incomplete, so that the next page button can not be activated
 *
 */
static void assistant_set_current_page_incomplete(GtkAssistant* assistant) {
	if(assistant) {
		int current_page = gtk_assistant_get_current_page(assistant);
		assistant_set_page_incomplete(assistant, current_page);
	}
}

/** \brief Clear "City name" Page of @p assistant
 *
 */
static void assistant_clearCityname_page(GtkAssistant* assistant) {
	if(assistant) {
		assistant_set_page_incomplete(assistant, AssistantPages_Calc_Cityname);
		GtkListBox* listbox = GTK_LIST_BOX(find_child(GTK_WIDGET(assistant), "assistant_add_city_page1_listbox")); // Todo statt listbox die grid �bertragen; dann kann ich die listbox l�schen. Denn wenn ich die listbox l�sche, kann der gpointer nicht mehr ordnungsgem�� arbeitne.
		GtkSearchEntry* search_entry = GTK_SEARCH_ENTRY(find_child(GTK_WIDGET(assistant), "assistant_add_city_page1_search"));
		if(listbox)
			gtk_listbox_clear_assistant_addcity(listbox);
		if(search_entry)
			gtk_entry_set_text(GTK_ENTRY(search_entry), "");
	}
}

/** \brief Read name of selected City in @p assistant, Apply settings of City @p c
 * and return a City containing all information
 *
 */
static City assistant_read_city_name(GtkAssistant* assistant, City c) {
	City ret = c;
	if(assistant) {
		GtkRadioButton* radio = GTK_RADIO_BUTTON(find_child(GTK_WIDGET(assistant), "radio_button_first"));
		GtkRadioButton* active = NULL;
		if(radio) {
			GSList* list = gtk_radio_button_get_group(radio);
			if(list) {
				for(size_t i = 0, len = g_slist_length(list); i < len; i++) {
					active = GTK_RADIO_BUTTON(g_slist_nth_data(list, i));
					if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(active)))
						break;
				}
				char const*const string = gtk_button_get_label(GTK_BUTTON(active));
				char buffer[strlen(string) + 5];
				strcpy(buffer, string);
				if(string) {
					char* name = strtok(buffer, ",");
					strtok(0, ":");
					char* lattitude_str = strtok(0, ";");
					char* longitude_str = strtok(0, "\n");

					double lattitude = strtod(lattitude_str, 0);
					double longitude = strtod(longitude_str, 0);
					ret = *city_set_name(&ret, name);
					ret.latitude = lattitude;
					ret.longitude = longitude;
				}
			}
		}
	}
	return ret;
}

/** \brief Read selected calculation methods in @p assistant into City @p c and return
 * City containing all informations
 *
 * \param c City with settings
 * \param method GtkComboBox containing calculation method information
 * \param asr GtkComboBox containing calculation information for asr method
 * \param highlats GtkBomboBox containing calculation information for high lattitudes
 * \return City containing all informations
 */
static City* assistant_read_calc_params(City* c, GtkAssistant* assistant, GtkComboBox* method, GtkComboBox* asr, GtkComboBox* highlats) {
	City* ret = 0;
	if(c && assistant && method && asr && highlats) {
		char const*const method_id = gtk_combo_box_get_active_id(method);
		if(method_id) {
			for(enum ST_calculation_method cm = ST_cm_Karachi; cm < ST_cm_num; cm++) {
				if(!strcmp(method_id, ST_cm_names[cm])) {
					c->method = cm;
					break;
				}
			}
		}
		char const*const asr_id = gtk_combo_box_get_active_id(asr);
		if(asr_id) {
			if(!strcmp(asr_id, "shafi")) {
				c->asr_juristic = ST_jm_Shafii;
			} else if(!strcmp(asr_id, "hanafi")) {
				c->asr_juristic = ST_jm_Hanafi;
			}
		}
		char const*const adjust_id = gtk_combo_box_get_active_id(highlats);
		if(adjust_id) {
			if(!strcmp(adjust_id, "none")) {
				c->adjust_high_lats = ST_am_None;
			} else if(!strcmp(adjust_id, "midnight")) {
				c->adjust_high_lats = ST_am_MidNight;
			} else if(!strcmp(adjust_id, "one_seventh")) {
				c->adjust_high_lats = ST_am_OneSeventh;
			} else if(!strcmp(adjust_id, "angle")) {
				c->adjust_high_lats = ST_am_AngleBased;
			}
		}
		ret = c;
	}
	return ret;
}

/** \brief Parse chosen @p str GtkComboBox and return Code of chosen element
 */
static size_t assistant_get_diyanet_code(char const*const str) {
	size_t ret = 0;
	if(str) {
		char buffer[strlen(str) + 5];
		strcpy(buffer, str);

		strtok(buffer, ",");
		char* id = strtok(0, "\0");
		ret = strtol(id, 0, 10);
	}
	return ret;
}

/** \brief Parse @p str of diyanet and add elements to @p combobox_dest
 */
static void assistant_apply_diyanet_to_combobox(GtkComboBoxText* combobox_dest, char* str) {
	if(combobox_dest && str) {
		gtk_combo_box_text_remove_all(combobox_dest);
		char* name = 0;
		name = strtok(str, ";");
		gtk_combo_box_text_append_text(combobox_dest, name);
		while((name = strtok(0, ";"))) {
			gtk_combo_box_text_append_text(combobox_dest, name);
		}
		gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_dest), -1);
	}
}

/** \brief (Diyanet) Get all countries and att to GtkComboBox
 */
static void assistant_diyanet_display_countries(GtkAssistant* assistant) {
	if(assistant) {
		char* countries = diyanet_get_country_codes(cfg->lang);
		if(countries) {
			GtkComboBoxText* combobox_country = GTK_COMBO_BOX_TEXT(find_child(GTK_WIDGET(assistant), "assistant_addcity_diyanet_combobox_country"));
			if(combobox_country) {
				assistant_apply_diyanet_to_combobox(combobox_country, countries);
			}
		}
		free(countries);
	}
}

/** \brief Reset Combobox of Diyanet page
 */
static void assistant_diyanet_reset_combobox(GtkComboBoxText* combobox, bool set_sensitive) {
	if(combobox) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), -1);
		if(gtk_combo_box_get_has_entry(GTK_COMBO_BOX(combobox)))
			gtk_combo_box_text_remove_all(combobox);
		gtk_widget_set_sensitive(GTK_WIDGET(combobox), set_sensitive);
	}
}

/** \brief Reset all comboboxes of diyanet page
 */
static void assistant_diyanet_reset_comboboxes(GtkComboBoxText* combobox_country, GtkComboBoxText* combobox_provinces, GtkComboBoxText* combobox_cities) {
	if(combobox_country)
		assistant_diyanet_reset_combobox(combobox_country, true);
	if(combobox_provinces)
		assistant_diyanet_reset_combobox(combobox_provinces, false);
	if(combobox_cities)
		assistant_diyanet_reset_combobox(combobox_cities, false);
}

/** \brief (Diyanet) Parse chosen element and update next combobox.
 *  This function is called when the chosen element of a combobox chanes.
 *
 *  \param combobox_src Combobox which was changed by user
 *  \param combobox_dest Next combobox to be filled
 *  \param set_dest_sensitive whether desination is to be set sensitive
 *  \param f function pointer returning codes for @p combobox_next
 */
static void on_assistant_addcity_diyanet_combobox_changed_func(GtkComboBoxText* combobox_src, GtkComboBoxText* combobox_dest, bool set_dest_sensitive, char* (*f)(size_t, enum Languages)) {
	if(combobox_src && combobox_dest && f) {
		char const*const chosen_element = gtk_combo_box_text_get_active_text(combobox_src);
		if(chosen_element) {
			size_t chosen_id = assistant_get_diyanet_code(chosen_element);
			char* result_str = f(chosen_id, cfg->lang);
			assistant_apply_diyanet_to_combobox(combobox_dest, result_str);
			if(set_dest_sensitive)
				gtk_widget_set_sensitive(GTK_WIDGET(combobox_dest), true);
			free(result_str);
		} else {
			// Reset
			if(gtk_combo_box_get_has_entry(GTK_COMBO_BOX(combobox_src)))
				gtk_combo_box_text_remove_all(combobox_src);
			assistant_diyanet_reset_combobox(combobox_dest, false);
		}
	}
}

/** \brief (Diyanet) Parse chosen Country and update provinces.
 *
 */
void on_assistant_addcity_diyanet_combobox_country_changed(GtkWidget* widget, gpointer data) {
	GtkComboBoxText* combobox_country = GTK_COMBO_BOX_TEXT(widget);
	GtkComboBoxText* combobox_provinces = GTK_COMBO_BOX_TEXT(data);
	if(combobox_country && combobox_provinces) {
		on_assistant_addcity_diyanet_combobox_changed_func(combobox_country, combobox_provinces, true, diyanet_get_provinces);
	}
}

/** \brief (Diyanet) Parse chosen Province and update Cities.
 *
 */
void on_assistant_addcity_diyanet_combobox_province_changed(GtkWidget* widget, gpointer data) {
	GtkComboBoxText* combobox_provinces = GTK_COMBO_BOX_TEXT(widget);
	GtkComboBoxText* combobox_cities = GTK_COMBO_BOX_TEXT(data);
	if(combobox_provinces && combobox_cities) {
		on_assistant_addcity_diyanet_combobox_changed_func(combobox_provinces, combobox_cities, true, diyanet_get_cities);
	}
}

/** \brief (Diyanet) Parse chosen City and set page complete or incomplete.
 *
 */
void on_assistant_addcity_diyanet_combobox_city_changed(GtkWidget* widget, gpointer data) {
	GtkComboBoxText* combobox_city = GTK_COMBO_BOX_TEXT(widget);
	GtkAssistant* assistant = GTK_ASSISTANT(data);
	if(combobox_city && assistant) {
		char const*const chosen_id = gtk_combo_box_text_get_active_text(combobox_city);
		if(chosen_id && strcmp(chosen_id, ""))
			assistant_set_current_page_complete(assistant);
		else
			assistant_set_current_page_incomplete(assistant);
	}
}

/** \brief (Diyanet) Parse chosen City, add to config and show it, if it is the only city in config
 *
 */
static void assistant_addcity_diyanet_parse_and_add_city(char const*const combobox_city_str) {
	if(combobox_city_str) {
		char buffer[strlen(combobox_city_str) + 5];
		strcpy(buffer, combobox_city_str);
		char* name = strtok(buffer, ",");
		char* id_str = strtok(0, "\0");
		char filename[strlen(id_str) + 30];
		strcpy(filename, diyanet_prayer_times_file_destination);
		strcat(filename, id_str);
		strcat(filename, ".json");

		city_init_diyanet(&city_buffer, name, prov_diyanet, filename, 0);
		config_add_city(city_buffer, cfg);
		if(cfg->num_cities == 1) {
			city_ptr = 0;
			display_city(cfg->cities[0]);
		}
	}
}

/** \brief State machien for Addcity-Assistant determining next page
 *
 */
int assistant_addcity_nextpage_func(int current_page, gpointer data) {
	AssistantAddcityPages next_page = 0;
	AssistantAddcityPages curr_page = current_page;
	GtkAssistant* assistant = GTK_ASSISTANT(data);

	switch(curr_page) {
	case AssistantPages_Intro:
		city_buffer = *city_init_empty(&city_buffer);
		GtkRadioButton* button_calc = GTK_RADIO_BUTTON(find_child(GTK_WIDGET(assistant), "assistant_addcity_radiobutton_calc"));
		GtkRadioButton* button_diyanet = GTK_RADIO_BUTTON(find_child(GTK_WIDGET(assistant), "assistant_addcity_radiobutton_diyanet"));
		assert(button_calc && button_diyanet);
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_calc))) {
			next_page = AssistantPages_Calc_Cityname;
			city_buffer.pr_time_provider = prov_calc;
		} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_diyanet))) {
			next_page = AssistantPages_Diyanet;
			city_buffer.pr_time_provider = prov_diyanet;
		}

		break;
	case AssistantPages_Calc_Cityname:
		next_page = AssistantPages_Calc_Method;
		break;

	case AssistantPages_Calc_Method:
		(void)next_page;
		GtkComboBox* method = 0, *asr = 0, *highlats = 0;
		method = GTK_COMBO_BOX(find_child(GTK_WIDGET(assistant), "assistant_addcity_calc_combobox_method"));
		asr = GTK_COMBO_BOX(find_child(GTK_WIDGET(assistant), "assistant_addcity_calc_combobox_asr"));
		highlats = GTK_COMBO_BOX(find_child(GTK_WIDGET(assistant), "assistant_addcity_calc_combobox_highlats"));
		if(method && asr && highlats) {
			city_buffer = *assistant_read_calc_params(&city_buffer, assistant, method, asr, highlats);
			if(config_add_city(city_buffer, cfg) == EXIT_FAILURE) {
				assert(false);
			}
			if(cfg->num_cities == 1) {
				city_ptr = 0;
				display_city(cfg->cities[city_ptr]);
			} else
				gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), true);
			gtk_widget_hide(GTK_WIDGET(assistant));
			//next_page = AssistantPages_Apply;
		}
		break;
	case AssistantPages_Diyanet:
		(void) next_page;
		GtkComboBoxText* combobox_diyanet_city = GTK_COMBO_BOX_TEXT(find_child(GTK_WIDGET(assistant), "assistant_addcity_diyanet_combobox_city"));
		if(combobox_diyanet_city) {
			char const*const chosen_element = gtk_combo_box_text_get_active_text(combobox_diyanet_city);
			if(chosen_element) {
				assistant_addcity_diyanet_parse_and_add_city(chosen_element);
				gtk_widget_set_sensitive(GTK_WIDGET(btn_next_city), true);
				gtk_widget_hide(GTK_WIDGET(assistant));
			} else {
				next_page = current_page;
			}
		}
		break;
	default:
		perror("assistant_addcity_nextpage_func, default. Should not occur");
		break;

	}
	return next_page;
}

/** \brief Prepare next page of addcity assistant before it is shown
 *
 */
void on_assistant_addcity_prepare(GtkWidget* widget, gpointer data) {
	GtkAssistant* assistant = GTK_ASSISTANT(widget);

	int current_page = gtk_assistant_get_current_page(assistant);
	switch(current_page) {
	case AssistantPages_Intro:
		(void) current_page;
		assistant_set_current_page_complete(assistant);
		assistant_set_page_incomplete(assistant, AssistantPages_Calc_Cityname);
		assistant_clearCityname_page(assistant);
		gtk_window_resize(GTK_WINDOW(assistant), 500, 130);
		break;
	case AssistantPages_Calc_Cityname:
		break;

	case AssistantPages_Calc_Method:
		city_buffer = assistant_read_city_name(assistant, city_buffer);
		assistant_clearCityname_page(assistant);
		assistant_set_page_incomplete(assistant, AssistantPages_Calc_Cityname);
		assistant_set_current_page_complete(assistant);
		gtk_window_resize(GTK_WINDOW(assistant), 500, 130);
		break;

	case AssistantPages_Diyanet:
		(void) current_page;
		GtkComboBoxText *combobox_country, *combobox_provinces, *combobox_cities;
		combobox_country = GTK_COMBO_BOX_TEXT(find_child(GTK_WIDGET(assistant), "assistant_addcity_diyanet_combobox_country"));
		combobox_provinces = GTK_COMBO_BOX_TEXT(find_child(GTK_WIDGET(assistant), "assistant_addcity_diyanet_combobox_province"));
		combobox_cities = GTK_COMBO_BOX_TEXT(find_child(GTK_WIDGET(assistant), "assistant_addcity_diyanet_combobox_city"));
		if(combobox_country && combobox_provinces && combobox_cities) {
			assistant_diyanet_reset_comboboxes(combobox_country, combobox_provinces, combobox_cities);
		}
		assistant_diyanet_display_countries(assistant);
		break;

	default:
		assistant_clearCityname_page(assistant);
		gtk_window_resize(GTK_WINDOW(assistant), 500, 130);
		break;

	}
}

/** \brief Cancel button of assistant add city is pressed
 *
 */
void on_assistant_addcity_cancel(GtkWidget* widget, gpointer data) {
	GtkAssistant* assistant = GTK_ASSISTANT(widget);
	gtk_widget_hide(GTK_WIDGET(assistant));
}

/** \brief Search bar of add city assistant is changed (calc)
 *
 */
void on_assistant_add_city_page1_search_search_changed(GtkWidget* widget, gpointer data) {
	GtkAssistant* assistant = GTK_ASSISTANT(data);
    GtkGrid* grid = GTK_GRID(find_child(GTK_WIDGET(assistant), "assistant_addcity_page1_grid"));
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
        GtkRadioButton* first_radio_button = GTK_RADIO_BUTTON(find_child(GTK_WIDGET(grid), "radio_button_first"));
        if(first_radio_button) { // Destroy remaining radio buttons
            assert(listbox);
            GtkListBox* newList = gtk_listbox_clear_assistant_addcity(listbox);
            if(newList) listbox = newList;
        }

        if(!(first_radio_button = gui_create_and_add_radio_button(listbox, NULL, split, "radio_button_first"))) // create new radio buttons
                goto ERR;
        while((split = strtok(0, "\n"))) {
            GtkRadioButton* radio_button = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label_from_widget(first_radio_button, split));
            gtk_container_add(GTK_CONTAINER(listbox), GTK_WIDGET(radio_button));
        }
        assistant_set_current_page_complete(assistant);
        gtk_widget_show_all(GTK_WIDGET(listbox));
        free(geolocation_string);
    }
    return;

ERR:
	myperror("Error on_assistant_add_city_page1_search_search_changed");
    free(geolocation_string);
    return;
}
