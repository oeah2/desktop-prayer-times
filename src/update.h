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

#ifndef UPDATE_H_INCLUDED
#define UPDATE_H_INCLUDED

#include <stdbool.h>

enum Version {
    VERSION_MAIN_MAJOR = 0,
    VERSION_MAIN_MINOR = 1,
};

/** \brief Check if update is available
 *
 */
bool update_check_is_available(void);

/** \brief Get current installed version of this application
 *
 * \return char* string of installed version. Calling function takes ownership
 */
char* update_get_current_version(void);

/** \brief Get available version from internet
 *
 * \return char* string of available version. Calling function takes ownership.
 */
char* update_get_available_version(void);

#endif // UPDATE_H_INCLUDED
