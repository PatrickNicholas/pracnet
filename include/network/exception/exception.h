#pragma once

#include <string>

namespace network {

class Exception {
    std::string message_;

   public:
    Exception(std::string what) : message_(what) {}

    const std::string& message() const { return message_; }
};

}  // namespace network