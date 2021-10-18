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
