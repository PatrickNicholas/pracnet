#pragma once

namespace network {

#ifdef _WIN32
#include <Windows.h>
typedef HANDLE EventID;
#else
typedef int EventID;
#endif

}  // namespace network