#pragma once

namespace network {
namespace ip {
namespace tcp {

class AbstractBuilder {
   protected:
    void decorate(socket::socket_t sock);
};

}  // namespace tcp
}  // namespace ip
}  // namespace network
