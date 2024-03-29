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


#ifndef CITY_H_INCLUDED
#define CITY_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define NDEBUG 1
#include <assert.h>
#include "Salah_times_calc.h"

typedef struct City City;
/** \brief struct city contains the informations to process the diyanet prayer times. Create yourself a local variable with correct values, then once use init_diyanet and afterwards calculate prayer times only with city_id
 *
 * \param file_times pointer to local file which contains the prayer times
 * \param name name of city
 * \param id your self chosen id for the city; start with 0; don't skip numbers
 *
 */
struct City {
    FILE* file_times;
    char* name;
    char* filename;
    size_t id, pr_time_provider;						// id is depreciated now
    size_t method;                                      // must be element of enum ST_calculation_method
    size_t asr_juristic;                                // must be element of enum ST_juristic_method
    size_t adjust_high_lats;                            // must be element of enum ST_adjusting_method
    double latitude, longitude;
};

/** \brief Initializes an existing city element to diyanet
 *
 * \param c city to be initialized
 * \param name of city
 * \param filename name of prayer times file
 * \param id id of city, depreciated
 * \return pointer to initialized city
 *
 */
City* city_init_diyanet(City* c, char const*const name, size_t pr_time_provider, char const*const filename, size_t id);

/** \brief Initializes an existing city element to diyanet
 *
 * \param c city to be initialized
 * \param name of city
 * \param method of calculation
 * \param id id of city, depreciated
 * \return pointer to initialized city
 *
 */
City* city_init_calc(City* c, char const*const name, size_t pr_time_provider, size_t method, size_t id, double longitude, double latitude, size_t asr_juristic, size_t adjust_high_lats);

/** \brief Initializes an empty city
 *
 * \param c city to be initialized
 * \return pointer to initialized city
 *
 */
City* city_init_empty(City* c);

/** \brief Set name of city by copying into own storage
 *
 * \param c whose name is to be set
 * \param name new name
 * \return pointer to modified city
 *
 */
City* city_set_name(City* c, char const*const name);

/** \brief Destroys city element, without freeing c
 *
 * \param c city element to be destroyed
 * \return destroyed city
 *
 */

City* city_destroy(City* c);

/** \brief Creates and initializes new city
 *
 * \param name of city
 * \param filename name of prayer times file
 * \param id id of city, depreciated
 * \return pointer to new and initialized city
 *
 */

City* city_new(char const*const name, size_t pr_time_provider, char const*const filename, size_t id);

/** \brief Delete existing city. C is freed and pointer set to 0.
 *
 * \param c pointer to city to be deleted
 * \return pointer to freed city
 *
 */

City* city_delete(City* c);

/** \brief Create array of new cities
 *
 * \param num number of new cities to be created
 * \param name array to names of cities
 * \param filename array of filenames for prayer times files
 * \param id array of pointers, depreciated
 * \return pointer to newly created array of cities
 *
 */

City* city_vnew(size_t num, char const (*name)[num], size_t pr_time_provider[num], char const (*filename)[num], size_t id[num]);

/** \brief Creates a new city for calculation
 *
 * \param name char const*const
 * \param pr_time_provider size_t
 * \param method size_t
 * \param id size_t, depreciated
 * \param longitude double
 * \param latitude double
 * \param asr_juristic size_t
 * \param adjust_high_lats size_t
 * \return City*
 *
 */
City* city_new_calc(char const*const name, size_t pr_time_provider, size_t method, size_t id, double longitude, double latitude, size_t asr_juristic, size_t adjust_high_lats);

/** \brief Delete array of cities
 *
 * \param num number of elements in array
 * \param c pointer to array of cities
 * \return freed pointer to cities
 *
 */

City* city_vdelete(size_t num, City* c);

/** \brief Compare two cities for equality
 *
 * \return true if equal
 *
 */
bool city_is_equal(City a, City b);

#endif // CITY_H_INCLUDED
