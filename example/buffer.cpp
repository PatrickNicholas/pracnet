#include <buffer.h>

#include <assert.h>
#include <atomic>
size_t Buffer::ReadSocket(socket::socket_t sock, Buffer& buffer) {
    struct iovec vecs[3];
    extend(1);

    vecs[0].iov_base = reinterpret_cast<void*>(last_avaliable_buf_ + 1) +
                       last_avaliable_buf_->index;
    vecs[0].iov_len =
        PagePool::size(last_avaliable_buf_) - last_avaliable_buf_->index;
    vecs[1].iov_base = PagePool::alloc(0);
    vecs[1].iov_len = PagePool::size(vecs[1].iov_base);
    vecs[2].iov_base = PagePool::alloc(1);
    vecs[2].iov_len = PagePool::size(vecs[2].iov_base);

    size_t size = socket::readv(sock, vecs, 3);
    if (size > vecs[0].iov_len) {
        last_avaliable_buf_->index = PagePool::size(last_avaliable_buf_);
        last_avaliable_buf_->next =
            reinterpret_cast<BufChain*>(vecs[1].iov_base);
        last_avaliable_buf_ = last_avaliable_buf_->next;

        if (size > vecs[0].iov_len + vecs[1].iov_len) {
            last_avaliable_buf_->index = PagePool::size(last_avaliable_buf_);
            last_avaliable_buf_->next =
                reinterpret_cast<BufChain*>(vecs[2].iov_base);
            last_avaliable_buf_ = last_avaliable_buf_->next;
        } else {
            last_avaliable_buf_->index = size - vecs[0].iov_len;
            PagePool::free(vecs[2].iov_base);
        }
    } else {
        last_avaliable_buf_->index += size;
        PagePool::free(vecs[1].iov_base);
        PagePool::free(vecs[2].iov_base);
    }

    return size;
}

size_t Buffer::WriteSocket(socket::socket_t sock, Buffer& bufer) {
    struct iovec vecs[128];
    size_t vec_len = 0;
    for (BufChain* buf = chain_; buf != nullptr; buf = buf->next, vec_len++) {
        vecs[vec_len].iov_base = buf + 1;
        vecs[vec_len].iov_len = buf->index;
    }
    if (chain_ != nullptr) {
        vecs[0].iov_base = reinterpret_cast<void*>(buf + 1) + read_index_;
        vecs[0].iov_len = buf->index - read_index_;
    }

    if (vec_len == 0) {
        return;
    }

    size_t writen = socket::writev(sock, vecs, vec_len);
    size_t copy = writen;
    for (BufChain* buf = chain_; copy > 0 && buf != nullptr;) {
        copy -= buf->index - read_index_;
        chain_ = chain_->next;
        PagePool::free(buf);
        buf = chain_;
        read_index_ = 0;
    }
    return writen;
}

void Buffer::append(const byte_t* data, size_t len) {
    extend(len);
    appendInternal(data, len);
}

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

bool Buffer::read(byte_t* data, size_t len) {
    if (size() < len) {
        return false;
    }
    readInternal(data, len);
    return true;
}

void Buffer::readInternal(byte* data, size_t len) {
    if (len == 0) {
        return;
    }

    size_t size = PagePool::size(chain_) - read_index_;
    if (chain_ == last_avaliable_buf_) {
        size = last_avaliable_buf_->index - read_index_;
    }

    size_t readn = std::min(len, size);
    memcpy(data, reinterpret_cast<void*>(chain_ + 1) + read_index_, readn);
    if (size < len) {
        assert(last_avaliable_buf_ != chain_);
        read_index_ = 0;
        BufChain* buf = chain_;
        chain_ = chain_->next;
        PagePool::free(buf);
        readInternal(data + readn, len - readn);
    }
}
