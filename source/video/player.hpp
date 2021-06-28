#ifndef CONVENTION_NAMETAG_PLAYER_HPP
#define CONVENTION_NAMETAG_PLAYER_HPP

#include <cstddef>
#include <cstdint>

class Player {
    public:
    virtual ~Player();

    virtual void GetFrame(uint8_t *buffer, size_t bufferSize) = 0;

    private:
};

#endif
