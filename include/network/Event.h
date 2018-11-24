#pragma once

#include <cassert>

#include <network/platform/EventID.h>

namespace network {

// copyable
class Event {
    unsigned status_;

   public:
    enum {
        kEmpty = 0x0,
        kError = 0x1,
        kRead = 0x2,
        kWrite = 0x4,
    };

    Event() : status_{kEmpty} {}
    Event(const Event&) = default;
    Event& operator=(const Event&) = default;
    ~Event() = default;

    void clear() { status_ = kEmpty; }

    void add(unsigned state) {
        assert(status_ >= kError && status_ <= kWrite &&
               "out of status ranges.");
        status_ |= state;
    }

    bool error() const { return status_ & kError; }

    bool write() const { return status_ & kWrite; }

    bool read() const { return status_ & kRead; }
};

}  // namespace network