#pragma once 

#include <map>
#include <queue>
#include <vector>

#include <network/Timer.h>

namespace network {

class TimerQueue 
{
    using milliseconds = std::chrono::milliseconds;
    using time_point = std::chrono::high_resolution_clock::time_point;
    using TimerPair = std::pair<time_point, int>;

    std::map<int, Timer> timers_;
    std::priority_queue<TimerPair> queue_;  // std::pair compare first & second.
    
public:

    // noncopyable
    TimerQueue() = default;
    ~TimerQueue() = default;
    TimerQueue(const TimerQueue &) = delete;
    TimerQueue & operator =(const TimerQueue &) = delete;

    int insert(milliseconds after, TimerCallback callback);
    int insert(time_point timeout, TimerCallback callback);
    bool remove(int id);

    /**
     * If -1 is returned, it means that the resolver can block until 
     * the IO event occurs; if it returns 0, it needs to be returned 
     * directly; otherwise, the specific blocking time.
     */
    long getMinimumTimeout() const;

    void getTimeoutCallbacks(std::vector<TimerCallback> &callbacks);

	bool empty() const;
};

} // namespace network