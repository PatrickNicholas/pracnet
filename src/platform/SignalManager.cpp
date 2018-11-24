#include <network/SignalManager.h>

#include <cstdio>
#include <cstdlib>

#include <network/ip/helper/socket.h>

namespace network {

namespace {

int MakeInternalPipe(socket::socket_t fd[2]) {
    if (socket::socketpair(AF_INET, SOCK_STREAM, 0, fd) == 0) {
        if (socket::setnonblocking(fd[0]) < 0 ||
            socket::setnonblocking(fd[1]) < 0 ||
            socket::setcloseexec(fd[0]) < 0 ||
            socket::setcloseexec(fd[1] < 0)) {
            socket::close(fd[0]);
            socket::close(fd[1]);
            fd[0] = fd[1] = -1;
            return -1;
        }
        return 0;
    }
    fd[0] = fd[1] = -1;
    return -1;
}

}  // namespace

SignalManager::SignalManager(EventBase& base) : base_{base} {
    // fd[0] - write, fd[1] - read.
    int fd[2];
    if (MakeInternalPipe(fd) < 0) {
        fprintf(stderr, "make_internal_pipe");
        exit(1);
    }

    // INIT signal handle.
    writefd_ = fd[0];
    // handle_ = std::make_shared<EventHandler>(fd[1]);
}

SignalManager::~SignalManager() {
    // TODO: remove from eventbase.
}

void SignalManager::initialize() {
    // base_.insert(*handle_);

    //::signal(SIGABRT, );
    //::signal(SIGFPE, );
    //::signal(SIGILL, );
    //::signal(SIGINT, );
    //::signal(SIGSEGV, );
    //::signal(SIGTERM, );
}

void SignalManager::onSignal() {
    // TODO:
}

}  // namespace network