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

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/applink.c>
#else
#define __USE_XOPEN2K
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#endif
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include "socket.h"
#include "update.h"
#include "error.h"

//#define USE_LIBCURL
#ifdef USE_LIBCURL
#include <curl/curl.h>
#endif // USE_LIBCURL

//#define DEBUG 1

enum {
    SOCK_OK,
    SOCK_ERR_INIT,
    SOCK_ERR_ADDRINFO,
};

static size_t http_find_header_length(char const*const http_response);
static size_t http_find_resp_length(char const*const http_response);

/** \brief Initialize socket
 *
 */
static int socket_init(void)
{
#ifdef _WIN32
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(1,1), &wsaData)) {
    	myperror(__FILE__, __LINE__, "Error during Socket initialization.");
        return SOCK_ERR_INIT;
    }
#endif
    return SOCK_OK;
}

/** \brief Deinitialize socket
 *
 */
static int socket_deinit(void)
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


/** \brief Set socket to non blocking
 *
 * \param fd socket
 * \param blocking true for blocking
 * \return true on success
 *
 */
bool socket_set_blocking(int fd, bool blocking)
{
   if (fd < 0) return false;

#ifdef _WIN32
   unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1) return false;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
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
    	myperror(__FILE__, __LINE__, "Error getting addrinfo.");
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
#if OLDRECVALL
static int socket_receiveall(int sock_id, char* msg, size_t max_len, int flags)
{
    int received = 0, buff_pos = 0;
    while((received = socket_receive(sock_id, msg + buff_pos, max_len - buff_pos, flags))) {
        if(received <= -1) {
#ifdef _WIN32
            int ret = WSAGetLastError();
#else
            int ret = errno;
#endif // _WIN32
            if(!buff_pos)
                return ret;
            else
                break;
        }
        buff_pos += received;
    }
    return buff_pos;
}
#elif NEWRECVALL
static int socket_receiveall(int sock_id, char* msg, size_t max_len, int flags)
{
    int last_val = 0, received = 0;
    while(received = socket_receive(sock_id, msg, max_len, MSG_PEEK)) {
        if(received == -1) {
#ifdef _WIN32
            int ret = WSAGetLastError();
#else
            int ret = errno;
#endif // _WIN32
            return ret;
        }
        if(received != last_val)
            last_val = received;
        else
            break;
    }
    if(last_val > max_len) return -1;

    int ret = socket_receive(sock_id, msg, last_val, flags);
    http_find_resp_length(msg);
    http_find_header_length(msg);
    return ret;
}
#elif NEWESTRECVALL
static int socket_receiveall(int sock_id, char* msg, size_t max_len, int flags)
{
    int received = 0;
    int buff_pos = 0;
    size_t header_length = 0;

    do {
        received = socket_receive(sock_id, msg + buff_pos, max_len - buff_pos, flags);
        if(received == -1) goto ERR_RECV;
        buff_pos += received;
        if(!http_is_response_ok(msg)) break;                        // error in response
        if((header_length = http_find_header_length(msg))) break;     // header complete
    } while(received != -1);

    size_t resp_length = http_find_resp_length(msg);

    while(buff_pos < resp_length + header_length) {
        received = socket_receive(sock_id, msg + buff_pos, max_len - buff_pos, flags);
        if(received == -1) goto ERR_RECV;
        buff_pos += received;
    }
    return buff_pos;

    int ret = 0;
ERR_RECV:
#ifdef _WIN32
    ret = WSAGetLastError();
#else
    ret = errno;
#endif // _WIN32
    if(!buff_pos)
        return ret;
    return ret;
}
#endif // NEWRECVALL

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

/** \brief Returns the content length of the http response according to the http header
 *
 * \param http_response char const*const http response of the server
 * \return size_t length according to http header
 *
 */
