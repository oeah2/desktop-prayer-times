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


#ifndef GEOLOCATION_H_INCLUDED
#define GEOLOCATION_H_INCLUDED

/** \brief Fetch the geolocation of a city
 *
 * \param city_name char* the name of the city
 * \return char* A string containing all matching cities and their latitudes and longitudes. (Format: Cityname lat long). Must be freed by calling function.
 *
 */
char* geolocation_get(char const*const city_name);

#endif // GEOLOCATION_H_INCLUDED
