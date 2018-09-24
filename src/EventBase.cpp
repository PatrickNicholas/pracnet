#include <network/EventBase.h>

#include <network/Event.h>
#include <network/EventHandler.h>

namespace network {

EventBase::EventBase() 
    : isTerminal_(false)
    , baseThreadID_(std::this_thread::get_id())
    , selector_(CreateSelector())
    , signalMgr_(*this)
{
    signalMgr_.initialize();
}

EventBase::~EventBase() {
	// TODO(maochuan): release 
}

void EventBase::update(const EventObserver &observer) 
{
    requireRunInLoopThread();
    selector_->update(observer);
}

void EventBase::remove(const EventObserver &observer)
{
    requireRunInLoopThread();
    selector_->remove(observer);
}

void EventBase::loop()
{
    requireRunInLoopThread();

	constexpr size_t kBatch = 32;
	EventDispatchList activeEventList{kBatch};
    TimerDispatchList activeTimers;

    while (true) {
        if (isTerminal_) {
            isTerminal_ = false;
            break;
        }

        // TODO: 校正系统时间
        long timeout = timerQueue_.getMinimumTimeout();

        selector_->dispatch(activeEventList, timeout);
        dispatch(activeEventList);

        timerQueue_.getTimeoutCallbacks(activeTimers);
        dispatch(activeTimers);

        // If there is no timer or listening event, exit the loop.
		if (selector_->empty() && timerQueue_.empty()) {
			break;
		}
    }
}

void EventBase::dispatch(EventDispatchList &actives)
{
    for (auto &ctx : actives) {
		ctx.observer_->dispatch(ctx.event);
    }
}

void EventBase::dispatch(TimerDispatchList &actives)
{
    for (auto &timercb : actives) {
        timercb();
    }
}

int EventBase::runAfter(milliseconds ms, const TimerCallback &cb)
{
    using std::chrono::high_resolution_clock;

    requireRunInLoopThread();
    time_point now = high_resolution_clock::now();
    return timerQueue_.insert(now + ms, cb);
}

int EventBase::runAt(time_point at, const TimerCallback &cb)
{
    requireRunInLoopThread();
    return timerQueue_.insert(at, cb);
}

int EventBase::runEach(milliseconds ms, const TimerCallback &cb)
{
    requireRunInLoopThread();
    return timerQueue_.insert(ms, cb);
}

bool EventBase::removeTimer(int id) 
{
    requireRunInLoopThread();
    return timerQueue_.remove(id);
}

} // namespace network