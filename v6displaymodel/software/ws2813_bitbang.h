#ifndef _WS2813_BITBANG_H_
#define _WS2813_BITBANG_H_

#include <avr/io.h>

// note: this code is purely written for the arduino nano every (ATMega4809), running at 20MHz
// it will likely not work on any other chip as it is cycle-accurate assembly.

#if F_CPU != 20000000
#error "F_CPU is not set to 20MHz, this library will not generate the correct signal"
#endif

// amount of cycles that the input has to be low to register a reset condition. Supports values up to 1500.
// should be at most 1000 (50us), and at least 8us
#define _WS2813_RESET_CYCLES 600

// convenience macro to calculate frametime from fps
#define WS2813_FRAMING_FPS(_fps) (uint16_t(1250000ul / uint32_t(_fps)))

namespace ws2813 {

    // a descriptor for a ws2813 led segment connected to a single output pin.
    // configure using `segment_configure`
    struct Segment {
        uint8_t port_lb;
        uint8_t pinmask;
    };

    // a single ws2813 data pixel, in GRB format. Packed as arrays of the thing are used directly.
    struct Pixel {
        uint8_t g;
        uint8_t r;
        uint8_t b;

        Pixel() : g(0), r(0) , b(0) { }
        Pixel(uint8_t r, uint8_t g, uint8_t b) : g(g), r(r), b(b) { }

    } __attribute__((packed));

    // frame control utilities. Uses TCB0 to provide configurable asynchronous timing.
    // framing controls both the FPS and ensures the LED protocol is correctly driven
    // while allowing remaining time to be spend on user code.
    // the structure of a single frame is as follows
    //
    // framing_start(WS2813_FRAMETIME_FPS(60));
    // blit_segments(framebuffer, leds_per_segment, segments, segment_count)
    // framing_end()
    // -- arbitrary user code


