#include <network/EventHandler.h>

namespace network {

EventHandler::EventHandler(EventBase &base, EventID id)
    : status_{ kNoneEvent }
	, id_{id}
    , base_{ base }
{}

EventHandler::~EventHandler() 
{
    remove();
}

void EventHandler::dispatch(const Event &event) 
{
    if (event.error())
        errorCallback_();
    
    if (event.read())
        readCallback_();
    
    if (event.write())
        writeCallback_();
}

void EventHandler::update()
{
    base_.update(*this);
}

void EventHandler::remove()
{
    base_.remove(*this);
}

} // namespace network