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


#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

/** \brief Returns length of file with additional buffer at end
 *
 * \param file FILE* of which the lengh shall be determined
 * \return size_t file length, including terminating zero and additional buffer
 *
 */
size_t file_find_length(FILE* file);

/** \brief Reads all content of file into destination array
 *
 * \param file FILE*
 * \param max_len size_t max size to be read, including terminating 0
 * \param dest[max_len] char
 * \return bool true on success, false otherwise
 *
 */
bool file_read_all(FILE* file, size_t max_len, char dest[max_len]);

#endif // FILE_H_INCLUDED
