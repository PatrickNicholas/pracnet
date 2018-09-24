#pragma once 

#include <string>

#include <network/exception/exception.h>
#include <network/ip/helper/socket.h>

namespace network {
inline static void ThrowException(const std::string &what) {
	std::string error = socket::error();
	std::string message = what + ": " + error;
	throw Exception{ message };
}
}  // namespace network