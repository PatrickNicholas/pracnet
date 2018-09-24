#pragma once

#include <functional>

#include <network/platform/EventID.h> // EventID
#include <network/Event.h>	// Event
#include <network/EventObserver.h>
#include <network/EventBase.h>

namespace network {

/** 
 * EventHandler - noncopyable
 */
class EventHandler final : public EventObserver
{
    using ReadCallback = std::function<void()>;
    using WriteCallback = std::function<void()>;
    using ErrorCallback = std::function<void()>;

    unsigned status_;
	EventID id_;
    EventBase &base_;
    ReadCallback readCallback_;
    WriteCallback writeCallback_;
    ErrorCallback errorCallback_;

public:
    enum { kNoneEvent = 0x0, kReadEvent = 0x1, kWriteEvent = 0x2 };

    EventHandler(const EventHandler &) = delete;
    EventHandler &operator = (const EventHandler &) = delete;

    EventHandler(EventBase &base, EventID id);
    ~EventHandler();

    void dispatch(const Event &event) override;
    
	EventID id() const override {
		return id_;
	}

    void enableReading() {
        status_ |= kReadEvent;
        update();
    }

    void enableWriting() {
        status_ |= kWriteEvent;
        update();
    }

    void disableReading() {
        status_ &= ~kReadEvent;
        update();
    }

    void disableWriting() {
        status_ &= kWriteEvent;
        update();
    }

    bool isReading() const override {
        return status_ & kReadEvent;
    }

    bool isWriting() const override {
        return status_ & kWriteEvent;
    }

    void setReadCallback(ReadCallback call) {
        readCallback_ = call;
    }

    void setWriteCallback(WriteCallback call) {
        writeCallback_ = call;
    }

    void setErrorCallback(ErrorCallback call) {
        errorCallback_ = call;
    }

private:
    void update();
    void remove();
};

} // namespace network