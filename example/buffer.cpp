#include <buffer.h>

#include <atomic>

namespace buffer {
constexpr size_t kPageSize = 1024 * 4;
Byte* PagePool::alloc(size_t factor) {
    void* buf = new Byte[kPageSize << factor];
    if (buf != nullptr) {
        *(reinterpret_cast<int32_t*>(buf)) = kPageSize << factor;
        *(reinterpret_cast<int32_t*>(buf) + 1) = 1;
        buf = (reinterpret_cast<int32_t*>(buf) + 2);
    }
    return buf;
}

void PagePool::free(Byte* byte) {

}

void PagePool::ref(Byte* byte) {

}


size_t read_buffer(Connection& conn, Buffer& buffer) {
    constexpr size_t n = 2;
    struct iovec vecs[2];
    // TODO: fill vecs

    std::vector<Byte*> buffers = PagePool::allocn(n - 1);

    size_t size = socket::readv(conn.socket(), vecs, n);
}


}  // namespace buffer
