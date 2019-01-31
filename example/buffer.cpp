#include <buffer.h>

#include <assert.h>
#include <atomic>

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

    static Holder* holder(int factor) {
        static Holder holders[10] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        };

        assert(0 <= factor && factor < 10);
        return holders[factor];
    }
};

class PagePool final {
   public:
    static Byte* alloc(size_t factor = 0);
    static void free(Byte* byte);
    static void ref(Byte* bytes);
    static size_t size(Byte* bytes);
};

Byte* PagePool::alloc(size_t factor) {
    page_des* page = PageManager::holder(factor)->alloc();
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
        PageManager::holder(page->factor)->free(page);
    }
}

void PagePool::ref(Byte* byte) {
    page_des* page = reinterpret_cast<page_des*>(byte) - 1;
    atomic_inc(&page->ref);
}

size_t Buffer::ReadSocket(socket::socket_t sock, Buffer& buffer) {
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

size_t Buffer::WriteSocket(socket::socket_t sock, Buffer& bufer) {}

void Buffer::append(const byte_t* data, size_t len) {
    extend(len);

    for () }

void Buffer::extend(size_t len) {
    while (avaliable_ < len) {
        extendPage();
    }
}

void Buffer::extendPage() {
    byte_t* buf = PagePool::alloc(0);
    // TODO: nullptr
    avaliable_ += PagePool::size(buf);
    page_refs_.push_back(buf);
}

size_t Buffer::size() const {
    size_t total = 0;
    for (BufChian* buf = chain_; buf != last_avaliable_buf_; buf = buf->next) {
        total += PagePool::size(reinterpret_cast<byte*>(buf));
    }
    if (last_avaliable_buf_) {
        total += last_avaliable_buf_->index;
    }
    total -= read_index_;
    return total;
}

bool Buffer::empty() const {
    if (last_avaliable_buf_ == nullptr || chain != last_avaliable_buf_ ||
        last_avaliable_buf_->index) {
        return true;
    }
    return false;
}

size_t Buffer::capacity() const {
    size_t cap = 0;
    for (BufChain* buf = last_avaliable_buf_; buf != nullptr; buf = buf->next) {
        cap += PagePool::size(reinterpret_cast<byte_t*>(buf)) - buf->index;
    }
    return cap;
}

bool Buffer::readable() const { return read_index_ != 0u; }

bool Buffer::writeable() const { return capacity() != 0; }

bool Buffer::clear() {
    read_index_ = 0;

    BufChain* buf = chain_;
    while (buf != nullptr) {
        BufChain* next = buf->next;
        PagePool::free(reinterpret_cast<byte_t*>(buf));
        buf = next;
    }
    chain_ = nullptr;
    last_avaliable_buf_ = nullptr;
    last_buf_ = nullptr;
}

void Buffer::append(const byte_t* data, size_t len) {
    extend(len);
    appendInternal(data, len);
}

void Buffer::extend(size_t required) {
    size_t cap = capacity();
    if (cap < required) {
        last_buf_ = alloc();
        if (chain_ == nullptr) {
            chain_ = last_avaliable_buf_ = last_buf_;
        }
        extend(required);
    }
}

BufChain* Buffer::alloc() {
    BufChain* chain = reinterpret_cast<BufChain*>(PagePool::alloc(0));
    chain->index = 1;  // notice skip
    chain->next = nullptr;
    return chain;
}

void Buffer::appendInternal(const byte_t* data, size_t len) {
    if (len == 0) {
        return;
    }

    if (last_avaliable_buf_->isFull()) {
        assert(last_avaliable_buf_->next);
        last_avaliable_buf_ = last_avaliable_buf_->next;
    }

    void* start = last_avaliable_buf_ + last_avaliable_buf_->index;
    size_t cap =
        PagePool::size(reinterpret_cast<byte_t*>())->last_avaliable_buf_->index;
    size_t writeable = std::min(len, cap);
    memcpy(start, data, writeable);
    last_avaliable_buf_ += writeable;
    if (writeable < len) {
        appendInternal(data + writeable, len - writeable);
    }
}
