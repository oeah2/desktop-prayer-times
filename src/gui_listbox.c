/*
 * gui_listbox.c
 *
 *  Created on: 10.10.2021
 *      Author: ahmet
 */


#include "gui_internal.h"

enum ListboxClearCommand {
	ListboxClearUndefined,
	ListboxClearAssistantAddCity,
	ListboxClearDialogRemoveCity,

};

enum AddChildToListbox {
	createUndefined,
	createRadioButton,
	createCheckButton,
};


static GtkListBox* gtk_listbox_clear(GtkListBox* listbox, enum ListboxClearCommand command);
static GtkWidget* gui_create_and_add_child_to_listbox(GtkListBox* listbox, enum AddChildToListbox command, char const*const label, char const*const name, GtkRadioButton* parent);

/** \brief Clear @p listbox
 *
 * @param command determines if this function is applied to Add-City-Assistant or Remove-city-Dialog
 *
 */
static GtkListBox* gtk_listbox_clear(GtkListBox* listbox, enum ListboxClearCommand command) {
    GtkListBox* box_return = listbox;
    if(!listbox) return box_return;

    GtkWidget* parent = gtk_widget_get_parent(GTK_WIDGET(listbox));
    if(!parent || !GTK_IS_CONTAINER(parent)) return box_return;

    GtkListBox* new_listbox = GTK_LIST_BOX(gtk_list_box_new());
    if(!new_listbox) return box_return;

    if(command == ListboxClearAssistantAddCity) {
    	gtk_widget_set_name(GTK_WIDGET(new_listbox), "assistant_add_city_page1_listbox");
        assert(GTK_IS_GRID(parent));
        GtkGrid* grid = GTK_GRID(parent);
        gtk_grid_attach(grid, GTK_WIDGET(new_listbox), 1, 1, 1, 1);
    } else if(command == ListboxClearDialogRemoveCity) {
    	gtk_widget_set_name(GTK_WIDGET(new_listbox), "dlg_removecity_listbox");
        assert(GTK_IS_BOX(parent));
        GtkBox* box = GTK_BOX(parent);
        gtk_box_pack_end(box, GTK_WIDGET(new_listbox), false, false, 6);
    }

    gtk_widget_destroy(GTK_WIDGET(listbox));
    box_return = new_listbox;
    return box_return;
}

GtkListBox* gtk_listbox_clear_assistant_addcity(GtkListBox* listbox) {
	return gtk_listbox_clear(listbox, ListboxClearAssistantAddCity);
}

GtkListBox* gtk_listbox_clear_dialog_removeCity(GtkListBox* listbox) {
	return gtk_listbox_clear(listbox, ListboxClearDialogRemoveCity);
}


/** \brief Creates an child element and adds to listbox
 *
 * \param listbox parent
 * \param command determines whether CheckButton or RadioButton shall be added
 * \param label Label of newly created button
 * \param name name of newly created button (optional)
 * \param parent group of RadioButton (optional)
 *
 */
static GtkWidget* gui_create_and_add_child_to_listbox(GtkListBox* listbox, enum AddChildToListbox command, char const*const label, char const*const name, GtkRadioButton* parent) {
	GtkWidget* ret = 0;
	if(listbox && label) {
	    GtkListBoxRow* list_element = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	    if(!list_element) goto ERR;

	    switch(command) {
	    case createCheckButton:
	    	ret = gtk_check_button_new_with_label(label);
	    	break;

	    case createRadioButton:
	    	ret = gtk_radio_button_new_with_label_from_widget(parent, label);
	    	break;

	    default:
	    	assert(false);
	    }
	    if(!ret) goto ERR;
	    if(name) gtk_widget_set_name(GTK_WIDGET(ret), name);

	    gtk_container_add(GTK_CONTAINER(list_element), GTK_WIDGET(ret));
	    gtk_list_box_insert(listbox, GTK_WIDGET(list_element), -1);
	}
	return ret;
ERR:
	myperror("gui_create_and_add_child_to_listbox: Error creating element");
	return ret;
}

GtkCheckButton* gui_create_and_add_check_button(GtkListBox* listbox, char const*const label, char const*const name) {
	return GTK_CHECK_BUTTON(gui_create_and_add_child_to_listbox(listbox, createCheckButton, label, name, NULL));
}

GtkRadioButton* gui_create_and_add_radio_button(GtkListBox* listbox, GtkRadioButton* group, char const*const label, char const*const name) {
    return GTK_RADIO_BUTTON(gui_create_and_add_child_to_listbox(listbox, createRadioButton, label, name, group));
}
