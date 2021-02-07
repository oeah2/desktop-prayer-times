#ifndef CITY_H_INCLUDED
#define CITY_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define NDEBUG 1
#include <assert.h>

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
    size_t id, pr_time_provider;
    size_t method;
    double latitude, longitude;
};

/** \brief Initializes an existing city element to diyanet
 *
 * \param c city to be initialized
 * \param name of city
 * \param filename name of prayer times file
 * \param id id of city
 * \return pointer to initialized city
 *
 */
City* city_init_diyanet(City* c, char const*const name, size_t pr_time_provider, char const*const filename, size_t id);

/** \brief Initializes an existing city element to diyanet
 *
 * \param c city to be initialized
 * \param name of city
 * \param method of calculation
 * \param id id of city
 * \return pointer to initialized city
 *
 */
City* city_init_calc(City* c, char const*const name, size_t pr_time_provider, size_t method, size_t id, double longitude, double latitude);

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
 * \param id id of city
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
 * \param id array of pointers
 * \return pointer to newly created array of cities
 *
 */

City* city_vnew(size_t num, char const (*name)[num], size_t pr_time_provider[num], char const (*filename)[num], size_t id[num]);

/** \brief Delete array of cities
 *
 * \param num number of elements in array
 * \param c pointer to array of cities
 * \return freed pointer to cities
 *
 */

City* city_vdelete(size_t num, City* c);

#endif // CITY_H_INCLUDED
