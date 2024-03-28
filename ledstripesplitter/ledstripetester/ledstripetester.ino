#include <avr/io.h>

#include "ledstripetester.h"

#if F_CPU != 20000000
#error "F_CPU is not set to 20MHz, this code will not generate the correct signal"
#endif

Pixel bytecode_white[8] = {
    {0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF}
};

Pixel bytecode_phase1[8] = {
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00
};

Pixel bytecode_phase2[8] = {
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
};

Pixel bytecode_phase3[8] = {
    0x00, 0xFF, 0x00,  
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
};

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
Segment segment;
Pixel framebuffer[8 * 5] = {{0,0,0}};

void setup() {
    // initialize output pin
    cli();
    configure_segment(&segment, PD, 5);
    framing_init();
}


void loop() {
    // put your main code here, to run repeatedly:
    cli();

    // white tester
    for (uint8_t i = 0; i < 40; i++) {
        framebuffer[i].r = 0xFF;
        framebuffer[i].g = 0xFF;
        framebuffer[i].b = 0xFF;
    }
    for (;;) {
        framing_start(20833);
        blit_segment(framebuffer, 40, &segment);
        framing_end();
    }


    // color wheel tester
    uint16_t angle = 0;
    for(;;) {
        framing_start(20833);
        blit_segment(framebuffer, 40, &segment);
        framing_end();

        angle += 5;
        if (angle >= 360) {
            angle -= 360;
        }
        for (uint8_t col = 0; col < 5; col++) {
            for (uint8_t row = 0; row < 8; row++) {
                h_to_rgb(&framebuffer[col * 8 + row], angle + 45 * (col + row));
            }
        }        
    }

    uint8_t frame = 0;
    uint8_t state = 0;

    for(;;) {
        framing_start(20833);
        blit_segment(framebuffer, 40, &segment);
        framing_end();

        memset(framebuffer, 0x08, sizeof(framebuffer));
        // draw heart
        uint8_t initial_offset = 0;
        if (++frame == 30) {
            frame = 0;
            if (++state == 12) {
                state = 0;
            }
        }
        switch(state) {
            case 0:
            case 2:
            case 4:
            case 6:
            case 8:
            case 10:
                continue;
            case 1:
                initial_offset = 0;
                break;
            case 3:
            case 11:
                initial_offset = 1;
                break;
            case 5:
            case 9:
                initial_offset = 2;
                break;
            case 7:
                initial_offset = 3;
                break;
        }
        framebuffer[initial_offset + 1].r = 0xFF;
        framebuffer[initial_offset + 3].r = 0xFF;

        framebuffer[initial_offset + 8].r = 0xFF;
        framebuffer[initial_offset + 10].r = 0xFF;
        framebuffer[initial_offset + 12].r = 0xFF;

        framebuffer[initial_offset + 16].r = 0xFF;
        framebuffer[initial_offset + 20].r = 0xFF;

        framebuffer[initial_offset + 25].r = 0xFF;
        framebuffer[initial_offset + 27].r = 0xFF;

        framebuffer[initial_offset + 34].r = 0xFF;
    }








    for (uint16_t i = 0; i < 60 ; i++) {
        framing_start(20833);
        blit_segments(bytecode_white, 8, &segment, 1);
        blit_segments(bytecode_white, 8, &segment, 1);
        blit_segments(bytecode_white, 8, &segment, 1);
        blit_segments(bytecode_white, 8, &segment, 1);
        blit_segments(bytecode_white, 8, &segment, 1);
        framing_end();
    }
    for (uint8_t i = 0; i < 2; i++) {
        for (uint16_t i = 0; i < 10 ; i++) {
            framing_start(20833);
            blit_segments(bytecode_phase1, 8, &segment, 1);
            blit_segments(bytecode_phase1, 8, &segment, 1);
            blit_segments(bytecode_phase1, 8, &segment, 1);
            blit_segments(bytecode_phase1, 8, &segment, 1);
            blit_segments(bytecode_phase1, 8, &segment, 1);
            framing_end();
        }
        for (uint16_t i = 0; i < 10 ; i++) {
            framing_start(20833);
            blit_segments(bytecode_phase2, 8, &segment, 1);
            blit_segments(bytecode_phase2, 8, &segment, 1);
            blit_segments(bytecode_phase2, 8, &segment, 1);
            blit_segments(bytecode_phase2, 8, &segment, 1);
            blit_segments(bytecode_phase2, 8, &segment, 1);
            framing_end();
        }
        for (uint16_t i = 0; i < 10 ; i++) {
            framing_start(20833);
            blit_segments(bytecode_phase3, 8, &segment, 1);
            blit_segments(bytecode_phase3, 8, &segment, 1);
            blit_segments(bytecode_phase3, 8, &segment, 1);
            blit_segments(bytecode_phase3, 8, &segment, 1);
            blit_segments(bytecode_phase3, 8, &segment, 1);
            framing_end();
        }
    }
}

