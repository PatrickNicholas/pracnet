#pragma once

#include <variant>

namespace network {
namespace ip {
namespace tcp {

typedef std::variant<Acceptor, int> AcceptorResult;

enum Option {

};

class AcceptorBuilder final {
   public:
    AcceptorBuilder& backlog(size_t n) { backlog_ = n; }

    AcceptorBuilder& bind(const InetAddress& address) { address_ = &address; }

    AcceptorBuilder& option();
    AcceptorBuilder& childOption();
    AcceptorResult build() {
        if (address_) {
            throw "empty address";
        }

        Socket socket = Socket::create(AF_INET, SOCK_STREAM, 0);
    }

   private:
    size_t backlog_ = 10;
    const InetAddress* address_;
};

}  // namespace tcp
}  // namespace ip
}  // namespace network
