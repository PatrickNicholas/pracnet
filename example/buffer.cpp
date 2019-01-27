#include <buffer.h>

#include <atomic>

namespace buffer {

struct page_des {
    int32_t factor;
    int32_t ref;
};

constexpr size_t kPageSize = 1024 * 4;

struct Holder final {
   public:
    Holder(const Holder&) = delete;
    Holder& operator=(const Holder&) = delete;

    explicit Holder(size_t factor) : factor_{factor} {}
    ~Holder() {
        while (pages_.size()) {
            page_des* page = pages_.front();
            pages_.pop_front();
            delete[] page;
        }
    }

    page_des* alloc() {
        {
            std::lock_gurad lock(mutex_);
            if (!pages_.empty()) {
                page_des* page = pages_.front();
                pages_.pop_front();
                return page;
            }
        }
        return new Byte[kPageSize << factor_];
    }

    void free(page_des* page) {
        std::lock_gurad lock(mutex_);

        if (pages_.size() >= 5) {
            delete[] page;
        } else {
            pages_.push_back(page);
        }
    }

   private:
    const size_t factor_;
    std::mutex mutex_;
    std::list<page_des> pages_;
};

class PageManager final {
   public:
    PageManager(const PageManager&) = delete;
    PageManager& operator=(const PageManager&) = delete;

    static page_des* alloc(int factor){std::lock_mutex} page_des* free()

        private:
};

class PagePool final {
   public:
    static Byte* alloc(size_t factor = 0);
    static void free(Byte* byte);
    static void ref(Byte* bytes);
    static size_t size(Byte* bytes);
};

Byte* PagePool::alloc(size_t factor) {
    page_des* page = holder(factor)->alloc();
    if (page != nullptr) {
        page->ref = 1;
        return page + 1;
    }
    return nullptr;
}

size_t PagePool::size(Byte* byte) {
    page_des* page = reinterpret_cast<page_des*>(byte) - 1;
    return kPageSize << page->factor;
}

void PagePool::free(Byte* byte) {
    page_des* page = reinterpret_cast<page_des*>(byte) - 1;
    if (!atomic_dec_and_test(&page->ref)) {
        holder(page->factor)->free(page);
    }
}

void PagePool::ref(Byte* byte) {
    page_des* page = reinterpret_cast<page_des*>(byte) - 1;
    atomic_inc(&page->ref);
}

size_t read_buffer(socket::socket_t sock, Buffer& buffer) {
    struct iovec vecs[3];
    buffer.fill(vecs[0]);

    vecs[1].iov_base = PagePool::alloc(0);
    vecs[1].iov_len = PagePool::size(vecs[1].iov_base);
    vecs[2].iov_base = PagePool::alloc(1);
    vecs[2].iov_len = PagePool::size(vecs[2].iov_base);

    size_t size = socket::readv(sock, vecs, 3);
    if (size > vecs[0].iov_len) {
        buffer.page_refs_.push_back(vecs[1].iov_base);

        if (size > vecs[0].iov_len + vecs[1].iov_len) {
            buffer.page_refs_.push_back(vecs[2].iov_base);
        } else {
            PagePool::free(vecs[2].iov_base);
        }
    } else {
        PagePool::free(vecs[1].iov_base);
        PagePool::free(vecs[2].iov_base);
    }

    return size;
}

size_t write_buffer(socket::socket_t sock, Buffer& bufer) {

}

}  // namespace buffer
