/*
 * gui_general.c
 *
 *  Created on: 10.10.2021
 *      Author: ahmet
 */


#include "gui_internal.h"

void label_append_text(GtkLabel* label, char const*const text) {
	if(label && text) {
		char const*const current_content = gtk_label_get_text(label);
		assert(current_content);

		if(!strstr(current_content, text)) { // Don't add multiple times
			size_t buffer_len = strlen(current_content) + strlen(text) + 1;
			char buffer[buffer_len];
			strcpy(buffer, current_content);
			strcat(buffer, text);

			gtk_label_set_text(label, buffer);
		}
	}
}

GtkWidget* find_child(GtkWidget* parent, const char*const name)
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

void sprint_dates(prayer times, size_t buff_len, char dest_julian_date[buff_len], char dest_hijri_date[buff_len])
{
    sprint_prayer_date(times, buff_len, dest_julian_date, false);
    sprint_prayer_date(times, buff_len, dest_hijri_date, true);
}
