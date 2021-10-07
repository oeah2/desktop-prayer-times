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

#ifndef LANG_H_INCLUDED
#define LANG_H_INCLUDED

#include <stdbool.h>

enum Languages {
    LANG_EN = 0,
    LANG_DE,
    LANG_TR,

    LANG_NUM,
};

bool lang_is_available(enum Languages ID);
char* lang_get_filename(enum Languages ID);

extern char const*const lang_names[LANG_NUM];


#endif // LANG_H_INCLUDED
