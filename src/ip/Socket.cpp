#include <ip/Socket.h>

#include <cassert>          // assert

namespace network {
namespace ip {
namespace {

void close_socket(socket::socket_t fd)
{
    if (fd >= 0) {
        int res = socket::close(fd);
        assert(res == 0 && "some logic error arose.");
    }
}

} // anonymous namespace

Socket::Socket() 
    : handle_{ -1 } 
    {}

Socket::Socket(socket::socket_t fd)
    : handle_{ fd }
    {}

Socket::~Socket() 
{
    close_socket(handle_);
}

} // namespace ip
} // namespace network