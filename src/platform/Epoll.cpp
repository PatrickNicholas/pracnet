#include "platform/Epoll.h"

#include <errno.h>
#include <error.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <network/Event.h>
#include <network/EventHandler.h>
#include <network/exception/exception.h>

namespace network {

namespace {

Event FillEvent(uint32_t flags) {
    Event event;
    if (flags & EPOLLIN) {
        event.add(Event::kRead);
    } else if (flags & EPOLLOUT) {
        event.add(Event::kWrite);
    } else if (flags & EPOLLERR) {
        event.add(Event::kError);
    }
    return event;
}

void ThrowErrnoException(const std::string& what) {
    char message[128] = {0};
    strerror_r(errno, message, sizeof(message));
    throw Exception(what + ": " + message);
}

void close_no_eintr(int fd) {
    int res;
    do {
        res = close(fd);
    } while (res == EINTR);
}

}  // anonymous namespace

Epoll::Epoll() : events_(kInitEventListSize) {
    // since linux 2.6.8, size is ignored, but must be greater than zero.
    epollfd_ = epoll_create(10);
    if (epollfd_ == -1) {
        ThrowErrnoException("epoll_create");
    }
}

Epoll::~Epoll() {
    if (epollfd_ != -1) {
        close_no_eintr(epollfd_);
    }
}

bool Epoll::empty() const { return observerSet_.empty(); }

void Epoll::update(const EventObserver& observer) {
    assert(epollfd_ > 0 && "call insert on error instance.");

    uint32_t events = EPOLLERR;
    events |= observer.isReading() ? EPOLLIN : 0;
    events |= observer.isWriting() ? EPOLLOUT : 0;

    EventID id = observer.id();
    epoll_event epoll;
    epoll.data.ptr = const_cast<EventObserver*>(&observer);
    epoll.events = events;

    auto it = observerSet_.find(id);
    if (it == observerSet_.end()) {
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, id, &epoll) == -1) {
            ThrowErrnoException("epoll_ctl");
        }
        observerSet_.insert(id);
    } else {
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, id, &epoll) == -1) {
            ThrowErrnoException("epoll_ctl");
        }
    }
}

void Epoll::remove(const EventObserver& observer) {
    assert(epollfd_ > 0 && "call remove on error instance.");

    EventID id = observer.id();
    if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, id, NULL) == -1) {
        ThrowErrnoException("epoll_ctl");
    }
    observerSet_.erase(id);
}

void Epoll::dispatch(std::vector<EventContext>& actives, long timeout) {
    assert(epollfd_ > 0 && "call dispatch on error instance.");

    actives.clear();

    int nfds = epoll_wait(epollfd_, events_.data(),
                          static_cast<int>(events_.size()), timeout);
    if (nfds > 0) {
        for (int i = 0; i < nfds; ++i) {
            EventObserver* observer =
                reinterpret_cast<EventObserver*>(events_[i].data.ptr);
            Event event = FillEvent(events_[i].events);
            actives.push_back({event, observer});
        }
        if (events_.size() == static_cast<size_t>(nfds)) {
            events_.reserve(events_.size() * 2);
        }
    } else if (nfds == 0 || errno == EINTR) {
        // timeout or interrupt, do nothings
    } else {
        ThrowErrnoException("epoll_wait");
    }
}

}  // namespace network