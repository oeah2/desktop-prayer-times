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

#include <stdbool.h>
#include <errno.h>

/** \brief A very simple http request is being made and the result returned. The returned string needs to be freed by the user
 * \details This function initializes the socket interface, connects to @p host, requests @p file and adds @p add_info into the request header.
    The returned message is being checked for validity. If valid, the http header is removed and the http body returned.
 *
 * \param host char const*const host to be connected
 * \param file char const*const file to be requested
 * \param add_info char const*const Additional informations to be placed into the http request header. If no additional info shall be placed into header, set 0
 * \return char*
 *
 */
char* http_get(char const*const host, char const*const file, char const*const add_info);

/** \brief Checks the internet availability
 *  \details A HTTP request is sent to google and the reponse is checked for validity.
 * \return bool true of internet is available, false otherwise
 *
 */
bool socket_check_connection();

/** \brief A very simple http request is being made and the result returned. The returned string needs to be freed by the user
 * \details This function initializes the socket interface, connects to @p host, requests @p file and adds @p add_info into the request header.
    The returned message is being checked for validity. If valid, the http header is removed and the http body returned.
 *
 * \param host char const*const host to be connected
 * \param file char const*const file to be requested
 * \param add_info char const*const Additional informations to be placed into the http request header. If no additional info shall be placed into header, set 0
 * \return char*
 *
 */
char* https_get(char const*const host, char const*const file, char const*const add_info);

/** \brief A very simple http request is being made and the result returned. The returned string needs to be freed by the user. This function additionally transmits the user agent.
 * \details This function initializes the socket interface, connects to @p host, requests @p file and adds @p add_info into the request header.
    The returned message is being checked for validity. If valid, the http header is removed and the http body returned.
 *
 * \param host char const*const host to be connected
 * \param file char const*const file to be requested
 * \param add_info char const*const Additional informations to be placed into the http request header. If no additional info shall be placed into header, set 0
 * \return char*
 *
 */
char* https_get_with_useragent(char const*const host, char const*const file, char const*const add_info);