void wait(uint8_t us) {
    for (uint8_t i = 0; i < 1; i++) {
    asm volatile (
        "loop: \n"
            "nop \n nop \n nop \n nop \n nop\n"
            "nop \n nop \n nop \n nop \n nop \n"
            "nop \n nop \n nop \n nop \n nop \n"
            "nop \n nop \n"
            "dec %[count] \n"
            "brne loop \n"
            "nop \n"
        : [count]"+r"(us)
        :
        :
    );
    }
}

// frame control. blit_start returns a certain time after the previous blit_start was called, and 1ms after blit_end was used.
// uses TCB0

void framing_init() {
    // we need to use the prescaler from TCA, so configure that
    // initialize the timer to 1000, counting down, and halting when it reaches zero
    // prescaler set to 16 so time is specified in 0.8us. max delay is therefore ~50ms
    TCA0.SINGLE.CTRLA = (4 << 1) | 1; // enable, set prescaler to 16
    TCA0.SINGLE.EVCTRL = 0; // disable count on event input
    // so now CLK_TCA runs at 0.8us per tick.

        
    TCB0.CTRLA = 1 | (2 << 1); // enable, use CLK_TCA
    TCB0.CTRLB = 0x6; // single shot mode
    TCB0.EVCTRL = 0; // no events used
    TCB0.INTCTRL = 0; // no capture interrupt
    TCB0.CCMP = 62500; // in single shot mode this sets the TOP value at which the counter stops counting
    TCB0.CNT = (62500 - 1250); // set to 1ms remaining
}

// frame time specified in units of 0.8us
void framing_start(uint16_t frame_time) {
    // wait for the timer to stop running
    while (TCB0.STATUS & 1);
    TCB0.CNT = 62500 - frame_time;
}

void framing_end() {
    // if there's less than 1ms remaining, set the counter to 1ms remaining
    if (TCB0.CNT > (62500 - 1250)) {
        TCB0.CNT = (62500 - 1250);
    }
}

// writing to the "screen"
void blit_segment(Pixel *data, uint16_t length, Segment *segment) {
    length *= 3;

    // here be assembly
    asm volatile (
        "byte_loop: \n"
            "ld r0, %a[data]+ \n" // 2 cycles
            "ldi r18, 8 \n" // 1 cycle
            "bit_loop: \n"
                "st %a[port_addr], %[pin_mask] \n" // 1 cycle
                "nop \n nop \n nop \n nop \n" // 4 cycle delay
                "lsl r0 \n" // 1 cycle
                "brcs one \n" // 2 cycles if taken, 1 if not
                    "st %a[port_addr], %[pin_mask] \n" // 1 cycle
                    "nop \n nop \n nop \n nop \n" // 4 cycle delay
                    "rjmp bit_end \n" // 2 cycles
                "one: \n"
                    "nop \n nop \n nop \n nop \n nop \n" // 5 cycle delay
                    "st %a[port_addr], %[pin_mask] \n" // 1 cycle
            "bit_end: \n" 
                "subi r18, 1 \n" // 1 cycle
                "breq byte_end \n" // 2 cycles if taken, 1 if not
                    "nop \n nop \n nop \n nop \n nop \n nop \n" // 6 cycle delay                
                    "rjmp bit_loop \n" // 2 cycles
                "byte_end: \n"
                "sbiw %[data_remaining], 1 \n" // 2 cycles
                "brne byte_loop \n" // 2 cycles
    : [data]"+e"(data)
    , [data_remaining]"+w"(length)
    : [port_addr]"e"((uint16_t *)(0x0400 + uint16_t(segment->port_lb)))
    , [pin_mask]"r"(segment->pinmask)
    : "r0", "r18"
    );
}

void blit_segments(Pixel* data, uint16_t segment_length, Segment *segments, uint8_t segment_count) {
    for (uint8_t i = 0; i < segment_count; i++) {
        blit_segment(data, segment_length, segments++);
        data = data + segment_length;
    }
}

// segment configuration

void configure_segment(Segment *segment, uint8_t port, uint8_t pin) {
    // set bytecode entry.
    segment->port_lb = 0x07 + 0x20 * port;
    segment->pinmask = 1 << pin;

    // pin init. set direction and initialize low.
    PORT_t& port_ref = (&PORTA)[port];
    port_ref.DIRSET = 1 << pin;
    port_ref.OUTCLR = 1 << pin;
}


