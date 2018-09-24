#include <network/ip/tcp/Acceptor.h>        // Acceptor
#include <network/ip/helper/socket.h>       // socket::*
#include <errno.h>                  // errno
#include <network/utils/unreachable.h>      // unreachable()

#include "ip/exception.h"

namespace network {
namespace ip {
namespace tcp {

void Acceptor::listen(int backlog) {
	if (status_ == status_t::Listening) {
		throw Exception("Acceptor::listen: call listen on listening socket.");
	}
	if (socket::listen(socket(), backlog) < 0) {
		ThrowException("Acceptor::listen");
	}
}

Socket Acceptor::accept() 
{
    socket::socket_t fd = socket();
    int connfd = socket::accept(fd);
    if (connfd == -1) {
		ThrowException("Acceptor::accept");
    }
    return Socket(connfd);
}

} // namespace tcp
} // namespace ip
} // namespace network