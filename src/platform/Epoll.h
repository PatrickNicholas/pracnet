#pragma once

#include <unordered_set>
#include <vector>

#include <network/Selector.h>
#include <network/platform/EventID.h>

struct epoll_event;

namespace network {

class Event;
class EventHandler;

class Epoll final : public Selector {
    using EventList = std::vector<epoll_event>;
    using ObserverSet = std::unordered_set<EventID>;

    static const int kInitEventListSize = 16;

    int epollfd_;
    EventList events_;
    ObserverSet observerSet_;

   public:
    Epoll(const Epoll&) = delete;
    Epoll& operator=(const Epoll&) = delete;

    Epoll();
    virtual ~Epoll();

    bool empty() const override;
    void update(const EventObserver&) override;
    void remove(const EventObserver&) override;
    void dispatch(std::vector<EventContext>& actives, long timeout) override;
};

}  // namespace network