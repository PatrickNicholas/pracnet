#pragma once

#include <network/ip/SocketOwner.h>

namespace network {
namespace ip {
namespace tcp {

class Connection : public SocketOwner {
    Connection(Connection&) = delete;
    Connection& operator=(Connection&) = delete;

   public:
    enum {
        kSuccess = 0x0,
        kEOF = 0x1,
        kWouldBlock = 0x2,
    };
    using SocketOwner::SocketOwner;
    Connection(Connection&&) = default;
    Connection& operator=(Connection&&) = default;

    bool eof() const;
    bool again() const;

    size_t read(void* buf, size_t len);
    size_t write(const void* buf, size_t len);
};

}  // namespace tcp
}  // namespace ip
}  // namespace network