#ifndef _RANDOM_H_
#define _RANDOM_H_

// just some quick decent-ish 8-bit prng

struct Random {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t counter;

    Random() : a(0), b(0), c(0), d(0) { }
    Random(uint32_t seed) : a(0), b(0), c(0), d(0) {
        this->seed(seed);
    }

    uint8_t random() {
        uint8_t t = this->a ^ (this->a >> 1);
        this->a = this->b;
        this->b = this->c;
        this->c = this->d;
        this->d = this->d ^ (this->d >> 3) ^ t ^ (t << 1);
        this->counter += 197;
        return this->d ^ this->counter;

    }

    void seed(uint32_t seed) {
        this->a ^= uint8_t(seed);
        this->b ^= uint8_t(seed >> 8);
        this->c ^= uint8_t(seed >> 16);
        this->d ^= uint8_t(seed >> 24);
    }
};

#endif
