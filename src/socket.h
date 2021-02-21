#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include <unistd.h>

enum {
    SOCK_OK,
    SOCK_ERR_INIT,
    SOCK_ERR_ADDRINFO,
};

int socket_init(void);
int socket_deinit(void);
char* http_get(char const*const host, char const*const file);
