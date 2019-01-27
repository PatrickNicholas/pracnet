#pragma once

#include <atomic>

#include <network/ip/helper/socket.h>

#include <network/ip/tcp/Connection.h>

namespace buffer {

typedef char Byte;
typedef Byte* Page;

class PagePool final {
   public:
    static Byte* alloc(size_t factor = 0);
    static void free(Byte* byte);
    static void ref(Byte* bytes);
};

class Buffer final {
    friend size_t read_buffer(Connection& conn, Buffer& buffer);

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
    std::list<char*> page_refs_;
};

size_t read_buffer(Connection& conn, Buffer& buffer);

}  // namespace buffer
