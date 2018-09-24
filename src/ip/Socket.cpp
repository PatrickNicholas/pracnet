#include <network/ip/Socket.h>

#include <cassert>          // assert

namespace network {
namespace ip {
namespace {

void close_socket(socket::socket_t fd)
{
    if (fd != INVALID_SOCKET) {
        int res = socket::close(fd);
        assert(res == 0 && "some logic error arose.");
    }
}

} // anonymous namespace

Socket::Socket() 
    : handle_{ INVALID_SOCKET } 
    {}

Socket::Socket(socket::socket_t fd)
    : handle_{ fd }
    {}

Socket::~Socket() 
{
    close_socket(handle_);
}

Socket Socket::create(int domain, int type, int protocol) 
{
	return Socket{ socket::create(domain, type, protocol) };
}

int Socket::setcloseexec() {
	return socket::setcloseexec(handle_);
}

int Socket::setnonblocking() {
	return socket::setnonblocking(handle_);
}

} // namespace ip
} // namespace network