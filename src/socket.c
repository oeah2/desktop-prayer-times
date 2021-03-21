#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#ifdef _WIN32
#include <openssl/applink.c>
#endif
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include "socket.h"

enum {
    SOCK_OK,
    SOCK_ERR_INIT,
    SOCK_ERR_ADDRINFO,
};

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

/** \brief Closes Socket
 *
 * \param sock_id int socket to be closed
 * \return int
 *
 */
static int socket_close(int sock_id)
{
#ifdef _WIN32
    return closesocket(sock_id);
#else
    return close(sock_id);
#endif
}

/** \brief Connect to socket
 *
 * \param addr char const*const address information
 * \return int socket
 *
 */
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

/** \brief Send data oversocket
 *
 * \param sock_id int id of socket
 * \param msg char const* message to be send
 * \param msg_len size_t length of message
 * \return int
 *
 */
inline
static int socket_send(int sock_id, char const* msg, size_t msg_len)
{
    return send(sock_id, msg, msg_len, 0);
}

/** \brief Send whole message
 *
 * \param sock_id int id of socket
 * \param msg char const* message to be sent
 * \param msg_len size_t length of message
 * \return int
 *
 */
static int socket_sendall(int sock_id, char const* msg, size_t msg_len)
{
    size_t msg_sent = 0;
    do {
        msg_sent += socket_send(sock_id, msg, msg_len);
    } while (msg_sent < msg_len);
    return msg_sent;
}

/** \brief Receive data from socket
 *
 * \param sock_id int id of socket
 * \param msg char* destination array
 * \param max_len size_t max length of @p msg
 * \param flags int additional flags
 * \return int
 *
 */
inline
static int socket_receive(int sock_id, char* msg, size_t max_len, int flags)
{
    return recv(sock_id, msg, max_len, flags);
}

/** \brief Receive whole message from host
 *
 * \param sock_id int id of socket
 * \param msg char* destination array
 * \param max_len size_t max length of @p msg
 * \param flags int additional flags
 * \return int
 *
 */
static int socket_receiveall(int sock_id, char* msg, size_t max_len, int flags)
{
    int received = 0, buff_pos = 0;
    while((received = socket_receive(sock_id, msg + buff_pos, max_len - buff_pos, 0))) {
        if(received > -1) {
#ifdef _WIN32
            int ret = WSAGetLastError();
#else
            int ret = errno;
#endif // _WIN32
            return ret;
        }
        buff_pos += received;
    }
    return buff_pos;
}

/** \brief Creates http request. needs to be freed by the user
 *
 * \param host char const*const host to be connected
 * \param file char const*const file to be requested
 * \param add_info char const*const additional info to be placed into http header
 * \return char* string containing http 1.1 request
 *
 */
static char* http_create_request(char const*const host, char const*const file, char const*const add_info)
{
    if(!host || !file) {
        return 0;
    }

    size_t const header_max = 2000;
    char* request = calloc(header_max, sizeof(char));
    if(request) {
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nAccept: text/plain\r\n%s\r\n\r\n", file, host, add_info ? add_info : "");
#ifdef OLD_CODE
        strcpy(request, "GET ");
        strcat(request, file);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, host);
        strcat(request, "\r\nConnection: close\r\nAccept: text/plain\r\n\r\n");
#endif // OLD_CODE
        char* new_req = realloc(request, strlen(request + 1));
        if(new_req) {
            request = new_req;
        }
    }
    return request;

}

/** \brief Checks http response for validity ("200 OK")
 *
 * \param http_response char const*const http response to be checked
 * \return bool true of OK, false otherwise
 *
 */
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

/** \brief removes http header from http response
 *
 * \param http_response char* response of remote computer
 * \return char* containing http body
 *
 */
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


