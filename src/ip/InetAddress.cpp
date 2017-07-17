#include <cstring>              // memset, bzero
#include <ip/InetAddress.h>     // class InetAddress
#include <stdexcept>            // std::invalid_argument

extern "C" {
#include <arpa/inet.h>          // inet_pton

#ifdef _WIN32
#   include <winsock2.h>
#   include <ws2tcpip.h>        // getaddrinfo
#else
#   include <sys/socket.h>
#   include <netdb.h>           // getaddrinfo
#endif
}

namespace network {
namespace ip {
namespace {

} // anonymous namespace 

class InetAddress::InetAddressImpl 
{
    InetAddressImpl(const InetAddressImpl &) = delete;
    InetAddressImpl & operator = (const InetAddressImpl &) = delete;

    domain_t domain_;
public:
    InetAddressImpl(domain_t domain) : domain_(domain) {}

    domain_t domain() const { return domain_; }

    virtual size_t length() const = 0;
    virtual const sockaddr *address() const = 0;
};

class InetAddressV4 : public InetAddress::InetAddressImpl 
{
    InetAddressV4(const InetAddressV4 &) = delete;
    InetAddressV4 & operator = (const InetAddressV4 &) = delete;

    sockaddr_in addr_;

public:
    explicit InetAddressV4(const sockaddr_in &v4addr) 
        : InetAddress::InetAddressImpl(domain_t::IPV4)
        , addr_(v4addr) 
        {}

    virtual size_t length() const override final {
        return sizeof(addr_);
    }

    virtual const sockaddr *address() const override final {
        return reinterpret_cast<sockaddr*>(const_cast<sockaddr_in*>(&addr_));
    }

};

class InetAddressV6 : public InetAddress::InetAddressImpl 
{
    InetAddressV6(const InetAddressV6 &) = delete;
    InetAddressV6 & operator = (const InetAddressV6 &) = delete;

    sockaddr_in6 addr_;

public:
    explicit InetAddressV6(const sockaddr_in6 &v6addr) 
        : InetAddress::InetAddressImpl(domain_t::IPV6)
        , addr_(v6addr) 
        {}

    virtual size_t length() const override final {
        return sizeof(addr_);
    }

    virtual const sockaddr *address() const override final {
        return reinterpret_cast<sockaddr*>(const_cast<sockaddr_in6*>(&addr_));
    }

};

InetAddress InetAddress::parseV4(const char *ip) 
{
    sockaddr_in v4addr;
    memset(&v4addr, 0, sizeof(v4addr));
    int res = inet_pton(AF_INET, ip, &v4addr);
    if (res == 1) {
        return InetAddress(std::make_shared<InetAddressV4>(v4addr));
    }
    else if (res == 0) {
        throw std::invalid_argument("not valid ip v4 address.");
    } else {
        throw std::invalid_argument("TODO:");
    }
}

InetAddress InetAddress::parseV6(const char *ip) 
{
    sockaddr_in6 v6;
    memset(&v6, 0, sizeof(v6));
    int res = inet_pton(AF_INET6, ip, &v6);
    if (res == 1) {
        return InetAddress(std::make_shared<InetAddressV6>(v6));
    }
    else if (res == 0) {
        throw std::invalid_argument("not valid ip v6 address.");
    } else {
        throw std::invalid_argument("TODO:");
    }
}

size_t InetAddress::length() const
{
    if (!pImpl_) {
        throw std::invalid_argument("empty InetAddress");
    }
    return pImpl_->length();
}

const sockaddr * InetAddress::address() const
{
    if (!pImpl_) {
        throw std::invalid_argument("empty InetAddress");
    }
    return pImpl_->address();
}

domain_t InetAddress::domain() const
{
    if (!pImpl_) {
        throw std::invalid_argument("empty InetAddress");
    }
    return pImpl_->domain();
}

std::list<InetAddress> InetAddress::query(const char *host) 
{
    struct addrinfo *answer, hint, *curr; 
    const struct sockaddr_in  *sockaddr_ipv4; 
    const struct sockaddr_in6 *sockaddr_ipv6; 
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
        switch (curr->ai_family){ 
            case AF_UNSPEC: 
                // just ignore it.
                break; 
            case AF_INET: {
                sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in *>(curr->ai_addr); 
                std::shared_ptr<InetAddress::InetAddressImpl> pointer
                    = std::make_shared<InetAddressV4>(*sockaddr_ipv4);
                query_result.emplace_back(pointer);
                break; 
            }
            case AF_INET6: {
                sockaddr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(curr->ai_addr); 
                std::shared_ptr<InetAddress::InetAddressImpl> pointer 
                    = std::make_shared<InetAddressV6>(*sockaddr_ipv6);
                query_result.emplace_back(pointer);
                break; 
            }
        } 
    } 	    
    freeaddrinfo(answer); 
    return query_result;
}

} // namespace ip
} // namespace network