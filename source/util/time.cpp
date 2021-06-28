#include "time.hpp"

#include <sys/time.h>

namespace util::timing {
    int64_t Get()
    {
        struct timeval val {
        };
        gettimeofday(&val, nullptr);
        return val.tv_sec * static_cast<int64_t>(1000000) + val.tv_usec;
    }

    int64_t Frequency()
    {
        return 1000000;
    }
} // namespace util::timing
