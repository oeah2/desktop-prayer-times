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

/** \brief Apply settings from config to @p dlg_settings
 *
 */
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
	myperror(__FILE__, __LINE__, buffer);
    return false;
}

/** \brief Apply settings from Settings-Dialog to Config
 *
 */
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
	myperror(__FILE__, __LINE__, "Error finding widget!");
    gtk_widget_hide(GTK_WIDGET(dlg_settings));
}
