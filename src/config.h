#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "prayer_times.h"
#include "city.h"

typedef struct Config Config;

struct Config {
    size_t /*pr_time_providers,*/ num_cities, counter_parsed_cities;
    City* cities;
    char lang[5];
    bool config_changed;
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

int config_save(char const*const filename, Config const* cfg);

int config_save_binary(char const*const filename, Config cfg);

int config_read_binary(char const*const filename, Config* cfg);

/** \brief Initialize config, if no config shall be read from file
 *
 * \param cfg pointer to struct Config
 * \return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 *
 */

Config* config_init(Config* cfg);

/** \brief Returns next free id for cities
 *
 * \param cfg pointer to struct Config, containing configuration of application
 * \return next free id
 *
 */

size_t config_find_next_id(Config const* cfg);

/** \brief Add city to config. User shall verify id of city is not already assigned to other city. Use find_next_id if possible
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

Config* config_remove_city(size_t city_id, Config* cfg);

/** \brief Clear config. Frees memory of cities and than calls @f config_init.
 *
 * \param cfg pointer to struct Config
 * \return pointer to cfg
 *
 */

Config* config_clear(Config* cfg);

#endif // CONFIG_H_INCLUDED
