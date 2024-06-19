#ifndef _FIRESIM_H_
#define _FIRESIM_H_

#include "random.h"

#define FIRESIM_WIDTH 6
#define FIRESIM_HEIGHT 9

class Firesim {
public:
    uint8_t state[FIRESIM_HEIGHT * FIRESIM_WIDTH];
    Random random;


public:
    Firesim () : random(0xDEADC0DE) {
        // zero everything but the last line, which is set to 31
        memset(this->state, 0, FIRESIM_WIDTH * (FIRESIM_HEIGHT - 1));
        memset(this->state + FIRESIM_WIDTH * (FIRESIM_HEIGHT - 1), 25, FIRESIM_WIDTH);
    }

    void tick() {
        for (uint8_t y = 1; y < FIRESIM_HEIGHT; y++) {
            for (uint8_t x = 0; x < FIRESIM_WIDTH; x++) {
                // get some entropy
                uint8_t random = this->random.random();

                // where to spread to (25% chance left, 50% upwards, 25% right)
                uint8_t newx = x;
                if ((random & 3) == 0) {
                    if (x) {
                        newx--;
                    } else {
                        newx = FIRESIM_WIDTH - 1;
                    }
                } else if ((random & 3) == 3) {
                    if (x == (FIRESIM_WIDTH - 1)) {
                        newx++;
                    } else {
                        newx = 0;
                    }
                }

                // current cell value
                uint8_t value = this->state[y * FIRESIM_WIDTH + x] + 128;
                // remove some heat
                value -= (random >> 2) & 7;

                // dest cell value
                uint8_t& dest = this->state[(y - 1) * FIRESIM_WIDTH + newx];
                uint8_t current = dest;

                // intended change (compiler please do arithmetic right shift)
                current += (value - current + 2) >> 2;
                if (current < 32) {
                    dest = 0;
                } else {
                    dest = current - 32;
                }
            }
        }
    }

    uint8_t get_pixel(uint8_t x, uint8_t y) {
        return this->state[y * FIRESIM_WIDTH + x];
    }
};

#endif
