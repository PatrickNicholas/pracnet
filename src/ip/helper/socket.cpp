#include <network/ip/helper/socket.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

namespace network {

namespace socket {

namespace {

class SocketHelper {
   public:
    SocketHelper() {
#ifdef _WIN32
        WORD sockVersion = MAKEWORD(2, 2);
        WSADATA wsaData;
        if (WSAStartup(sockVersion, &wsaData) != 0) {
            fprintf(stderr, "initialize WSAStartup failed.");
            exit(1);
        }
#endif
    }

    void doSomething() {
        // ignore...
    }

    ~SocketHelper() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

}  // namespace

typedef struct sockaddr SA;

socket_t create(int domain, int type, int protocol) {
    // for initialize WSA environment.
    static SocketHelper helper;

    return ::socket(domain, type, protocol);

    // clear nouse warning.
    helper.doSomething();
}

int close(socket_t sock) {
#ifdef _WIN32
    return ::closesocket(sock);
#else
    return ::close(sock);
#endif
}

int bind(socket_t sock, const struct sockaddr* addr, socklen_t len) {
    return ::bind(sock, addr, len);
}

int bind(socket_t sock, const struct sockaddr_in* addr) {
    return ::bind(sock, (const SA*)addr, static_cast<socklen_t>(sizeof(*addr)));
}

int bind(socket_t sock, const struct sockaddr_in6* addr) {
    return ::bind(sock, (const SA*)addr, static_cast<socklen_t>(sizeof(*addr)));
}

int connect(socket_t sock, const struct sockaddr* addr, socklen_t len) {
    return ::connect(sock, addr, len);
}

int connect(socket_t sock, const struct sockaddr_in* addr) {
    return ::connect(sock, (const SA*)addr,
                     static_cast<socklen_t>(sizeof(*addr)));
}

int connect(socket_t sock, const struct sockaddr_in6* addr) {
    return ::connect(sock, (const SA*)addr,
                     static_cast<socklen_t>(sizeof(*addr)));
}

int listen(socket_t sock, int backlog) { return ::listen(sock, backlog); }

int accept(socket_t sock) {
    return ::accept(sock, NULL, static_cast<socklen_t>(0));
}

int accept(socket_t sock, struct sockaddr_in* addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
    return ::accept(sock, (SA*)addr, &addrlen);
}

int accept(socket_t sock, struct sockaddr_in6* addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
    return ::accept(sock, (SA*)addr, &addrlen);
}

int read(socket_t sock, void* buf, size_t count) {
#ifndef _WIN32
    return ::read(sock, buf, count);
#else
    return ::recv(sock, reinterpret_cast<char*>(buf), static_cast<int>(count),
                  0);
#endif
}

int write(socket_t sock, const void* buf, size_t count) {
#ifndef _WIN32
    return ::write(sock, buf, count);
#else
    return ::send(sock, reinterpret_cast<char*>(const_cast<void*>(buf)),
                  static_cast<int>(count), 0);
#endif
}

int shutdown(socket_t sock, int how) {
    return ::shutdown(sock, how);
}

int readv(socket_t sock, const struct iovec* iov, int cnt) {
    return ::readv(sock, iov, cnt);
}

int writev(socket_t sock, const struct iovec* iov, int cnt) {
    return ::writev(sock, iov, cnt);
}

int setnonblocking(socket_t sock) {
    unsigned long mode = 1;
#ifdef _WIN32
    return ioctlsocket(sock, FIONBIO, &mode);
#else
    mode = fcntl(sock, F_GETFL, 0);
    return fcntl(sock, F_SETFL, mode | O_NONBLOCK);
#endif
}

int setcloseexec(socket_t sock) {
#ifndef _WIN32
    unsigned long mode = fcntl(sock, F_GETFD, 0);
    return fcntl(sock, F_SETFD, mode | FD_CLOEXEC);
#endif
}

#ifdef WIN32
#define SOCKET_ERROR() WSAGetLastError()
#define SET_SOCKET_ERROR(errcode) \
    do {                          \
        WSASetLastError(errcode); \
    } while (0)
#else
#define SOCKET_ERROR() (errno)
#define SET_SOCKET_ERROR(errcode) \
    do {                          \
        errno = (errcode);        \
    } while (0)
#endif

int socketpair(int family, int type, int protocol, socket_t fd[2]) {
#ifndef _WIN32
    return ::socketpair(family, type, protocol, fd);
#else
    socket_t listener = -1;
    socket_t connector = -1;
    socket_t acceptor = -1;
    struct sockaddr_in listen_addr;
    struct sockaddr_in connect_addr;
    int size;

    if (protocol || family != AF_INET) {
        fprintf(stderr, "EAFNOSUPPORT\n");
        return -1;
    }
    if (!fd) {
        fprintf(stderr, "EINVAL\n");
        return -1;
    }

    /*创建listener，监听本地的换回地址，端口由内核分配*/
    listener = create(AF_INET, type, 0);
    if (listener < 0) return -1;

    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listen_addr.sin_port = 0; /* kernel chooses port.    */
    if (bind(listener, &listen_addr) == -1) goto fail;
    if (listen(listener, 1) == -1) goto fail;

    /*创建connector, 连接到listener, 作为Socketpair的一端*/
    connector = create(AF_INET, type, 0);
    if (connector < 0) goto fail;
    /* We want to find out the port number to connect to.  */
    size = sizeof(connect_addr);
    if (getsockname(listener, (SA*)&connect_addr, &size) == -1) goto fail;
    if (size != sizeof(connect_addr)) goto fail;
    if (connect(connector, &connect_addr) == -1) goto fail;

    size = sizeof(listen_addr);
    /*调用accept函数接受connector的连接，将返回的文件描述符作为Socketpair的另一端*/
    acceptor = accept(listener, (SA*)&listen_addr, &size);
    if (acceptor < 0) goto fail;
    if (size != sizeof(listen_addr)) goto fail;
    close(listener);

    /* Now check we are talking to ourself by matching port and host on the
       two sockets.     */
    if (getsockname(connector, (SA*)&connect_addr, &size) == -1) goto fail;
    if (size != sizeof(connect_addr) ||
        listen_addr.sin_family != connect_addr.sin_family ||
        listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr ||
        listen_addr.sin_port != connect_addr.sin_port)
        goto fail;
    fd[0] = connector;
    fd[1] = acceptor;

    return 0;

fail:
    if (listener != -1) close(listener);
    if (connector != -1) close(connector);
    if (acceptor != -1) close(acceptor);

    return -1;
#endif
}

std::string error() {
    char message[128] = {0};
#ifdef _WIN32
    strerror_s(message, sizeof(message), errno);
#else
    int err = errno;
    if (err < sys_nerr)
        snprintf(message, sizeof(message), "%s", sys_errlist[err]);
    else
        snprintf(message, sizeof(message), "Unknown error %d", err);

        // char *res = strerror_r(errno, message, sizeof(message));
        // printf("%s %d %s", message, strlen(message), res);
#endif
    return message;
}

}  // namespace socket
}  // namespace network
