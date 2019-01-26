#pragma once

#include <list>    // std::list
#include <memory>  // std::shared_ptr

extern "C" {
#include <sys/socket.h>  // struct sockaddr
}

namespace network {
namespace ip {

enum class domain_t {
    IPV4,
    IPV6,
};

class InetAddress {
   public:
    class InetAddressImpl;
    typedef std::shared_ptr<InetAddressImpl> pointer;

    explicit InetAddress() : impl_(nullptr) {}
    explicit InetAddress(pointer p) : impl_(p) {}

    static InetAddress parseV4(const char* ip, int port);
    static InetAddress parseV6(const char* ip, int port);
    static std::list<InetAddress> query(const char* host);

    size_t length() const;
    const sockaddr* address() const;
    domain_t domain() const;

   private:
    pointer impl_;
};

}  // namespace ip
}  // namespace network
