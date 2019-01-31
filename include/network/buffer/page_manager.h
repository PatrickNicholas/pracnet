#pragma once

namespace network {
namespace buffer {

typedef char Byte;

class PagePool final {
   public:
    // alloc
    static Byte* alloc(size_t factor = 0);

    // release page ref.
    static void free(Byte* byte);

    // add page ref.
    static void ref(Byte* bytes);

    // return page avaliable space size.
    static size_t size(const Byte* bytes);
};

}  // namespace buffer
}  // namespace network