static size_t http_find_resp_length(char const*const http_response)
{
    size_t ret = 0;
    if(http_is_response_ok(http_response)) {            // response valid
        if(strstr(http_response, "\r\n\r\n")) {         // header complete
            char* pos_length = strstr(http_response, "Content-Length: ");
            int scan = sscanf(pos_length, "Content-Length: %zu", &ret);
            assert(scan);
        }
    }
    return ret;
}

/** \brief Return the length of the http header
 *
 * \param http_response char const*const http response of the server
 * \return size_t length of header
 *
 */
static size_t http_find_header_length(char const*const http_response)
{
    size_t ret = 0;
    if(http_is_response_ok(http_response)) {
        char* pos_header_end = strstr(http_response, "\r\n\r\n") + strlen("\r\n\r\n");   // find end of header, -1 necessary?
        if(pos_header_end) {
            ptrdiff_t length = pos_header_end - http_response;
            ret = length;
        }
    }
    return ret;
}

/** \brief Checks whether the http response is complete.
 * The actual content length must match the content length given in the http header.
 *
 * \param http_response char const*const http response of the server
 * \return bool true if complete, false otherwise
 *
 */
static bool http_is_response_complete(char const*const http_response)
{
    bool ret = false;
    if(http_response) {
    	if(http_is_response_ok(http_response)) {
			size_t header_length = http_find_header_length(http_response);
			size_t resp_setpoint = http_find_resp_length(http_response);
			int actual_resp = strlen(http_response) - header_length;
			if(actual_resp > 0 && header_length && resp_setpoint && actual_resp >= resp_setpoint) { // Todo tbe last comparison should be == instead of >=
				ret = true;
			}
        }
    }
    return ret;
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
    char const*const close = "close";
    char const*const keep = "keep-alive";
    char const*const method = keep;
    if(request) {
#ifdef OLDRECVALL
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nAccept: text/plain\r\n%s\r\n\r\n", file, host, add_info ? add_info : "");
#else
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: %s\r\nAccept: text/plain\r\n%s\r\n\r\n", file, host, method, add_info ? add_info : "");

//        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nAccept: text/plain\r\n%s\r\n\r\n", file, host, add_info ? add_info : "");
#endif // OLDRECVALL
        char* new_req = realloc(request, strlen(request) + 1);
        if(new_req) {
            request = new_req;
        }
    }
    return request;

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
        if(strstr(http_response, "\r\n\r\n")) {
            size_t length = strlen(http_response);
            char* header_end = strstr(http_response, "\r\n\r\n") + strlen("\r\n\r\n"); // ToDo: header_end kann nie 0 werden, weil strlen immer 4 ist.
            ptrdiff_t header_length = header_end - http_response;
            assert(header_length > 0);
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

/** \brief Receive whole message from host
 *
 * \param sock_id int id of socket
 * \param msg char* destination array
 * \param max_len size_t max length of @p msg
 * \param flags int additional flags
 * \return int
 *
 */
static int http_receiveall(int sock_id, char* msg, size_t max_len, int flags)
{
    int received = 0;
    int buff_pos = 0;

RECV:
    do {
        received = socket_receive(sock_id, msg + buff_pos, max_len - buff_pos, flags);
        if(received == -1) goto ERR_RECV;
        buff_pos += received;
        if(http_is_response_ok(msg)) {
        	if(http_is_response_complete(msg) || received == 0) {
        		break;
        	}
        }
        if(!http_is_response_ok(msg)) break;                        // error in response
    } while(received != -1);

    return buff_pos;

    int ret = 0;
ERR_RECV:
#ifdef _WIN32
    ret = WSAGetLastError();
    if(ret == WSAEWOULDBLOCK) {
    	goto RECV;
    }
#else
    ret = errno;
#endif // _WIN32
    return ret;
}

/** \brief Generate user agent for http request
 *
 * \param void
 * \return char* string containing user agent, must be freed by user
 *
 */
static char* socket_get_useragent(void)
{
    char* ret = malloc(100 * sizeof(char));
    if(ret) {
        strcpy(ret, "User-Agent: desktop_prayer_time/");
        char* current_version = update_get_current_version();
        assert(current_version);
        strcat(ret, current_version);
        free(current_version);
        char* new_mem = realloc(ret, (strlen(ret) + 1) * sizeof(char));
        ret = new_mem ? new_mem : ret;
    }
    return ret;
}


/** \brief Connect to host and request file using HTTP. Add_info will be sent in request
 *
 * \param host char const*const address of host
 * \param file char const*const requested file
 * \param add_info char const*const additional info to be sent in header
 * \return char* server response, http header removed. 0 if no valid response
 *
 */
#ifndef USE_LIBCURL
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
#ifdef DEBUG
        printf("HTTP_Get: Sent Request: %s\n", http_request);
#endif // DEBUG
        size_t buf_len = 100E3;
        buffer = calloc(buf_len, sizeof(char));
        if(!buffer) goto ERR_SEND;
		size_t received_bytes = 0;
		if(!socket_set_blocking(s, false)) {
			myperror(__FILE__, __LINE__, "Error setting socket to nonblocking");
			return ret;
		}
	RECV:
#ifdef SOCKET_RECVALL
        received_bytes += socket_receiveall(s, buffer, buf_len, 0);
#else
        received_bytes += http_receiveall(s, buffer + received_bytes, buf_len, 0); // Todo: Set MSG_WAITALL?
#endif // HTTP_RECVALL
        //assert(received_bytes);
#ifdef DEBUG
        printf("HTTP_Get Received bytes: %zu\n", received_bytes);
        printf("HTTP_Get Received data: \n%s\n", buffer);
#endif // DEBUG
		if(!http_is_response_complete(buffer)) {
			myperror(__FILE__, __LINE__, "Error during http_get receive");
			goto ERR_RECV;
		}
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
	myperror(__FILE__, __LINE__, "Error during receive");
    free(buffer);
ERR_SEND:
    free(http_request);
ERR_SOCKET:
    socket_close(s);
    return 0;
}
#else
char* http_get(char const*const host, char const*const file, char const*const add_info)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, host);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
        	myperror(__FILE__, __LINE__, "Error performing curl operationg!");

        }
        curl_easy_cleanup(curl);
    }
}
#endif

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
	myperror(__FILE__, __LINE__, msg);
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
    if (!SSL_CTX_load_verify_locations(*ctx_in,
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
    SSL_CTX* ctx = NULL;
    BIO* bio = https_connect(host, &ctx);
    char* http_request = http_create_request(host, file, add_info);
    int sent_bytes = BIO_puts(bio, http_request);
    if(sent_bytes == -1 || sent_bytes == 0) {
    	myperror(__FILE__, __LINE__, "Error while sending data over HTTPS socket!");
        return 0;
    }
    assert(strlen(http_request) == sent_bytes);
    free(http_request);
    http_request = NULL;

    char* http_response = https_receive(bio);
	if(!http_is_response_complete(http_response)) {
		myperror(__FILE__, __LINE__, "Error during receiving of https_get");
	}
    https_cleanup(ctx, bio);
    if(http_is_response_ok(http_response)) {
    	http_response = http_remove_header(http_response);
    } else {
        free(http_response);
        http_response = NULL;
    }
    return http_response;
}

char* https_get_with_useragent(char const*const host, char const*const file, char const*const add_info)
{
    char* ret = 0;
    size_t buffer_length = 150;
    char* user_agent = socket_get_useragent();
    if(user_agent) {
        assert(strlen(user_agent) < buffer_length - 1);
        if(add_info) {
            buffer_length += strlen(add_info);
        }
        char buffer[buffer_length];
        if(add_info) {
            strcpy(buffer, add_info);
            strcat(buffer, user_agent);
        } else {
            strcpy(buffer, user_agent);
        }
        free(user_agent);
        user_agent = 0;

        ret = https_get(host, file, buffer);
    }
    return ret;
}
