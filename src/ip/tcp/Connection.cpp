#include <network/ip/tcp/Connection.h>

#include <network/ip/helper/socket.h>

#include "ip/exception.h"

namespace network {
namespace ip {
namespace tcp {

bool Connection::eof() const {
    // If count is zero, read() may detect the errors
    // described below.  In the absence of any errors,
    // or if read() does not check for errors, a read()
    // with a count of 0 returns zero and has no other
    // effects.
    return socket::read(socket(), NULL, 0) == 0;
}

bool Connection::again() const {
    // FIXME(maochuan)
    //
    // If count is zero and fd refers to a regular file,
    // then write() may return a failure status if one of
    // the errors below is detected.  If no errors are
    // detected, or error detection is not performed, 0
    // will be returned without causing any other effect.
    // If count is zero and fd refers to a file other than
    // a regular file, the results are not specified.
    return errno == EAGAIN || errno == EWOULDBLOCK;
}

size_t Connection::read(void* buf, size_t len) {
    using socket::read;
    using socket::socket_t;

    socket_t sock = socket();

    ssize_t count = 0;
    do {
        count = read(sock, buf, len);
    } while (count < 0 && errno == EINTR);

    if (count >= 0) {
        return count;
    } else if (again()) {
        return 0;
    } else {
        ThrowException("Connection::read");
    }
}

size_t Connection::write(const void* buf, size_t len) {
    using socket::socket_t;
    using socket::write;

    socket_t sock = socket();
    ssize_t count;
    do {
        count = write(sock, buf, len);
    } while (count < 0 && errno == EINTR);

    if (count >= 0) {
        return count;
    } else if (again()) {
        return 0;
    } else {
        ThrowException("Connection::write");
    }
}

}  // namespace tcp
}  // namespace ip
}  // namespace network
