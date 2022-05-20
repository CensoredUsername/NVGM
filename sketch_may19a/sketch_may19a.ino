#include <avr/io.h>

// port index of the input pin being used
#define INPUT_PORT 0

// pin index of the input pin being used
#define INPUT_PIN 1

// amount of cycles that the input has to be low to register a reset condition. Supports values up to 1500.
// should be at most 1000 (50us), and at least 8us
#define RESET_CYCLES 600

// storage for segment bytecode. Don't change this.
#define MAX_SEGMENT_COUNT 255
uint8_t segment_bytecode[2 * MAX_SEGMENT_COUNT] = {0};

void configure_input() {
    // configure input pin as input, enable pullup so it doesn't float.
    PORT_t& port_ref = (&PORTA)[INPUT_PORT];
    port_ref.DIRCLR = 1 << INPUT_PIN;
    (&port_ref.PIN0CTRL)[INPUT_PIN] = 0x80;
}

void configure_segment(uint8_t index, uint8_t port, uint8_t pin) {
    // set bytecode entry.
    segment_bytecode[index] = 0x07 + 0x20 * port;
    segment_bytecode[index + 1] = 1 << pin;

    // pin init. set direction and initialize low.
    PORT_t& port_ref = (&PORTA)[index];
    port_ref.DIRSET = 1 << pin;
    port_ref.OUTCLR = 1 << pin;
}

void execute_bytecode(uint8_t segment_count, uint8_t segment_length) {
    asm volatile (
        // register allocation
        // Z (r30:r31) contains a pointer to the current entry in the bytecode
        // X (r26:r27) contains a pointer to the toggle register for the current output port
        // r25:r24 contains the bits remaining in the current segment
        // r18 contains either the reset counter or the segment counter
        // r0 contains the bitmask for the current output port
        "entry_point: \n"
        "movw r30, %A[bytecode_addr] \n" // load segment bytecode address into Z register
        "ldi r27, %[port_reg_hibyte] \n" // X high byte, high byte of all port register addresses

        // reset await loop prologue
        "ldi r18, %[reset_loop_cycles] \n"
        "reset_loop: \n" // this takes 6 cycles to loop, which at 20MHz means ~0.3us per loop
            "sbic %[input_port], %[input_pin] \n" // 1 cycle + 2 if next is skipped
            "rjmp entry_point \n" // 2 cycles
            "dec r18 \n" // 1 cycle
            "brne reset_loop \n" // 2 cycles if taken, 1 if not

        // segment loop prologue
        "mov r18, %[segment_count] \n" // amount of segments to go through

        "segment_loop: \n"
            // setup for the current segment. need to load the right address in X, and the right bitmask in R20.
            // both are read from a small datastructure. said data structure should be in the Z pointer. also iterate through it here
            "ld r26, Z+ \n" // 2 cycles, load low byte of X from bytecode
            "ld r0, Z+ \n" // 2 cycles, load the correct bitmask from bytecode

            "movw r24, %A[bitcount_per_segment] \n" // ; 1 cycle, load amount of bits in bitcount to counter
            "bit_loop: \n"

                "rising_edge: \n"
                    "sbis %[input_port], %[input_pin] \n" // 1 cycle, + 2 if next skipped
                    "rjmp rising_edge \n" // 2 cycles

                // rising edge detected, toggle current output pin
                "st X, r0 \n" // 1 cycle

                "falling_edge: \n"
                    "sbic %[input_port], %[input_pin] \n" // 1 cycle, + 2 if next skipped
                    "rjmp falling_edge \n" // ; 2 cycles

                // falling edge detected, toggle current output pin
                "st X, r0 \n" // 1 cycle

                "sbiw r24, 1 \n" // 2 cycles
                "brne bit_loop \n" // 2 cycles if taken, 1 if not 

            "dec r18 \n" // 1 cycle
            "brne segment_loop \n" // 2 cycles if taken, 1 if not 

        "rjmp entry_point \n"
        // if this proves too slow (or a variable bitcount is needed)
        // unrolling segment_loop so there's just one special loop for each segment
        // would probably be the best idea
        : /* no outputs */
        : [segment_count]"r"(segment_count)
        , [bitcount_per_segment]"r"(24*(uint16_t)segment_length)
        , [reset_loop_cycles]"M"(RESET_CYCLES / 6)
        , [bytecode_addr]"r"((uint8_t *)segment_bytecode)
        , [input_port]"I"(INPUT_PORT)
        , [input_pin]"I"(INPUT_PIN)
        , [port_reg_hibyte]"I"(0x04)
        // clobbers
        : "r0", "r18", "r24", "r25", "r26", "r27", "r30", "r31"
    );
}

void setup() {
    configure_input();
}

void loop() {
    execute_bytecode(0, 16);
}
