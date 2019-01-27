#include <string.h>  // memset, bzero

#include <network/ip/InetAddress.h>  // class InetAddress

#include "ip/exception.h"

extern "C" {
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>  // getaddrinfo
#else
#include <arpa/inet.h>  // inet_pton
#include <netdb.h>      // getaddrinfo
#include <sys/socket.h>
#endif
}

namespace network {
namespace ip {

class InetAddress::InetAddressImpl {
    InetAddressImpl(const InetAddressImpl&) = delete;
    InetAddressImpl& operator=(const InetAddressImpl&) = delete;

    domain_t domain_;

   public:
    InetAddressImpl(domain_t domain) : domain_(domain) {}

    domain_t domain() const { return domain_; }

    virtual size_t length() const = 0;
    virtual const sockaddr* address() const = 0;
    virtual void print(std::ostream& out) const = 0;
};

class InetAddressV4 : public InetAddress::InetAddressImpl {
    InetAddressV4(const InetAddressV4&) = delete;
    InetAddressV4& operator=(const InetAddressV4&) = delete;

    sockaddr_in addr_;

   public:
    explicit InetAddressV4(const sockaddr_in& v4addr)
        : InetAddress::InetAddressImpl(domain_t::IPV4), addr_(v4addr) {}

    virtual size_t length() const override final { return sizeof(addr_); }

    virtual const sockaddr* address() const override final {
        return reinterpret_cast<sockaddr*>(const_cast<sockaddr_in*>(&addr_));
    }

    virtual void print(std::ostream& out) const {
        // TODO(maochuan)
    }
};

class InetAddressV6 : public InetAddress::InetAddressImpl {
    InetAddressV6(const InetAddressV6&) = delete;
    InetAddressV6& operator=(const InetAddressV6&) = delete;

    sockaddr_in6 addr_;

   public:
    explicit InetAddressV6(const sockaddr_in6& v6addr)
        : InetAddress::InetAddressImpl(domain_t::IPV6), addr_(v6addr) {}

    virtual size_t length() const override final { return sizeof(addr_); }

    virtual const sockaddr* address() const override final {
        return reinterpret_cast<sockaddr*>(const_cast<sockaddr_in6*>(&addr_));
    }

    virtual void print(std::ostream& out) const {
        // TODO(maochuan)
    }
};

InetAddress InetAddress::parseV4(const char* ip, int port) {
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int res = inet_pton(AF_INET, ip, &addr.sin_addr);
    if (res == 1) {
        return InetAddress(std::make_shared<InetAddressV4>(addr));
    } else if (res == 0) {
        throw Exception("invalid ip v4 network address");
    } else {
        throw Exception("AF not support");
    }
}

InetAddress InetAddress::parseV6(const char* ip, int port) {
    sockaddr_in6 v6;
    memset(&v6, 0, sizeof(v6));
    v6.sin6_family = AF_INET6;
    v6.sin6_port = htons(port);
    int res = inet_pton(AF_INET6, ip, &v6);
    if (res == 1) {
        return InetAddress(std::make_shared<InetAddressV6>(v6));
    } else if (res == 0) {
        throw Exception("invalid ip v6 network address");
    } else {
        throw Exception("AF not support");
    }
}

size_t InetAddress::length() const {
    if (!impl_) {
        throw std::invalid_argument("empty InetAddress");
    }
    return impl_->length();
}

const sockaddr* InetAddress::address() const {
    if (!impl_) {
        throw std::invalid_argument("empty InetAddress");
    }
    return impl_->address();
}

domain_t InetAddress::domain() const {
    if (!impl_) {
        throw std::invalid_argument("empty InetAddress");
    }
    return impl_->domain();
}

std::list<InetAddress> InetAddress::query(const char* host) {
    struct addrinfo *answer, hint, *curr;
    const struct sockaddr_in* sockaddr_ipv4;
    const struct sockaddr_in6* sockaddr_ipv6;
    std::list<InetAddress> query_result;

    bzero(&hint, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;

    int ret = getaddrinfo(host, NULL, &hint, &answer);
    if (ret != 0) {
        // TODO: error
        return query_result;
    }

    for (curr = answer; curr != NULL; curr = curr->ai_next) {
        switch (curr->ai_family) {
            case AF_UNSPEC:
                // just ignore it.
                break;
            case AF_INET: {
                sockaddr_ipv4 =
                    reinterpret_cast<struct sockaddr_in*>(curr->ai_addr);
                std::shared_ptr<InetAddress::InetAddressImpl> pointer =
                    std::make_shared<InetAddressV4>(*sockaddr_ipv4);
                query_result.emplace_back(pointer);
                break;
            }
            case AF_INET6: {
                sockaddr_ipv6 =
                    reinterpret_cast<struct sockaddr_in6*>(curr->ai_addr);
                std::shared_ptr<InetAddress::InetAddressImpl> pointer =
                    std::make_shared<InetAddressV6>(*sockaddr_ipv6);
                query_result.emplace_back(pointer);
                break;
            }
        }
    }
    freeaddrinfo(answer);
    return query_result;
}

void InetAddress::print(std::ostream& out) const {
    pImpl_->print(out);
}

}  // namespace ip
}  // namespace network
