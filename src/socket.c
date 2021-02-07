#include "socket.h"

int sock_init(void)
{
#ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1,1), &wsa_data);
#else
    return 0;
#endif
}

int sock_quit(void)
{
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}

int sock_close(SOCKET sock)
{

    int status = 0;

#ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0) {
        status = closesocket(sock);
    }
#else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) {
        status = close(sock);
    }
#endif

    return status;

}

int sock_send(size_t length, unsigned char data[length])
{
    return 0;
}

int sock_recv(size_t max_length, unsigned char data[max_length])
{
    return 0;
}


