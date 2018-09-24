#pragma once 

#include <network/ip/Socket.h>
#include <network/ip/SocketOwner.h>
#include <network/ip/InetAddress.h>

#include <iostream>

namespace network {
namespace ip {

class SocketOwner : public Socket {
	SocketOwner(SocketOwner &) = delete;
	SocketOwner& operator=(SocketOwner&) = delete;
public:
	SocketOwner() : Socket() {}
	SocketOwner(socket::socket_t sock) : Socket(sock) {}
	SocketOwner(SocketOwner&&) = default;
	SocketOwner& operator=(SocketOwner&&) = default;

	SocketOwner(Socket &&socket)
		: Socket(std::move(socket))
	{}

	void bind(const InetAddress &addr);
};

}
}  // namespace network::ip