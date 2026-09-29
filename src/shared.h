#pragma once

#include <iostream>

#define STR2(x) #x
#define STR(x) STR2(x)

struct FastIO {
    FastIO() noexcept {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }
};
inline FastIO fast_io_dummy;
