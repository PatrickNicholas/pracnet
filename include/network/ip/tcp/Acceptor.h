#pragma once

#include <network/ip/InetAddress.h>  // InetAddress
#include <network/ip/Socket.h>       // Socket
#include <network/ip/SocketOwner.h>

#include <iostream>

namespace network {
namespace ip {
namespace tcp {

class Acceptor final : public SocketOwner {
    Acceptor(const Acceptor&) = delete;
    Acceptor& operator=(const Acceptor&) = delete;

    enum class status_t {
        None,
        Listening,
    };

    status_t status_ = status_t::None;

   public:
    using SocketOwner::SocketOwner;
    Acceptor(Acceptor&& socket) = default;
    Acceptor& operator=(Acceptor&&) = default;

    void listen(int backlog);
    Socket accept();
};

}  // namespace tcp
}  // namespace ip
}  // namespace network