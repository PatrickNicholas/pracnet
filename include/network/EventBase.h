#pragma once 

#include <chrono>
#include <functional>
#include <map>
#include <thread>

#include <network/EventObserver.h>
#include <network/TimerQueue.h> // TimerQueue
#include <network/Selector.h>   // Selector
#include <network/SignalManager.h>

namespace network {

/**
 * Reactor
 */
class EventBase final
{
    using milliseconds = std::chrono::milliseconds;
    using time_point = std::chrono::high_resolution_clock::time_point;

    bool isTerminal_;
    std::thread::id baseThreadID_;
    std::shared_ptr<Selector> selector_;
	//SignalManager signalMgr_;
    TimerQueue timerQueue_;
    
public:
    using EventDispatchList = std::vector<EventContext>;
    using TimerDispatchList = std::vector<TimerCallback>;

    EventBase(const EventBase &) = delete;
    EventBase &operator = (const EventBase &) = delete;

    EventBase();
	~EventBase();

    void update(const EventObserver &observer);
    void remove(const EventObserver &observer);

    void loop();

    void terminal() {
        requireRunInLoopThread();
        isTerminal_ = true;
    }

    void requireRunInLoopThread() const {
        assert(baseThreadID_ == std::this_thread::get_id()
            && "base thread required.");
    }

    int runAfter(milliseconds ms, const TimerCallback& cb);
    int runAt(time_point at, const TimerCallback& cb);
    int runEach(milliseconds ms, const TimerCallback& cb);
    bool removeTimer(int id);
    
private:
    void dispatch(EventDispatchList &actives);
    void dispatch(TimerDispatchList &actives);
};

} // namespace 