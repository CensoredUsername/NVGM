#include <avr/io.h>

#include "ledstripesplitter.h"


// THE FOLLOWING VALUES CAN BE CHANGED TO SUIT THE WIRING


// port index of the input pin being used
#define INPUT_PORT PA

// pin index of the input pin being used
#define INPUT_PIN 0

// the amount of segments that the signal will be split over. Maxes out at 127
#define SEGMENT_COUNT 12

// the amount of LEDs in a single segment
#define SEGMENT_SIZE (2 * 4 * 5)

// the actual segment pin assignment is located in setup() below.


// END OF USER CONFIG




#if F_CPU != 20000000
#error "F_CPU is not set to 20MHz, this code will not generate the correct signal. check boards.txt"
#endif

// amount of cycles that the input has to be low to register a reset condition. Supports values up to 1500.
// should be at most 1000 (50us), and at least 8us
#define RESET_CYCLES 600

// storage for segment bytecode. Don't change this.
#define MAX_SEGMENT_COUNT 127
Segment segments[MAX_SEGMENT_COUNT] = {{0, 0}};
Pixel framebuffer[60 * 8 * 2] = {{0, 0, 0}};

void setup() {
    cli();
    configure_input();

    // THE FOLLOWING CODE CONFIGURES THE PINS THAT THE SIGNAL WILL BE SPLIT OVER.
    // The arguments represent segment index, port, and pin index within said port.

    configure_segment(segments + 0, PD, 5);
    configure_segment(segments + 1, PD, 4);
    configure_segment(segments + 2, PA, 3);
    configure_segment(segments + 3, PA, 2);
    configure_segment(segments + 4, PD, 0);   

    configure_segment(segments + 5, PF, 5);
    configure_segment(segments + 6, PC, 6);
    configure_segment(segments + 7, PB, 2);
    configure_segment(segments + 8, PF, 4);
    configure_segment(segments + 9, PA, 1);
    configure_segment(segments + 10, PE, 3);
    configure_segment(segments + 11, PB, 0);
}

void loop() {
    framing_init();

    uint16_t angle = 0;
    for (uint16_t i = 0; i < 3600; i++) {
        framing_start(10000);
        blit_segments(framebuffer, 40, segments, 12);
        framing_end();

        angle += 10;
        if (angle >= 360) {
            angle -= 360;
        }
        for (uint8_t row = 0; row < 60; row++) {
            for (uint8_t col = 0; col < 8; col++) {
                h_to_rgb(&framebuffer[row * 8 + col], angle + 0 * (col >> 1) + 15 * row);
            }
        }     
    }
    
    handle_splitting(segments, SEGMENT_COUNT, SEGMENT_SIZE);
}

void configure_input() {
    // configure input pin as input, enable pullup so it doesn't float.
    PORT_t& port_ref = (&PORTA)[INPUT_PORT];
    port_ref.DIRCLR = 1 << INPUT_PIN;
    (&port_ref.PIN0CTRL)[INPUT_PIN] = 0x08;
}

void configure_segment(Segment *segment, uint8_t port, uint8_t pin) {
    // set bytecode entry.
    segment->port_lb = 0x07 + 0x20 * port;
    segment->pinmask = 1 << pin;

    // pin init. set direction and initialize low.
    PORT_t& port_ref = (&PORTA)[port];
    port_ref.DIRSET = 1 << pin;
    port_ref.OUTCLR = 1 << pin;
}

