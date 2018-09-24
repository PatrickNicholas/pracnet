#pragma once 

#include <network/ip/helper/socket.h>   // socket_t

namespace network {
namespace ip {

class Socket 
{
	Socket(const Socket &) = delete;
	Socket & operator = (const Socket &) = delete;

    socket::socket_t handle_;

public:
    explicit Socket();
    explicit Socket(socket::socket_t fd);
    ~Socket();

	Socket(Socket &&rhs) : handle_{INVALID_SOCKET} {
		using std::swap;
		swap(rhs.handle_, handle_);
	}

	Socket & operator=(Socket &&rhs) {
		using std::swap;
		handle_ = INVALID_SOCKET;
		std::swap(rhs.handle_, handle_);
		return *this;
	}

	static Socket create(int domain, int type, int protoco);

	// user ensure current Socket is empty.
	void set(socket::socket_t fd) {
		handle_ = fd;
	}

	void clear() {
		handle_ = INVALID_SOCKET;
	}

    socket::socket_t socket() const {
        return handle_;
    }

	bool valid() const {
		return handle_ != INVALID_SOCKET;
	}

	int setnonblocking();
	int setcloseexec();
    // TODO: options
};

} // namespace ip
} // namespace network