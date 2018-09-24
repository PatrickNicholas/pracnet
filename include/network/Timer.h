#pragma once

#include <cassert>
#include <chrono>
#include <functional>

namespace network {

typedef std::function<void()> TimerCallback;

// copyable
class Timer
{
    using milliseconds = std::chrono::milliseconds;
    using high_resolution_clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::high_resolution_clock::time_point;

    Timer(time_point timeout, TimerCallback callback, 
        bool repeat, milliseconds after) 
        : repeat_(repeat)
        , timeout_(timeout)
        , callback_(callback)
        , after_(after)
    {}

    bool repeat_;
    time_point timeout_;
    TimerCallback callback_;
    milliseconds after_;

public:
    Timer(milliseconds after, TimerCallback callback)
        : Timer(high_resolution_clock::now() + after, callback, true, after)
    {
        assert(after.count() >= minTimeSlice && 
            "repeat time less than minimum time slice.");
    }

    Timer(time_point timeout, TimerCallback callback) 
        : Timer(timeout, callback, false, milliseconds(0))
    {}

    auto timeout() -> decltype(timeout_) const {
        return timeout_;
    }

    void toNextTimeout() {
        assert(repeat_ && "call toNextTimeout on non-repeat timer.");
        timeout_ += after_;
    }

    TimerCallback callback() const {
        return callback_;
    }

	void operator()() const {
		callback_();
	}

    bool repeat() const { 
        return repeat_;
    }

    static const int minTimeSlice = 5;
};

} // namespace network