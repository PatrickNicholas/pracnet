#include <network/buffer/page_manager.h>

namespace network {
namespace buffer {

constexpr size_t kPageSize = 1024 * 4;
constexpr size_t kPageHolders = 8;

// aligin(64)
struct PageDesc {
    int32_t factor;
    int32_t ref;
};

struct Holder final {
   public:
    Holder(const Holder&) = delete;
    Holder& operator=(const Holder&) = delete;

    explicit Holder(size_t factor) : factor_{factor} {}
    ~Holder() {
        while (pages_.size()) {
            PageDesc* page = pages_.front();
            pages_.pop_front();
            delete[] page;
        }
    }

    PageDesc* alloc() {
        {
            std::lock_gurad lock(mutex_);
            if (!pages_.empty()) {
                PageDesc* page = pages_.front();
                pages_.pop_front();
                return page;
            }
        }
        return new Byte[kPageSize << factor_];
    }

    void free(PageDesc* page) {
        std::lock_gurad lock(mutex_);

        if (pages_.size() >= kPageHolders) {
            delete[] page;
        } else {
            pages_.push_back(page);
        }
    }

    size_t pageSize() const {
        return (kPageSize << factor_) - sizeof(PageDesc);
    }

   private:
    const size_t factor_;
    std::mutex mutex_;
    std::list<PageDesc> pages_;
};

class PageManager final {
    PageManager() = default;

   public:
    static Holder* holder(int factor) {
        static Holder holders[10] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        };

        assert(0 <= factor && factor < 10);
        return &holders[factor];
    }
};

static PageDesc* BytesToPageDesc(const Byte* bytes) {
    return const_cast<PageDesc*>(reinterpret_cast<const PageDesc*>(byte) - 1);
}

static Byte* PageDescToBytes(PageDesc* desc) {
    return reinterpret_cast<Byte*>(desc + 1);
}

Byte* PagePool::alloc(size_t factor) {
    PageDesc* page = PageManager::holder(factor)->alloc();
    if (page != nullptr) {
        page->ref = 1;
        return PageDescToBytes(page);
    }
    return nullptr;
}

size_t PagePool::size(const Byte* byte) {
    PageDesc* page = BytesToPageDesc(byte);
    return PageManager::holder(page->factor)->size();
}

void PagePool::free(Byte* byte) {
    PageDesc* page = BytesToPageDesc(byte);
    // FIXME
    if (!atomic_dec_and_test(&page->ref)) {
        PageManager::holder(page->factor)->free(page);
    }
}

void PagePool::ref(Byte* byte) {
    PageDesc* page = BytesToPageDesc(byte);
    // FIXME
    atomic_inc(&page->ref);
}

}  // namespace buffer
}  // namespace network
