#pragma once

#include <atomic>

#include <network/ip/helper/socket.h>

#include <network/ip/tcp/Connection.h>

namespace buffer {

typedef char Byte;
typedef Byte* Page;

class Buffer final {
    friend size_t read_buffer(socket::socket_t sock, Buffer& buffer);
    friend size_t write_buffer(socket::socket_t sock, Buffer& buffer);

   public:
    BufferRef bytes(size_t length);
    size_t readableBytes() const;
    size_t writeableBytes() const;
    bool readable() const;
    bool writeable() const;
    void clear();

    size_t left() const;

    Buffer slice(size_t from, size_t to);

   private:
    void readIOVec(struct iovec& vec);
    void writeIOVec();

    size_t read_index_ = 0;
    size_t write_index_ = 0;
    std::list<char*> page_refs_;
};

size_t read_buffer(socket::socket_t sock, Buffer& buffer);
size_t write_buffer(socket::socket_t sock, Buffer& buffer);

}  // namespace buffer
