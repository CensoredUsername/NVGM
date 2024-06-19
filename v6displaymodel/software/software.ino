#include <avr/io.h>

#include "ws2813_bitbang.h"
#include "firesim.h"
#include "simple_gpio.h"

using ws2813::Pixel;

static const Pixel firesim_palette[32] = {
    Pixel(0x00, 0x00, 0x00),
    Pixel(0x0D, 0x01, 0x00),
    Pixel(0x2D, 0x02, 0x01),
    Pixel(0x4F, 0x06, 0x01),
    Pixel(0x6E, 0x09, 0x01),
    Pixel(0x8D, 0x0B, 0x01),
    Pixel(0xAB, 0x0C, 0x01),
    Pixel(0xC7, 0x0D, 0x00),
    Pixel(0xDC, 0x11, 0x00),
    Pixel(0xEB, 0x1B, 0x00),
    Pixel(0xF5, 0x26, 0x00),
    Pixel(0xFC, 0x31, 0x00),
    Pixel(0xFF, 0x3C, 0x00),
    Pixel(0xFF, 0x47, 0x00),
    Pixel(0xFF, 0x51, 0x03),
    Pixel(0xFF, 0x5B, 0x0D),
    Pixel(0xFF, 0x63, 0x17),
    Pixel(0xFF, 0x6B, 0x23),
    Pixel(0xFF, 0x71, 0x2F),
    Pixel(0xFF, 0x77, 0x3B),
    Pixel(0xFF, 0x7C, 0x48),
    Pixel(0xFF, 0x81, 0x54),
    Pixel(0xFF, 0x86, 0x60),
    Pixel(0xFF, 0x89, 0x6B),
    Pixel(0xFF, 0x8C, 0x76),
    Pixel(0xFF, 0x90, 0x80),
    Pixel(0xFF, 0x94, 0x88),
    Pixel(0xFF, 0x96, 0x8F),
    Pixel(0xFF, 0x99, 0x94),
    Pixel(0xFF, 0x9B, 0x97),
    Pixel(0xFF, 0x9C, 0x98),
    Pixel(0xFF, 0x9D, 0x99),
};

static const Pixel balanced = Pixel(0xFF, 0x9D, 0x99);

void h_to_rgb(Pixel *pixel, uint16_t angle) {
    while (angle > 360) {
        angle -= 360;
    }
    if (angle < 60) {
        pixel->r = 0xFF;
        pixel->b = 0;
        pixel->g = (0xFF * angle) / 60;
    } else if (angle < 120) {
        pixel->g = 0xFF;
        pixel->b = 0;
        pixel->r = (0xFF * (120 - angle)) / 60;
    } else if (angle < 180) {
        pixel->g = 0xFF;
        pixel->r = 0;
        pixel->b = (0xFF * (angle - 120)) / 60;
    } else if (angle < 240) {
        pixel->b = 0xFF;
        pixel->r = 0;
        pixel->g = (0xFF * (240 - angle)) / 60;
    } else if (angle < 300) {
        pixel->b = 0xFF;
        pixel->g = 0;
        pixel->r = (0xFF * (angle - 240)) / 60;
    } else {
        pixel->r = 0xFF;
        pixel->g = 0;
        pixel->b = (0xFF * (360 - angle)) / 60;
    }
}

// generates a waveform compatible with Ws2812 control
ws2813::Segment segment;
#define FRAMEBUFFER_SIZE (8 * 12)
Pixel framebuffer[8 * 12];

void setup() {
    // initialize output pin
    cli();
    ws2813::configure_segment(&segment, PD, 3);
    ws2813::framing_init();

    set_mode(PA, 0, false);
    set_pullup(PA, 0, false);
}


void loop() {
    // put your main code here, to run repeatedly:
    cli();

    for (uint32_t ticks = 0;; ticks++) {
        ws2813::framing_start(WS2813_FRAMING_FPS(60));
        if (!read_pin(PA, 0)) {
            ws2813::write_segment(framebuffer, 8 * 12, &segment);
        }
        ws2813::framing_end();

        if (ticks < 60) {

        } else {
            do_firesim(0);
        }/* else if (ticks < 360) {
            fade();
        } else {
            white_rotate(0);
        }*/
    }
}




// effects. 
void fade() {
    // fade out effect
    for (uint8_t i = 0; i < FRAMEBUFFER_SIZE; i++) {
        Pixel& pixel = framebuffer[i];
        pixel.r = (uint16_t(pixel.r) * 249) >> 8;
        pixel.g = (uint16_t(pixel.g) * 249) >> 8;
        pixel.b = (uint16_t(pixel.b) * 249) >> 8;
    } 
}

void do_firesim(uint8_t offset) {
    // fire simulation effect
    static Firesim firesim;
    firesim.tick();
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 6; x++) {
            framebuffer[offset + x * 8 + y] = firesim_palette[firesim.get_pixel(x, y)];
        }
    }
}

void white_rotate(uint8_t offset) {
    // white strips rotating at 60deg/sec over the motor module
    static uint8_t angle = 0;
    angle += 1;
    if (angle > 180) {
        angle = 0;
    }

    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t c = 0;

    uint8_t new_angle;

    if (angle < 60) {
        new_angle = angle * 4;
        a = 240 - new_angle;
        b = new_angle;

    } else if (angle < 120) {
        new_angle = (angle - 60) * 4;
        b = 240 - new_angle;
        c = new_angle;

    } else {
        new_angle = (angle - 120) * 4;
        c = 240 - new_angle;
        a = new_angle;
    }

    Pixel a_pixel((uint16_t(balanced.r) * a) >> 8, (uint16_t(balanced.g) * a) >> 8, (uint16_t(balanced.b) * a) >> 8);
    Pixel b_pixel((uint16_t(balanced.r) * b) >> 8, (uint16_t(balanced.g) * b) >> 8, (uint16_t(balanced.b) * b) >> 8);
    Pixel c_pixel((uint16_t(balanced.r) * c) >> 8, (uint16_t(balanced.g) * c) >> 8, (uint16_t(balanced.b) * c) >> 8);

    for (uint8_t y = 0; y < 8; y++) {
        framebuffer[offset + 0 * 8 + y] = a_pixel;
        framebuffer[offset + 1 * 8 + y] = b_pixel;
        framebuffer[offset + 2 * 8 + y] = c_pixel;
        framebuffer[offset + 3 * 8 + y] = a_pixel;
        framebuffer[offset + 4 * 8 + y] = b_pixel;
        framebuffer[offset + 5 * 8 + y] = c_pixel;
    }
}

void payload_scroll() {
    //
}
