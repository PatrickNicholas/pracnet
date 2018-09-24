#include <network/Selector.h>

#include <memory>

#include <network/config.h>

// FIXME: #ifdef LINUX
#ifdef _WIN32
#elif defined(HAVE_EPOLL)
#include "platform/Epoll.h"
#else

#endif

namespace network {

std::shared_ptr<Selector> CreateSelector()
{
#if defined(HAVE_EPOLL)
    return std::make_shared<Epoll>();
#endif
}

} // namespace network