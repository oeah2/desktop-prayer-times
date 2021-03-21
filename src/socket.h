#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#endif

#ifdef DEPRECATED
int socket_init(void);
int socket_deinit(void);
#endif // DEPRECATED

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