void wait(uint8_t us) {
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

void handle_splitting(Segment *segments, uint8_t segment_count, uint8_t segment_length) {
    asm volatile (
        // register allocation
        // Z (r30:r31) contains a pointer to the current entry in the bytecode
        // X (r26:r27) contains a pointer to the toggle register for the current output port
        // r25:r24 contains the bits remaining in the current segment
        // r18 contains either the reset counter or the segment counter
        // r0 contains the bitmask for the current output port
        ".entry_point: \n"
            "movw r30, %A[bytecode_addr] \n" // load segment bytecode address into Z register
            "ldi r27, %[port_reg_hibyte] \n" // X high byte, high byte of all port register addresses

        // reset await loop prologue
        ".restart_reset_loop: \n"
            "ldi r18, %[reset_loop_cycles] \n"
        ".reset_loop: \n" // this takes 6 cycles to loop, which at 20MHz means ~0.3us per loop
            "sbic %[input_port], %[input_pin] \n" // 1 cycle + 2 if next is skipped
                "rjmp .restart_reset_loop \n" // 2 cycles
            "dec r18 \n" // 1 cycle
            "brne .reset_loop \n" // 2 cycles if taken, 1 if not

        // set segment count
        "mov r18, %[segment_count] \n" // amount of segments to go through

        ".next_segment: \n"
            "ld r26, Z+ \n" // 2 cycles, load low byte of X from bytecode
            "ld r0, Z+ \n" // 2 cycles, load the correct bitmask from bytecode

        ".next_bit: \n" // start of the loop per bit
        
            // await a rising edge. This loop takes 3 cycles, or terminates 2, 3 or 4 cycles after a rising edge has happened.
            "sbis %[input_port], %[input_pin] \n" // 1 cycle
                "rjmp .next_bit \n" // 2 cycles if taken, 1 if skipped (one word instr)

            // 8 cycles remaining
            
            // padding
            "nop \n nop \n nop \n" // 3 cycles

            // toggle the output high.
            "st X, r0 \n" // 1 cycle. now we have either 5 or 13 cycles till we should go down.

            // reset bitcount if it reached zero
            "sbiw r24, 1 \n" // 2 cycles, decrement bitcount. store equality to zero in flags
            "brne .notzero \n" // 2 cycles if taken, 1 if not
                "movw r24, %A[bitcount_per_segment] \n" // 1 cycle, load amount of bits in bitcount to counter
            ".notzero: \n"

            // read if we should be high or low (6 cycles after the end of the rising_edge loop)
            "sbic %[input_port], %[input_pin] \n" // 1 cycle
            "rjmp .one \n" // 2 cycles if taken, 1 if not.
            
            // a zero was detected.
            ".zero: \n"
                // go low 6 cycles after the original go high
                "st X, r0 \n" // 1 cycle.

                // if not at the end of the bitcount loop, start scanning again.
                "brne .next_bit \n" // 2 cycles if taken, one if not

                // decrement segment counter and either go back to the next segment or the entry point
                "dec r18 \n" // 1 cycle
                "brne .next_segment \n" // 2 cycles if taken, one if not.
                "rjmp .entry_point \n" // 2 cycles

            // a one was detected. 
            ".one: \n"
                // 6 cycles to do something with
                "nop \n nop \n nop \n nop \n" // wait 4 cycles
                "brne .simple \n" // 2 cycles if taken, one if not
                "dec r18 \n" // one cycle
            
                // go low 13 cycles after the original go high
                "st X, r0 \n" // 1 cycle.
                "brne .next_segment \n" // 2 cycles if taken, one if not.
                "rjmp .entry_point \n" // 2 cycles

            ".simple: \n"
                // go low and start looking for the next bit
                "st X, r0 \n" // 1 cycle.
                "rjmp .next_bit \n" // 2 cycles

        : /* no outputs */
        : [segment_count]"r"(segment_count)
        , [bitcount_per_segment]"r"(24*(uint16_t)segment_length)
        , [reset_loop_cycles]"M"(RESET_CYCLES / 6)
        , [bytecode_addr]"r"(segments)
        , [input_port]"I"(INPUT_PORT * 4 + 2)
        , [input_pin]"I"(INPUT_PIN)
        , [port_reg_hibyte]"I"(0x04)
        // clobbers
        : "r0", "r18", "r24", "r25", "r26", "r27", "r30", "r31"
    );
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
        ".byte_loop: \n"
            "ld r0, %a[data]+ \n" // 2 cycles
            "ldi r18, 8 \n" // 1 cycle
            ".bit_loop: \n"
                "st %a[port_addr], %[pin_mask] \n" // 1 cycle
                "nop \n nop \n nop \n nop \n" // 4 cycle delay
                "lsl r0 \n" // 1 cycle
                "brcs .send_one \n" // 2 cycles if taken, 1 if not
                    "st %a[port_addr], %[pin_mask] \n" // 1 cycle
                    "nop \n nop \n nop \n nop \n" // 4 cycle delay
                    "rjmp .bit_end \n" // 2 cycles
                ".send_one: \n"
                    "nop \n nop \n nop \n nop \n nop \n" // 5 cycle delay
                    "st %a[port_addr], %[pin_mask] \n" // 1 cycle
            ".bit_end: \n" 
                "subi r18, 1 \n" // 1 cycle
                "breq .byte_end \n" // 2 cycles if taken, 1 if not
                    "nop \n nop \n nop \n nop \n nop \n nop \n" // 6 cycle delay                
                    "rjmp .bit_loop \n" // 2 cycles
                ".byte_end: \n"
                "sbiw %[data_remaining], 1 \n" // 2 cycles
                "brne .byte_loop \n" // 2 cycles
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

// utilities

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
