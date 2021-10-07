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

#include <assert.h>
#include "file.h"

#define FILE_ADDITIONAL_BUFFER 10

/** \brief Returns length of file
 *
 * \param file FILE* of which the lengh shall be determined
 * \return size_t file length, including terminating zero
 *
 */
static size_t file_find_actual_length(FILE* file) {
    size_t ret = 0;
    if(file) {
        rewind(file);
        int fseek_result = fseek(file, 0, SEEK_END);
        assert(!fseek_result);
        ret = ftell(file);
        rewind(file);
    }
    return ret;
}

size_t file_find_length(FILE* file)
{
    return file_find_actual_length(file) + FILE_ADDITIONAL_BUFFER;
}

bool file_read_all(FILE* file, size_t max_len, char dest[max_len])
{
    bool ret = false;
    if(file) {
        size_t file_length = file_find_actual_length(file);
        if(file_length > max_len)
            return ret;

        size_t read_bytes = fread(dest, sizeof(char), file_length, file);
        assert(max_len > read_bytes);
        dest[read_bytes] = '\0';
        if(read_bytes < (file_length - 1) && !feof(file)) {
            myperror("file_read_all: Error while reading config file.");
            return false;    // depends on library implementation(?)
        }
        ret = true;
    }
    return ret;
}
