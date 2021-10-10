/*
 * gui_internal.h
 *
 *  Created on: 10.10.2021
 *      Author: ahmet
 */

#ifndef SRC_GUI_INTERNAL_H_
#define SRC_GUI_INTERNAL_H_

#include <stdlib.h>
#include <stdbool.h>
//#define NDEBUG 1
#include <assert.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "gui.h"
#include "geolocation.h"
#include "cJSON.h"
#include "config.h"
#include "socket.h"
#include "update.h"
#include "error.h"
#include "hadith.h"


extern Config* cfg;
extern size_t city_ptr;
extern int day_ptr;
extern GtkButton* btn_next_city;
extern GtkButton* btn_next_date;
extern GtkButton* btn_prev_city;

#endif /* SRC_GUI_INTERNAL_H_ */
