#pragma once 

#include <cstddef>

#ifndef _WIN32
#   include <fcntl.h>
#   include <netinet/in.h>
#   include <sys/socket.h>
#   include <sys/types.h>
#   include <unistd.h>
#else
#   include <winsock2.h>
#endif

namespace network {
namespace socket {

/**
 * A type wide enough to hold the output of "socket()" or "accept()".  On
 * Windows, this is an intptr_t; elsewhere, it is an int. */
#ifdef _WIN32
    typedef intptr_t socket_t;
#else 
    typedef int socket_t;
#endif

socket_t create(int domain, int type, int protocol);
int close(socket_t socket);

int listen(socket_t sock, int backlog);

int bind(socket_t sock, const struct sockaddr *addr);
int bind(socket_t sock, const struct sockaddr_in *addr);
int bind(socket_t sock, const struct sockaddr_in6 *addr);

int connect(socket_t sock, const struct sockaddr *addr);
int connect(socket_t sock, const struct sockaddr_in *addr);
int connect(socket_t sock, const struct sockaddr_in6 *addr);

int accept(socket_t sock);
int accept(socket_t sock, struct sockaddr_in *addr);
int accept(socket_t sock, struct sockaddr_in6 *addr);

int read(socket_t sock, void *buff, size_t count);
int write(socket_t sock, const void *buf, size_t count);

int setnonblocking(socket_t sock);
int setcloseexec(socket_t sock);

int socketpair(int domain, int type, int protocol, socket_t sv[2]);

} // namespace socket
} // namespace network