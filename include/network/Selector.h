#pragma once

#include <vector>
#include <memory>

#include <network/Event.h>
#include <network/EventObserver.h>

namespace network {

class Selector;

// define at platform/Demultiplex.cpp
std::shared_ptr<Selector> CreateSelector();

class Selector
{
public:
    virtual ~Selector() {}

    // This must be used to ensure that the use of 
    // the pointer does not appear to hang.
	virtual bool empty() const = 0;
    virtual void update(const EventObserver &) = 0;
    virtual void remove(const EventObserver &) = 0;
    virtual void dispatch(std::vector<EventContext> &actives, long timeout) = 0;
};

} // namespace network