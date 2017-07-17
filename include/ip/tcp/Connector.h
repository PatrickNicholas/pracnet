#pragma once  

#include <ip/Socket.h>      // Socket
#include <ip/InetAddress.h> // InetAddress

namespace network {
namespace ip {
namespace tcp {

class Connector 
{
    Connector(const Connector &) = delete;
    Connector &operator = (const Connector &) = delete;

    enum status_t {
        None,
        Connected,
    };

    status_t status_;
    Socket socket_;
    InetAddress address_;

public:
    Connector();
    ~Connector();

    // void bind(const InetAddresss &);
    void connect(const InetAddress &);

    Socket & socket();
}

} // namespace tcp
} // namespace ip
} // namespace network