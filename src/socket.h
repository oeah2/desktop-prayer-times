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
