#ifndef _LEDSTRIPESPLITTER_H_
#define _LEDSTRIPESPLITTER_H_

typedef struct {
    uint8_t port_lb;
    uint8_t pinmask;
} Segment;

typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} Pixel;

#endif
