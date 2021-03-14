#ifndef GEOLOCATION_H_INCLUDED
#define GEOLOCATION_H_INCLUDED

/** \brief Fetch the geolocation of a city
 *
 * \param city_name char* the name of the city
 * \return char* A string containing all matching cities and their latitudes and longitudes. (Format: Cityname lat long)
 *
 */
char* geolocation_get(char const*const city_name);

#endif // GEOLOCATION_H_INCLUDED
