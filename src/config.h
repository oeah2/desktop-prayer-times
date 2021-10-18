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

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "prayer_times.h"
#include "city.h"
#include "lang.h"
#include "update.h"

typedef struct Config Config;

struct Config {
    size_t num_cities;
    size_t counter_parsed_cities;
    size_t lang;
    size_t displayed_cityID;
    int last_window_posX;
    int last_window_posY;
    City* cities;
    bool config_changed;
    bool save_position;
    bool check_for_updates;
    bool enable_notification;
    char /*const*/* cfg_filename;
};

/** \brief Read config from filename and write into Config* pointer
 *
 * \param filename of config file
 * \param cfg pointer to struct Config, containing configuration of application
 * \return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */

int config_read(char const*const filename, Config* cfg);

/** \brief Save config into filename
 *
 * \param filename of config file
 * \param cfg pointer to struct Config, containing configuration of application
 * \return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */

int config_save(char const*const filename, Config const*const cfg);

/** \brief Save config into filename using JSON format
 *
 * \param filename char const*const
 * \param cfg Config*const pointer to config of application
 * \return int EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */
int config_json_save(char const*const filename, Config*const cfg);

/** \brief Initialize config, if no config shall be read from file
 *
 * \param cfg pointer to struct Config
 * \return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */

Config* config_init(Config* cfg);

/** \brief Find position of City id
 *
 * \param city_id size_t id of city
 * \param cfg Config const*const
 * \return size_t position of city
 *
 */
size_t config_find_idpos(size_t city_id, Config const*const cfg);

/** \brief Returns next free id for cities
 *
 * \param cfg pointer to struct Config, containing configuration of application
 * \return next free id
 *
 */

size_t config_find_next_id(Config const*const cfg);

/** \brief Add city to config. ID of city is ignored (depreciated).
 *
 * \param city to be added to config
 * \param cfg pointer to struct Config, containing configuration of application
 * \return EXIT_SUCCESS on success, EXIT_Failure otherwise
 *
 */

int config_add_city(City c, Config* cfg);

/** \brief Remove city from config.
 *
 * \param city_id id of city to be removed
 * \param cfg pointer to struct Config, containing configuration of application
 * \return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */

int config_remove_city(size_t city_id, Config* cfg);

/** \brief Clear config. Frees memory of cities and than calls @f config_init.
 *
 * \param cfg pointer to struct Config
 * \return pointer to cfg
 *
 */

Config* config_clear(Config* cfg);

/** \brief Swap two cities in config.
 *
 * \param cfg pointer to struct Config
 * \param city1 first city
 * \param city2 second city
 * \return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */
int config_swap_cities(size_t const city1, size_t const city2, Config*const cfg);

/** \brief Move a City position in config.
 *
 * \param cfg pointer to struct Config
 * \param id_initial Initial position of city
 * \param id_after Position of City after this function
 * \return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */
int config_move_city(size_t const id_initial, size_t const id_after, Config*const cfg);


#endif // CONFIG_H_INCLUDED
