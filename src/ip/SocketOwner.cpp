#include <network/ip/SocketOwner.h>

#include <network/exception/exception.h>
#include <network/ip/helper/socket.h>

#include "ip/exception.h"

namespace network {
namespace ip {

void SocketOwner::bind(const InetAddress& addr) {
    using socket::socket_t;

    if (!valid()) {
        throw Exception{"call Socket::bind on invalid socket"};
    }

    socket_t sock = socket();
    int res = socket::bind(sock, addr.address(), addr.length());
    if (res < 0) {
        ThrowException("socket::bind");
    }
}

}  // namespace ip
}  // namespace network