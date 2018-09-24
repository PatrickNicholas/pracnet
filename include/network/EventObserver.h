#pragma once 

#include <network/platform/EventID.h>
#include <network/Event.h>

namespace network {

class EventObserver {
public:
	virtual EventID id() const = 0;
	virtual ~EventObserver() {};
	virtual bool isReading() const = 0;
	virtual bool isWriting() const = 0;
	virtual void dispatch(const Event &event) = 0;
};

struct EventContext {
	Event event;
	EventObserver *observer_;
};

}  // namespace network