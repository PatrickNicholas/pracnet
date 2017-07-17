#include <ip/tcp/Connector.h>       // class Connector

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

Connector::Connector() 
    : status_ { status_t::None }
{}

Connector::~Connector() {}

// void Connector::bind(const InetAddress & address) 
// {
//     // TODO:
// }

void Connector::connect(const InetAddress & address) 
{
    if (status_ == Connected) {
        // TODO:
        return;
    }
    
    socket_ = create_socket_with_domain(address.domain());

    int res;
    socket::socket_t fd = socket_.handle();
    res = socket::connect(fd, address.address());
    if (res < 0) {
        throw_exception();
    }
    status_ = status_t::Connected;
}

Socket & Connector::socket() 
{
    // TODO: assert status_ == status_t::Connected
    return this->socket_;
}

} // namespace tcp 
} // namespace ip
} // namespace network