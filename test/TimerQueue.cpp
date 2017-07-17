#include "unittest.h"
#include <chrono>
#include <thread>
#include <vector>
#include <TimerQueue.h>

using namespace network;

TEST_CASE(timer_queue_insert)
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    using std::this_thread::sleep_for;

    TimerQueue queue;

    auto now = high_resolution_clock::now();
    bool status1 = false;
    queue.insert(now + milliseconds(Timer::minTimeSlice), [&status1]() {
        status1 = true;
    });

    sleep_for(milliseconds(Timer::minTimeSlice));
    std::vector<TimerCallback> callbacks;
    queue.getTimeoutCallbacks(callbacks);
    for (auto call : callbacks) {
        call();
    }
    TEST_REQUIRE(status1, "insert or getTimeoutCallbacks failed.");
}

TEST_CASE(timer_queue_timer) 
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    using std::this_thread::sleep_for;

    TimerQueue queue;
    long res = queue.getMinimumTimeout();
    TEST_REQUIRE(res == -1, "the resolver can block until the IO event occurs");

    auto now = high_resolution_clock::now();
    queue.insert(now + milliseconds(Timer::minTimeSlice), []() { });
    res = queue.getMinimumTimeout();
    TEST_REQUIRE(res > 0, "special time");

    sleep_for(milliseconds(Timer::minTimeSlice + 10));
    res = queue.getMinimumTimeout();
    TEST_REQUIRE(res == 0, "needs to be returned directly");
}