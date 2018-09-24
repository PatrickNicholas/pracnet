#pragma once  

#include <network/ip/Socket.h>      // Socket
#include <network/ip/InetAddress.h> // InetAddress
#include <network/ip/SocketOwner.h>
#include <network/ip/tcp/Connection.h>

namespace network {
namespace ip {
namespace tcp {

class Connector final : public Connection
{
    Connector(const Connector &) = delete;
    Connector &operator = (const Connector &) = delete;

    enum status_t {
        None,
        Connected,
    };

    status_t status_;
public:
	using Connection::Connection;
	Connector(Connector&&) = default;
	Connector& operator=(Connector&&) = default;

    void connect(const InetAddress &);
};

} // namespace tcp
} // namespace ip
} // namespace network