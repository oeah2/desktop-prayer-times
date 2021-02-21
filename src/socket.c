#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "socket.h"

int socket_init(void)
{
#ifdef _WIN32
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(1,1), &wsaData)) {
        perror("Error during Socket initialization.");
        return SOCK_ERR_INIT;
    }
#endif
    return SOCK_OK;
}

int socket_deinit(void)
{
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}

static int socket_close(int sock_id)
{
#ifdef _WIN32
    return closesocket(sock_id);
#else
    return close(sock_id);
#endif
}

static int socket_connect(char const*const addr)
{
    struct addrinfo hints = {0}, *res = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(addr, "http", &hints, &res)) {
        perror("Error getting addrinfo.");
        return SOCK_ERR_ADDRINFO;
    }

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    connect(s, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    return s;
}

inline
static int socket_send(int sock_id, char const* msg, size_t msg_len)
{
    return send(sock_id, msg, msg_len, 0);
}

static int socket_sendall(int sock_id, char const* msg, size_t msg_len)
{
    size_t msg_sent = 0;
    do {
        msg_sent += socket_send(sock_id, msg, msg_len);
    } while (msg_sent < msg_len);
    return msg_sent;
}

inline
static int socket_receive(int sock_id, char* msg, size_t max_len, int flags)
{
    return recv(sock_id, msg, max_len, flags);
}

static int socket_receiveall(int sock_id, char* msg, size_t max_len, int flags)
{
    size_t received = 0, buff_pos = 0;
    while((received = socket_receive(sock_id, msg + buff_pos, max_len - buff_pos, 0))) {
        buff_pos += received;
    }
    return buff_pos;
}


static char* http_create_request(char const*const host, char const*const file)
{
    if(!host || !file) {
        return 0;
    }

    size_t const header_max = 2000;
    char* request = malloc(header_max);
    if(request) {
        strcpy(request, "GET ");
        strcat(request, file);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, host);
        strcat(request, "\r\nConnection: close\r\nAccept: text/plain\r\n\r\n");
        char* new_req = realloc(request, strlen(request + 1));
        if(new_req) {
            request = new_req;
        }
    }
    return request;

}

static bool http_is_response_ok(char const*const http_response)
{
    bool ret = false;
    if(http_response) {
        if(strstr(http_response, "HTTP/1.1 200 OK")) {
            ret = true;
        }
    }
    return ret;
}

static char* http_remove_header(char* http_response)
{
    if(http_response) {
        size_t length = strlen(http_response);
        char* header_end = strstr(http_response, "\r\n\r\n") + strlen("\r\n\r\n");
        if(header_end) {
            ptrdiff_t header_length = header_end - http_response;
            char buffer[length - header_length + 1];
            strcpy(buffer, header_end);
            http_response = realloc(http_response, length - header_length + 1);
            if(http_response) {
                strcpy(http_response, buffer);
            }
        }
    }
    return http_response;
}


char* http_get(char const*const host, char const*const file)
{
    char* ret = 0;
    int s = 0;
    char* http_request = 0, *buffer = 0;
    if(host && file) {
        s = socket_connect(host);
        http_request = http_create_request(host, file);
        if(!http_request) goto ERR_SOCKET;
        if(!socket_sendall(s, http_request, strlen(http_request) + 1)) goto ERR_SEND;

        size_t buf_len = 100E3;
        buffer = malloc(buf_len);
        if(!buffer) goto ERR_SEND;
        size_t received_bytes = socket_receiveall(s, buffer, buf_len, 0);
        assert(received_bytes);
        if(!http_is_response_ok(buffer)) goto ERR_RECV;
        buffer = http_remove_header(buffer);
        assert(buffer);
        ret = buffer;

        socket_close(s);
        free(http_request);
    }
    return ret;

ERR_RECV:
    perror("Error during receive");
    free(buffer);
ERR_SEND:
    free(http_request);
ERR_SOCKET:
    socket_close(s);
    return 0;
}