    // call framing_init during setup with interrupts disabled to configure the timer.
    inline void framing_init() {
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

    // Busy wait for the previous frame to finish. Then start sending the next frame. Frame time specified in units of 0.8us
    // i.e.
    // 30fps -> frametime = 41667
    // 60fps -> frametime = 20833
    // 100fps -> frametime = 12500
    // 120fps -> frametime = 10417
    inline void framing_start(uint16_t frame_time) {
        // wait for the timer to stop running
        while (TCB0.STATUS & 1);
        TCB0.CNT = 62500 - frame_time;
    }

    // Call at the end of writing output to leds, to ensure at least 1ms of idle time on the led output signal
    // to generate a proper reset.
    inline void framing_end() {
        // if there's less than 1ms remaining, set the counter to 1ms remaining
        if (TCB0.CNT > (62500 - 1250)) {
            TCB0.CNT = (62500 - 1250);
        }
    }

    // segment configuration to be run at setup time
    // example: `configure_segment(&segment, PD, 5) to configure segment to represent pin PD5
    inline void configure_segment(Segment *segment, uint8_t port, uint8_t pin) {
        // set bytecode entry.
        segment->port_lb = 0x07 + 0x20 * port;
        segment->pinmask = 1 << pin;

        // pin init. set direction and initialize low.
        PORT_t& port_ref = (&PORTA)[port];
        port_ref.DIRSET = 1 << pin;
        port_ref.OUTCLR = 1 << pin;
    }

    // led bitbanging code. This performs the work of parsing pixel data and sending that to a segment.
    inline void write_segment(const Pixel *data, uint16_t length, const Segment *segment) {
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

    // Write pixel data to `segment_count` segments automatically. Assumes each segment is of length `segment_length`.
    // `data` should be segment_length * segment_count pixels in size.
    inline void write_segments(const Pixel* data, uint16_t segment_length, const Segment *segments, uint8_t segment_count) {
        for (uint8_t i = 0; i < segment_count; i++) {
            write_segment(data, segment_length, segments++);
            data = data + segment_length;
        }
    }

    // led signal reading code: this code can read the WS2813 signal.
    // await a reset condition (line idle for _WS2813_RESET_CYCLES)
    // this requires the input port to be hard coded, otherwise time constraints cannot be met
    template<uint8_t input_port, uint8_t input_pin> 
    inline void read_signal(Pixel *data, uint16_t pixel_count) {
        asm volatile (
            // register allocation
            // r0 contains the reset counter or the accumulator
            ".entry_point: \n"

            // reset await loop prologue
            ".restart_reset_loop: \n"
                "ldi r0, %[reset_loop_cycles] \n"
            ".reset_loop: \n" // this takes 6 cycles to loop, which at 20MHz means ~0.3us per loop
                "sbic %[input_port], %[input_pin] \n" // 1 cycle + 2 if next is skipped
                    "rjmp .restart_reset_loop \n" // 2 cycles
                "dec r0 \n" // 1 cycle
                "brne .reset_loop \n" // 2 cycles if taken, 1 if not

            ".next_byte: \n"
                // set bit tracker and zero accumulator (we actually put a 1 in there so the 8th shift triggers the carry flag)
                "ldi r0, 1 \n" // 1 cycle

            // wait for line to go high
            ".next_bit: \n"
                // await a rising edge. This loop takes 3 cycles, or terminates 2, 3 or 4 cycles after a rising edge has happened.
                "sbis %[input_port], %[input_pin] \n" // 1 cycle
                    "rjmp .next_bit \n" // 2 cycles if taken, 1 if skipped (one word instr)

                // 8 cycles remaining
                "nop \n nop \n nop \n nop \n nop \n nop \n nop \n" // 7 cycles delay

                // shift accumulator left. This sets the carry flag if it's the final bit
                "lsl r0 \n" // 1 cycle

                // read if we should be high or low (6 cycles after the end of the rising_edge loop)
                "sbic %[input_port], %[input_pin] \n" // 1 cycle
                    "ori r0, 1 \n" // 1 cycle if taken or not. if set, or bit into accumulator. This doesn't set the carry flag ;)
            
                // if we aren't at the end of a byte, just await the next bit
                "brcc .next_bit \n" // 2 cycles if taken, 1 if not

                    // output data and increment data register, then clear accumulator and reset bit count
                    "st %a[data]+, r0 \n" // 1 cycle

                    // decrease data remaining, branch back if any still remaining
                    "sbiw %[data_remaining], 1 \n" // 2 cycles
                    "brne .next_byte \n" // 2 cycles if taken, 1 if not

            // if we get here, we're done!

            : [data]"+e"(data)
            , [data_remaining]"+w"(3 * pixel_count)
            : [reset_loop_cycles]"M"(_WS2813_RESET_CYCLES / 6)
            , [input_port]"I"(input_port * 4 + 2)
            , [input_pin]"I"(input_pin)
            // clobbers
            : "r0"
        );
    }

    // led signal splitting code: this code handles real-time reading and splitting of the ws2813 signal over multiple segments.
    // it does this for segment_count * segment_length pixels.
    // works by awaiting a reset condition first (line idle for _WS2813_RESET_CYCLES)
    // this requires the input port to be hard coded, otherwise time constraints cannot be met
    template<uint8_t input_port, uint8_t input_pin> 
    void split_to_segments(Segment *segments, uint8_t segment_count, uint8_t segment_length) {
        asm volatile (
            // register allocation
            // X (r26:r27) contains a pointer to the toggle register for the current output port
            // r24:25 contains the bits remaining in the current segment
            // r18 contains either the reset counter or the segment counter
            // r0 contains the bitmask for the current output port
            ".entry_point: \n"
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
            "movw r24, %A[bitcount_per_segment] \n" // initial set of bits in the first segment

            ".next_segment: \n"
                "ld r26, %a[bytecode_addr]+ \n" // 2 cycles, load low byte of X from bytecode
                "ld r0, %a[bytecode_addr]+ \n" // 2 cycles, load the correct bitmask from bytecode

            ".next_bit: \n" // start of the loop per bit
            
                // await a rising edge. This loop takes 3 cycles, or terminates 2, 3 or 4 cycles after a rising edge has happened.
                "sbis %[input_port], %[input_pin] \n" // 1 cycle
                    "rjmp .next_bit \n" // 2 cycles if taken, 1 if skipped (one word instr)

                // 8 cycles remaining

                "nop \n nop \n nop \n" // 3 cycles

                // toggle the output high.
                "st X, r0 \n" // 1 cycle. now we have either 5 or 13 cycles till we should go down.

                // decrement bitcount. Reset it if it reaches 0
                // this is hoised from below into the padding here to save cycles
                "sbiw r24, 1 \n" // 2 cycles, decrement bitcount. store Z flag
                "brne .reset_bitcount \n" // 2 cycles if taken, 1 if not. Based on Z flag
                    "movw r24, %A[bitcount_per_segment] \n" // 1 cycle, reset bitcount
                ".reset_bitcount: \n"

                // read if we should be high or low (6 cycles after the end of the rising_edge loop)
                "sbic %[input_port], %[input_pin] \n" // 1 cycle
                    "rjmp .one \n" // 2 cycles if taken, 1 if not.
                
                // a zero was detected.
                ".zero: \n"
                    // go low 6 cycles after the original go high
                    "st X, r0 \n" // 1 cycle.

                    // if not at the end of the bitcount loop, start scanning again.
                    "brne .next_bit \n" // 2 cycles if taken, one if not. still based on the same Z flag

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

            : [bytecode_addr]"+e"(segments)
            : [segment_count]"r"(segment_count)
            , [bitcount_per_segment]"r"(24*(uint16_t)segment_length)
            , [reset_loop_cycles]"M"(_WS2813_RESET_CYCLES / 6)
            , [input_port]"I"(input_port * 4 + 2)
            , [input_pin]"I"(input_pin)
            , [port_reg_hibyte]"I"(0x04)
            // clobbers
            : "r0", "r18", "r24", "r25", "r26", "r27"
        );
    }
}

#endif
