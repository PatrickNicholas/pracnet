#pragma once 

#include <ip/Socket.h>      // Socket
#include <ip/InetAddress.h> // InetAddress

namespace network {
namespace ip {
namespace tcp {

class Acceptor 
{
    enum class status_t {
        None,
        Listening,
    };

    InetAddress address_;
    Socket socket_;
    status_t status_;

public:
    Acceptor(const Acceptor &) = delete;
    Acceptor & operator = (const Acceptor &) = delete;

    Acceptor();
    ~Acceptor();

    void bind(const InetAddress & address);
    void listen(const InetAddress &address);
    void listen();

    Socket accept();

    Socket & socket();
};

} // namespace tcp
} // namespace ip
} // namespace network