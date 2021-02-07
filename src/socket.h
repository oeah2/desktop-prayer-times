#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
/* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
typedef SOCKET int;
#endif

int sock_init(void);
int sock_connect(void);
int sock_quit(void);
int sock_send(size_t length, unsigned char[length]);
int sock_recv(size_t max_length, unsigned char[max_length]);

#endif // SOCKET_H_INCLUDED