char* http_get(char const*const host, char const*const file, char const*const add_info)
{
    char* ret = 0;
    int s = 0;
    char* http_request = 0, *buffer = 0;
    if(host && file) {
        socket_init();
        s = socket_connect(host);
        http_request = http_create_request(host, file, add_info);
        if(!http_request) goto ERR_SOCKET;
        if(!socket_sendall(s, http_request, strlen(http_request) + 1)) goto ERR_SEND;

        size_t buf_len = 100E3;
        buffer = calloc(buf_len, sizeof(char));
        if(!buffer) goto ERR_SEND;
        size_t received_bytes = socket_receiveall(s, buffer, buf_len, 0);
        //assert(received_bytes);
        if(!received_bytes || !http_is_response_ok(buffer)) goto ERR_RECV;
        buffer = http_remove_header(buffer);
        assert(buffer);
        ret = buffer;

        socket_close(s);
        free(http_request);
        socket_deinit();
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

bool socket_check_connection()      // Das ist keine schoene Loesung, sollte aber funktionieren.
{
    char* ret = http_get("www.google.com", "/", 0);
    if(ret) {
        free(ret);
        return true;
    }
    return false;
}

/** \brief Reports error message from openSSL library
 *
 */
static void report_and_exit(const char* msg)
{
    perror(msg);
    ERR_print_errors_fp(stderr);
    exit(-1);
}

/** \brief Initialize openSSL and HTTPS
 *
 * \return void
 *
 */
static void https_init()
{
    SSL_load_error_strings();
    SSL_library_init();
}

/** \brief Cleanup openSSL and HTTPS
 *
 * \param ctx SSL_CTX*
 * \param bio BIO*
 * \return void
 *
 */
static void https_cleanup(SSL_CTX* ctx, BIO* bio)
{
    SSL_CTX_free(ctx);
    BIO_free_all(bio);
}

/** \brief Connect via HTTP to host
 *
 * \param hostname const char* hostname to be connected to
 * \param ctx_in SSL_CTX**
 * \return BIO*
 *
 */
static BIO* https_connect(const char* hostname, SSL_CTX** ctx_in)
{
    size_t BuffSize = 1000;
    char name[BuffSize];

    const SSL_METHOD* method = TLS_client_method();
    if (NULL == method) report_and_exit("TLSv1_2_client_method...");

    *ctx_in = SSL_CTX_new(method);
    if (NULL == *ctx_in) report_and_exit("SSL_CTX_new...");

    BIO* bio = BIO_new_ssl_connect(*ctx_in);
    if (NULL == bio) report_and_exit("BIO_new_ssl_connect...");

    SSL* ssl = NULL;

    /* link bio channel, SSL session, and server endpoint */

    sprintf(name, "%s:%s", hostname, "https");
    BIO_get_ssl(bio, &ssl); /* session */
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY); /* robustness */
    BIO_set_conn_hostname(bio, name); /* prepare to connect */

    /* try to connect */
    if (BIO_do_connect(bio) <= 0) {
        https_cleanup(*ctx_in, bio);
        report_and_exit("BIO_do_connect...");
    }

#define SKIP_VERIFICATION
#ifndef SKIP_VERIFICATION
    /* verify truststore, check cert */
    if (!SSL_CTX_load_verify_locations(ctx,
                                       "/etc/ssl/certs/ca-certificates.crt", /* truststore */
                                       "/etc/ssl/certs/")) /* more truststore */
        report_and_exit("SSL_CTX_load_verify_locations...");

    long verify_flag = SSL_get_verify_result(ssl);
    if (verify_flag != X509_V_OK)
        fprintf(stderr,
                "##### Certificate verification error (%i) but continuing...\n",
                (int) verify_flag);

#endif // SKIP_VERIFICATION

    return bio;
}

/** \brief Receives https reponse from bio
 *
 * \param bio BIO*
 * \return char*
 *
 */
static char* https_receive(BIO* bio)
{
    size_t resp_len = 1E6, recv_len = 0;
    char* response = calloc(resp_len, sizeof(char));
    /* read HTTP response from server and print to stdout */
    while (1) {
        int n = BIO_read(bio, response + recv_len, resp_len - recv_len);
        if (n <= 0) break; /* 0 is end-of-stream, < 0 is an error */
        recv_len += n;
    }
    response = realloc(response, strlen(response) + 10);
    return response;
}

char* https_get(char const*const host, char const*const file, char const*const add_info)
{
    https_init();
    SSL_CTX* ctx = 0;
    BIO* bio = https_connect(host, &ctx);
    char* http_request = http_create_request(host, file, add_info);
    BIO_puts(bio, http_request);
    free(http_request);

    char* http_response = https_receive(bio);
    //#define DEBUG
#ifdef DEBUG
    printf("Http response: %s\n", http_response);
#endif // DEBUG
    https_cleanup(ctx, bio);
    if(http_is_response_ok(http_response)) {
        http_remove_header(http_response);
    } else {
        free(http_response);
        http_response = 0;
    }
    return http_response;
}
