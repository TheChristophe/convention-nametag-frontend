#ifndef CONVENTION_NAMETAG_UTILITY_HPP
#define CONVENTION_NAMETAG_UTILITY_HPP

#include <cstdint>

namespace util::timing {
    // Get current time units
    [[nodiscard]] int64_t Get();

    // Get time units per second
    [[nodiscard]] int64_t Frequency();
} // namespace util::timing

#endif
