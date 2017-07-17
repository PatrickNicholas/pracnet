#pragma once 

#include <cassert>

static inline void unreachable() {
    assert(0 && "unreachable");
}