#include <network/ip/tcp/Connector.h>  // class Connector

#include <network/utils/unreachable.h>

#include "ip/exception.h"

namespace network {
namespace ip {
namespace tcp {

void Connector::connect(const InetAddress& address) {
    if (status_ == Connected) {
        throw Exception("Connector::connect call on connected socket.");
    }
    int res;
    socket::socket_t fd = socket();
    res = socket::connect(fd, address.address(), address.length());
    if (res < 0) {
        ThrowException("Connector::connect");
    }
    status_ = status_t::Connected;
}

}  // namespace tcp
}  // namespace ip
}  // namespace network