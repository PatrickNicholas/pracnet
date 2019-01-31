#pragma once

#include <atomic>

#include <network/ip/helper/socket.h>

#include <network/ip/tcp/Connection.h>

namespace buffer {

typedef char Byte;
typedef Byte* Page;

class Buffer final {
   public:
    static size_t ReadSocket(socket::socket_t sock, Buffer& buffer);
    static size_t WriteSocket(socket::socket_t sock, Buffer& buffer);

    struct BufChain {
        uint32_t index;
        uint32_t reserved;
        BufChain *next;
    };

    Buffer slice(size_t from, size_t to);

    void append(const byte_t* data, size_t len);
    bool read(byte_t** data, size_t len);

    size_t capacity() const;
    size_t size() const;
    bool empty() const;
    bool readable() const;
    bool writeable() const;

    void clear();

   private:
    void drain();

    BufChain* alloc();
    void appendInternal(const byte_t* data, size_t len);
    void extend(size_t required);

    uint32_t read_index_ = 0;

    BufChain *chain_;
    BufChain *last_avaliable_buf_;
    BufChain *last_buf_;
};

size_t read_buffer(socket::socket_t sock, Buffer& buffer);
size_t write_buffer(socket::socket_t sock, Buffer& buffer);

}  // namespace buffer
