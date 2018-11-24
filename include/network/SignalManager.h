#pragma once

#include <csignal>
#include <functional>
#include <map>
#include <memory>

namespace network {

class EventBase;
class EventHandler;

typedef std::function<void()> SignalCallback;

class SignalManager {
    // FIXME:
    int writefd_ = -1;
    EventBase& base_;
    std::shared_ptr<EventHandler> handler_;
    std::map<int, SignalCallback> callbacks_;

   public:
    SignalManager(EventBase& base);
    ~SignalManager();

    SignalManager(const SignalManager&) = delete;
    SignalManager& operator=(const SignalManager&) = delete;

    void initialize();

    void setCallback(int signal, SignalCallback cb) {
        // TODO: check signal.
        callbacks_[signal] = cb;
    }

    void onSignal();
};

}  // namespace network