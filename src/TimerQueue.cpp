#include <network/TimerQueue.h>

#include <cassert>

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace network {

const int Timer::minTimeSlice;

namespace {

int lastTimerID() {
    static int id = 0;
    return id++;
}

}  // anonymous namespace

int TimerQueue::insert(milliseconds after, TimerCallback callback) {
    int id = lastTimerID();
    Timer timer{after, callback};
    timers_.insert(std::make_pair(id, timer));
    queue_.push(std::make_pair(timer.timeout(), id));
    return id;
}

int TimerQueue::insert(time_point timeout, TimerCallback callback) {
    int id = lastTimerID();
    Timer timer{timeout, callback};
    timers_.insert(std::make_pair(id, timer));
    queue_.push(std::make_pair(timeout, id));
    return id;
}

bool TimerQueue::remove(int id) {
    auto timer = timers_.find(id);
    if (timer == timers_.end()) return false;

    // NOTICE: queue still records timer's id.
    timers_.erase(timer);
    return true;
}

long TimerQueue::getMinimumTimeout() const {
    // the resolver can block until the IO event occurs;
    if (queue_.empty()) return -1;

    auto timeout = queue_.top().first;
    auto now = high_resolution_clock::now();

    // needs to be returned directly
    if (now >= timeout) return 0;

    auto duration = timeout - now;
    return duration.count();
}

void TimerQueue::getTimeoutCallbacks(std::vector<TimerCallback>& callbacks) {
    callbacks.clear();

    auto now = high_resolution_clock::now();

    // NOTICE: Some of the timer may have been deleted
    while (!queue_.empty()) {
        auto pair = queue_.top();
        if (pair.first > now) break;

        queue_.pop();

        auto timer = timers_.find(pair.second);
        if (timer == timers_.end()) continue;

        callbacks.push_back(timer->second.callback());

        // whether it is repeated.
        if (timer->second.repeat()) {
            timer->second.toNextTimeout();
            queue_.push(std::make_pair(timer->second.timeout(), pair.second));
        } else {
            timers_.erase(timer);
        }
    }
}

bool TimerQueue::empty() const { return timers_.empty(); }

}  // namespace network