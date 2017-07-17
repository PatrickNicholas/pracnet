#include <ip/tcp/Acceptor.h>        // Acceptor
#include <ip/helper/socket.h>       // socket::*
#include <errno.h>                  // errno
#include <utils/unreachable.h>      // unreachable()

namespace network {
namespace ip {
namespace tcp {

namespace {

socket::socket_t create_ipv4()
{
    return socket::create(AF_INET, SOCK_STREAM, 0);
}

socket::socket_t create_ipv6()
{
    return socket::create(AF_INET6, SOCK_STREAM, 0);
}

socket::socket_t create(domain_t domain) 
{
    switch (domain) {
    case domain_t::IPV4:
        return create_ipv4();
    case domain_t::IPV6:
        return create_ipv6();
    default:
        unreachable();
        return -1;
    }
}

void throw_exception() 
{
    // TODO: throw exception by errno.
}

Socket create_socket_with_domain(domain_t domain) 
{
    return Socket(create(domain));
}

} // anonymous namepsace

Acceptor::Acceptor() 
    : status_ { status_t::None }
{

}

Acceptor::~Acceptor()
{

}

void Acceptor::bind(const InetAddress & address)
{
    address_ = address;
}

void Acceptor::listen(const InetAddress &address)
{
    bind(address);
    listen();
}

void Acceptor::listen()
{
    socket_ = create_socket_with_domain(address_.domain());

    int res;
    socket::socket_t fd = socket_.handle();
    res = socket::bind(fd, address_.address());
    if (res < 0) {
        throw_exception();
        return;
    }
    res = socket::listen(fd, 1024);
    if (res < 0) {
        throw_exception();
    }
}

Socket Acceptor::accept() 
{
    socket::socket_t fd = socket_.handle();
    int connfd = socket::accept(fd);
    if (connfd == -1) {
        throw_exception();
        return Socket();
    }
    return Socket(connfd);
}

Socket & Acceptor::socket()
{
    return socket_;
}

} // namespace tcp
} // namespace ip
} // namespace network